/*
** Contains all photoserver functionality
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>       // for close(), sleep()
#include <string.h>       // for memset(), strtok()
#include <netdb.h>
#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>   // for socket(), connect(), send(), and recv()
#include <arpa/inet.h>    // for sockaddr_in and inet_addr()

#define SENDTRIES 3		  // Number of consecutive zero-byte sends allowed
#define WHITESPACE " \t"  // Characters interpreted as whitespace
#define CACHESECONDS 1800 // Number of seconds before a photo is deemed old
#define CACHESIZE	 1024 // Number of coordinates for which to store photos

#define PHOTO_PORT 3230			// Port of photoserver TCP connection
#define PHOTO_IP "130.95.1.70"	// IP of photoserver TCP connection
//#define PHOTO_IP "127.0.0.1"	// IP of photoserver TCP connection for TESTING purposes

// TODO
// - Put all common functons (e.g. the Dies) into a common source file, using header.
// - Image Caching: use global unsigned int to keep track of filenames, with filename
//   number being index into ImageData array 

typedef enum {
	FACING_N, FACING_S, FACING_E, FACING_W
} FACING;

typedef struct {
    float lat;
    float lon;
} latLon;

typedef struct {
	float lat;
	float lon;
	FACING facing;
} view;

typedef struct {
    size_t nBytes;
    time_t timeStamp;
    char fileName[25];
} imageData;

void DieWithUserMessage(const char *msg, const char *detail) {
    fputs(msg, stderr);
    fputs(": ",stderr);
    fputs(detail, stderr);
    fputc('\n', stderr);
    exit(1);
}

void DieWithSystemMessage(const char *msg) {
    perror(msg);
    exit(1);
}

// Returns a sockaddr_in structure
struct sockaddr_in prepSocketAddr (char* servIP, in_port_t servPort){

	struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    // Convert IP address from presentation (human-readable) form to network (binary) form, assign 
	int rtnVal = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);
    if(rtnVal == 0) 
		DieWithUserMessage("inet_pton() failed", "invalid address string");
    else if(rtnVal < 0)
		DieWithSystemMessage("inet_pton() failed");
    // Convert port from host byte order to short network byte order, assign
	servAddr.sin_port = htons(servPort);
   	return servAddr;
}

/*
**
*/
int sendToSocket (int sock, char* msg){
	size_t msgLen = strlen(msg);
    
	int bytesSent = 0;		// number of bytes sent
    int bytesLeft = msgLen;	// how many bytes still need to be sent
    int justSent;
	int zeroSends = 0;

	// Send all information even if over several attempts
    while(bytesSent < msgLen && zeroSends < SENDTRIES) {
        justSent = send(sock, msg+bytesSent, bytesLeft, 0);
        if (justSent < 0) {	
			char detail[50];
			sprintf(detail, "%d bytes sent before send() failed", bytesSent);
			DieWithUserMessage("send()", detail);
		}
		if (justSent == 0) {
			zeroSends ++;
			perror("Receiver could not accept any data, trying again in 3 seconds...\n");
			sleep(3);
			continue;
		}
        bytesSent += justSent;
        bytesLeft -= justSent;
		zeroSends = 0;
    }

	// Deal with and report error and edge cases
	if(zeroSends != 0) {
		char detail[50];
		sprintf(detail, "%d bytes sent before receiver could not receive any more data", bytesSent);
		DieWithUserMessage("send()", detail);
	}		
    else if(bytesSent != msgLen) {
		char detail[50];
		sprintf(detail, "%d bytes sent, %d bytes should have been sent", bytesSent, msgLen);
		DieWithUserMessage("send()", detail);
	}
	fprintf(stderr, "Sent: %s", msg);
    return justSent == -1? -1 : 0; // return -1 on failure, 0 on success		
}

/*
**
*/
int readSocketToFile (int sock, int bytesToRead, char * filename){
	
	int bytesRead = 0;				// number of bytes read
    int bytesLeft = bytesToRead;	// how many bytes still need to be read
    int justRead;
	
	// Delete file if it exists
	remove(filename);
	// Attempt to open input file with write access
	FILE* fileStream = fopen(filename, "wb");
	// Check whether pointer is null (file is invalid)
	if(fileStream == NULL) {
		perror("Cannot open file\n");
		fclose(fileStream);
		return -1;
	}
	
	// Read all information even if over several attempts
	char buffer[BUFSIZ];
    while(bytesLeft > 0) {		
        justRead = recv(sock, buffer, BUFSIZ-1, 0);

        if (justRead < 0) {	
			char detail[50];
			sprintf(detail, "%d bytes read before recv() failed", bytesRead);
			fclose(fileStream);
			DieWithUserMessage("readSocketToFile()", detail);
		}
		else if (justRead == 0){
			fclose(fileStream);
			DieWithUserMessage("recv()", "connection closed prematurely");
		}
		bytesRead += justRead;
        bytesLeft -= justRead;
		buffer[justRead] = '\0';		// Define end of received data
		fwrite(buffer, 1, justRead, fileStream);	// CHECK: this will print newlines OK right??		
    }

	// Deal with and report error and edge cases		
    if(bytesRead != bytesToRead) {
		char detail[50];
		sprintf(detail, "%d bytes read, %d bytes should have been read", bytesRead, bytesToRead);
		fclose(fileStream);
		DieWithUserMessage("readSocketToFile()", detail);
	}
	fclose(fileStream);
    return bytesRead;
}

/*
** Fills the provided buffer (OF SIZE BUFSIZ) with the message return from the photoserver for a given command
 */
int photo_command(int sock_photo, struct sockaddr_in addr_photo, char* command, char* buffer){
	do {
		sendToSocket(sock_photo, command);
		// Receive message
		fputs("Received: ", stderr);	// Can be commented out later, nice for debugging
		int numBytes = recv(sock_photo, buffer, BUFSIZ-1, 0);
		if(numBytes < 0)
			DieWithSystemMessage("recv() failed");
		else if(numBytes==0)	
			DieWithUserMessage("recv()", "connection closed prematurely");
			// And if it's closed prematurely, then what do we do?
			// (i.e. this should enter a reconnection loop)
		buffer[numBytes] = '\0';		// Define end of received data
		// Debugging
		fputs(buffer, stderr);
		fputc('\n', stderr);
		// Check for YES
		if (strstr(buffer, "YES") == NULL) {
			char detail[100];
			sprintf(detail, "Photoserver Computer Says %s", buffer);
			DieWithUserMessage("recv()", detail);
		}
	} while(strstr(buffer, "server-busy") != NULL);
	return 0;
}

/*
** Sets the provided latLons to the top left and bottom right corners of the area served by the photoserver
*/
int photo_getArea (latLon* topLeft, latLon* botRight) {
	
	// Prepare Socket Name & Address Info
	int sock_photo = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sock_photo<0) DieWithSystemMessage("socket() failed");
	struct sockaddr_in addr_photo = prepSocketAddr(PHOTO_IP, PHOTO_PORT);
	
	// Attempt to connect
    if(connect(sock_photo, (struct sockaddr *)&addr_photo, sizeof(addr_photo))<0)
        DieWithSystemMessage("connect() failed");
	
	// Send command: ask for area
	char buffer[BUFSIZ];
	photo_command(sock_photo\, addr_photo, "area \r\n", buffer);
	char* word = strtok(buffer, WHITESPACE);
	// Grab the expected four floats and record them in the provided latLon structs
	topLeft->lat = atof(word = strtok(NULL, WHITESPACE));
	topLeft->lon = atof(word = strtok(NULL, WHITESPACE));
	botRight->lat = atof(word = strtok(NULL, WHITESPACE));
	botRight->lon = atof(word = strtok(NULL, WHITESPACE));

	close(sock_photo);
	return 0;
}

/*
** 
** Prepares an imageData object for the given location / direction combination
*/
int photo_getImageData (imageData *image, view *view) {

	// Prepare Socket Name & Address Info 
	int sock_photo = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sock_photo<0) DieWithSystemMessage("socket() failed");
	struct sockaddr_in addr_photo = prepSocketAddr(PHOTO_IP, PHOTO_PORT);
	
	// Attempt to connect
    if(connect(sock_photo, (struct sockaddr *)&addr_photo, sizeof(addr_photo))<0)
        DieWithSystemMessage("connect() failed");
	
	// Prepare message
	char direction;
	if(view->facing == FACING_N) direction = 'N';
	else if(view->facing == FACING_S) direction = 'S';
	else if(view->facing == FACING_E) direction = 'E';
	else if(view->facing == FACING_W) direction = 'W';
	char msg[50];
	sprintf(msg, "view %f %f %c \r\n", view->lat, view->lon, direction);
	// Send message: ask for area	
	char buffer[BUFSIZ];
	photo_command(sock_photo, addr_photo, msg, buffer);
	
	// *** Grab the expected words and store them in the image struct ***
	// Image size in Bytes
	char* word = strtok(buffer, WHITESPACE);
	image->nBytes = atoi(word = strtok(NULL, WHITESPACE));
	
	// Prepare and fill time info struct
	time_t rawtime;
	time(&rawtime);
	struct tm *timeInfo = localtime (&rawtime);
	// Day of the week (IGNORED)
	word = strtok(NULL, WHITESPACE);
	// Day of the month
	timeInfo->tm_mday = atoi(word = strtok(NULL, WHITESPACE));
	// Month of the year
	int mth = 0;
	char* months[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	word = strtok(NULL, WHITESPACE);
	while(mth < 12){
		if(strcmp(word, months[mth]) == 0) break;
		mth++;
	}
	if(mth == 12) DieWithUserMessage("photo_getImageData()", "Could not identify month"); 
	timeInfo->tm_mon = mth;
	// Year
	timeInfo->tm_year = atoi(word = strtok(NULL, WHITESPACE))+100;
	// Hour:Minute:Second
	word = strtok(NULL, WHITESPACE);
	sscanf(word, "%d:%d:%d", &timeInfo->tm_hour, &timeInfo->tm_min, &timeInfo->tm_sec);
	
	time_t t = mktime(timeInfo);
	image->timeStamp = t;
	
	// --- Read the image, writing it to a file
	readSocketToFile (sock_photo, image->nBytes, image->fileName);
	close(sock_photo);
	return 0;
}

int isValidLoc(latLon topLeft, latLon botRight, view point) {
	if (point.lat > topLeft.lat || point.lat < botRight.lat ||
	point.lon < topLeft.lon || point.lon > botRight.lon) return -1;
	else return 0;	
}

/*
** Returns the index of the point in the cache, or -1 on error
 */
int cacheImage(view vcache[], imageData icache[], int *cacheNum, int *cacheCycler, view point) {
	
	// Check cache...
	int index = 0;
	while(index < *cacheNum){
		if( vcache[index].lat == point.lat && vcache[index].lon == point.lon
		&& vcache[index].facing == point.facing) return index;
		index++;
	}	    
	if (index < *cacheNum) {	// Cached image exists: is it recent enough?
		time_t now;
		time(&now);
		if( ( (int) ( difftime(now, icache[index].timeStamp) ) ) < CACHESECONDS)
			// Existing image is recent enough: don't bother fetching a newer one
			return index;		
	} else {
		// No existing image: check if room in cache for more
		if(*cacheNum == CACHESIZE) {
			index = *cacheCycler;	// Overwrite "oldest" (by index) cached item
			*cacheCycler = (*cacheCycler+1) % CACHESIZE;
		}
		// Cache view and image info
		vcache[index].lat = point.lat;
		vcache[index].lon = point.lon;
		vcache[index].facing = point.facing;
		char nameBuffer[25];
		sprintf(nameBuffer, "PS-cache-%d.PNG", index);
		strcpy(icache[index].fileName, nameBuffer);
		*cacheNum = *cacheNum +1;
	}
	// Either the existing image was outdated, or none existed 
	photo_getImageData (&icache[index], &vcache[index]);
	return index;	
}

int main(int argc, char *argv[]){
    
	// Prepare topLeft / botRight information
	latLon topLeft, botRight;
    memset(&topLeft, 0, sizeof(topLeft));
	memset(&botRight, 0, sizeof(botRight));
	photo_getArea(&topLeft, &botRight);
	
	// Prepare image Cache
	imageData imageCache[CACHESIZE];
	memset(&imageCache, 0, sizeof(imageCache));
	view viewCache[CACHESIZE];
	memset(&imageCache, 0, sizeof(imageCache));
	int cacheNum = 0;
	int cacheCycler = 0;
	
	// Prepare sample data (or in reality: requested view)
	view testPoint;
	testPoint.lat = -31.97880;
	testPoint.lon = 115.81803;
	testPoint.facing = FACING_N;
	
	// Check that requested coordinates are within area served by photoserver
	if(isValidLoc(topLeft, botRight, testPoint) == -1) {
		// NOTIFY CLIENT OF THIS...
		// ...
		// (Debug)
		fputs("testPoint outside of bounds", stderr);
	} else {
		// Cache image
		int index = cacheImage(viewCache, imageCache, &cacheNum, &cacheCycler, testPoint);
		// SEND IMAGE TO CLIENT	
		
	}
	// NOTE: location-based caching should also be done on the iOS app, 
	// with the addition of checks for significant movement to reduce unnecessary transmissions
	
    exit(0);
}