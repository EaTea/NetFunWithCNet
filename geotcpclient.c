/*
** Contains all photoserver functionality
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>		// for close(), sleep()
#include <string.h>		// for memset(), strtok()
#include <netdb.h>
#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>		// for socket(), connect(), send(), and recv()
#include <arpa/inet.h>		// for sockaddr_in and inet_addr()

#define SENDTRIES 3		// Number of consecutive zero-byte sends allowed
#define WHITESPACE " \t"	// Characters interpreted as whitespace
#define CACHESECONDS 1800	// Number of seconds before a photo is deemed old
#define CACHESIZE	 1024	// Number of coordinates for which to store photos

#define PHOTO_PORT 3230		// Port of photoserver TCP connection
#define PHOTO_IP "130.95.1.70"	// IP of photoserver TCP connection
//#define PHOTO_IP "127.0.0.1"	// IP of photoserver TCP connection for TESTING purposes

// TODO
// - Put all common functons (e.g. the Dies) into a common source file, header.

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

// Global Variables
latLon topLeft, botRight;
imageData imageCache[CACHESIZE];
view viewCache[CACHESIZE];
int cacheNum = 0;
int cacheCycler = 0;

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
	fprintf(stderr, "Sent: %s", msg);	// DEBUG
    return justSent == -1? -1 : 0; 		// return -1 on failure, 0 on success		
}

/*
** Fills the provided buffer (OF SIZE BUFSIZ) with the message return from the photoserver for a given command
** Returns the size of the message INCLUDING the end-of-line sequence and NOT INCLUDING any binary data after it
** The returned value is thus equal to the index into the first character of data after the end of line sequence
 */
int photo_command(int sock_photo, struct sockaddr_in addr_photo, char* command, char* buffer){
	int size;
	do {
		sendToSocket(sock_photo, command);
		// Receive message
		fputs("Received: ", stderr);	// DEBUG
		int numBytes = recv(sock_photo, buffer, BUFSIZ-1, 0);
		if(numBytes < 0)
			DieWithSystemMessage("recv() failed");
		else if(numBytes==0)	
			DieWithUserMessage("recv()", "connection closed prematurely");
			// And if it's closed prematurely, then what do we do?
			// (i.e. this should enter a reconnection loop)
		// Debugging
		size = 0;
		while(buffer[size] != '\r') size++;
		size++;
		buffer[size] = '\0';	// Delimit end of message so it can be printed
		fputs(buffer, stderr);
		fputs("\n\n", stderr);
		// Check for YES
		if (strstr(buffer, "YES") == NULL) {
			char detail[100];
			sprintf(detail, "Photoserver Computer Says %s", buffer);
			DieWithUserMessage("recv()", detail);
		}
	} while(strstr(buffer, "server-busy") != NULL);
	return size +1;
}

/*
** Sets the provided latLons to the top left and bottom right corners of the area served by the photoserver
*/
int photo_getArea () {
	
	// Prepare Socket Name & Address Info
	int sock_photo = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sock_photo<0) DieWithSystemMessage("socket() failed");
	struct sockaddr_in addr_photo = prepSocketAddr(PHOTO_IP, PHOTO_PORT);
	
	// Attempt to connect
    if(connect(sock_photo, (struct sockaddr *)&addr_photo, sizeof(addr_photo))<0)
        DieWithSystemMessage("connect() failed");
	
	// Send command: ask for area
	char buffer[BUFSIZ];
	photo_command(sock_photo, addr_photo, "area \r\n", buffer);
	char* word = strtok(buffer, WHITESPACE);
	// Grab the expected four floats and record them in the provided latLon structs
	topLeft.lat = atof(word = strtok(NULL, WHITESPACE));
	topLeft.lon = atof(word = strtok(NULL, WHITESPACE));
	botRight.lat = atof(word = strtok(NULL, WHITESPACE));
	botRight.lon = atof(word = strtok(NULL, WHITESPACE));

	close(sock_photo);
	return 0;
}

/*
** 
** Prepares an imageData object for the given location / direction combination
** Returns -1 if no image for given location, 0 otherwise
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
	int start = photo_command(sock_photo, addr_photo, msg, buffer);
	
	// *** Grab the expected words and store them in the image struct ***
	// Image size in Bytes
	char* word = strtok(buffer, WHITESPACE);	// "YES"
	image->nBytes = atoi(word = strtok(NULL, WHITESPACE));
	
	// Check if image is available for this view
	if(image->nBytes == 0) return -1; // Because photoserver has said "YES 0\r\n" or similar
	
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
	
	int bytesRead = 0;						// number of bytes read
    int bytesLeft = image->nBytes;			// how many bytes still need to be read
    int justRead = sizeof(buffer) - start-1;	// in case any binary data was left from previous
											// recv call in photo_command (checked below)
											
	// Delete file if it exists
	remove(image->fileName);
	// Attempt to open input file with write access
	FILE* fileStream = fopen(image->fileName, "wb");
	// Check whether pointer is null (file is invalid)
	if(fileStream == NULL) {
		perror("Cannot open file\n");
		fclose(fileStream);
		return -2;
	}
	
	// Check if image data already exists in the buffer by examining buffer for PNG header signature
	if(buffer[start+1] == 'P' && buffer[start+2] == 'N' && buffer[start+3] == 'G') {
	
		fprintf(stderr, "First message was %d long, rest is start of PNG, %d long.\n", start, justRead);
		
		// Start from after the End-of-line sequence 
		bytesRead += justRead;
		bytesLeft -= justRead;
		fwrite(buffer+start, 1, justRead, fileStream);
	}
	
	// Read all information even if over several attempts
    while(bytesLeft > 0) {		
        justRead = recv(sock_photo, buffer, BUFSIZ-1, 0);

        if (justRead < 0) {	
			char detail[50];
			sprintf(detail, "%d bytes read before recv() failed", bytesRead);
			fclose(fileStream);
			DieWithUserMessage("photo_getImageData()", detail);
		}
		else if (justRead == 0){
			fclose(fileStream);
			DieWithUserMessage("recv()", "connection closed prematurely");
		}
		bytesRead += justRead;
        bytesLeft -= justRead;
		fwrite(buffer, 1, justRead, fileStream);		
    }

	// Deal with and report error and edge cases		
    if(bytesRead != image->nBytes) {
		char detail[50];
		sprintf(detail, "%d bytes read, %d bytes should have been read", bytesRead, image->nBytes);
		fclose(fileStream);
		DieWithUserMessage("photo_getImageData()", detail);
	}
	fclose(fileStream);	
	close(sock_photo);
	return 0;
}

/*
** Returns the index of the point in the cache, -1 if no image for given location, or -2 for other erros 
 */
int cacheImage(view point) {

	// Check for valid location
	if (point.lat > topLeft.lat || point.lat < botRight.lat ||
	point.lon < topLeft.lon || point.lon > botRight.lon) {
		fputs("testPoint outside of bounds", stderr);
		return -1;
	}

	// Check cache...
	int index = 0;
	while(index < cacheNum){
		if( viewCache[index].lat == point.lat && viewCache[index].lon == point.lon
		&& viewCache[index].facing == point.facing) break;
		index++;
	}	    
	if (index < cacheNum) {	// Cached image exists: is it recent enough?
		fprintf(stderr, "Cached image %s found, recent enough?... ", imageCache[index].fileName);
		time_t now;
		time(&now);	
		if ( (int) difftime(now, imageCache[index].timeStamp) < CACHESECONDS ) {
			fputs("YES\n\n", stderr);
			// Existing image is recent enough: don't bother fetching a newer one
			return index;
		}
		fputs("NO\n", stderr);
	} else {
		fprintf(stderr, "Cached image NOT found, caching...\n");
		// No existing image: check if room in cache for more
		if(cacheNum == CACHESIZE) {
			index = cacheCycler;	// Overwrite "oldest" (by index) cached item
			cacheCycler = (cacheCycler+1) % CACHESIZE;
		}
		// Cache view and image info
		viewCache[index].lat = point.lat;
		viewCache[index].lon = point.lon;
		viewCache[index].facing = point.facing;
		char nameBuffer[25];
		sprintf(nameBuffer, "PS-cache-%d.PNG", index);
		strcpy(imageCache[index].fileName, nameBuffer);
		cacheNum++;
	}
	// Either the existing image was outdated, or none existed: get image data and deal with errors
	int err = photo_getImageData (&imageCache[index], &viewCache[index]);
	if (err  < 0) {
		// No image existed: clear data structures and cache slot
		memset(&imageCache[index], sizeof(imageCache[index]), 0);
		memset(&viewCache[index], sizeof(viewCache[index]), 0);
		cacheNum = cacheNum -1;
		return err;
	}
	else return index;
}

int main(int argc, char *argv[]){
    
	// Prepare topLeft / botRight information
    memset(&topLeft, 0, sizeof(topLeft));
	memset(&botRight, 0, sizeof(botRight));
	photo_getArea(&topLeft, &botRight);
	
	// Prepare image Cache
	memset(&imageCache, 0, sizeof(imageCache));
	memset(&imageCache, 0, sizeof(imageCache));
	
	// Prepare sample data (or in reality: requested view)
	view testPoint;
	testPoint.lat = -31.97880;
	testPoint.lon = 115.81803;
	testPoint.facing = FACING_N;
	view testPoint2;
	testPoint2.lat = -31.981615;
	testPoint2.lon = 115.81846;
	testPoint2.facing = FACING_W;
	view testPoint3;
	testPoint3.lat = -31.977647;
	testPoint3.lon = 115.816904;
	testPoint3.facing = FACING_S;
	
	fputs("***\tTesting Point 1\n", stderr);	// DEBUG
	int index = cacheImage(testPoint);
	// SEND IMAGE TO CLIENT	(OR MESSAGE ABOUT NO IMAGE IF INDEX = -1; HOW TO DEAL WITH INABILITY TO OPEN FILE? (-2) )	(OR MESSAGE ABOUT NO IMAGE IF INDEX = -1; HOW TO DEAL WITH INABILITY TO OPEN FILE? (-2) )
	// ... SEND CODE HERE

	// Check point 2
	fputs("***\tTesting Point 2: Should have no image\n", stderr);	// DEBUG
	cacheImage(testPoint2);
	
	sleep(5);
	
	// Check point 1 again to see that cache is working
	fputs("***\tTesting Point 1: Should be cached\n", stderr);	// DEBUG
	cacheImage(testPoint);

	// Check point 3
	fputs("***\tTesting Point 3\n", stderr);	// DEBUG
	cacheImage(testPoint3);
	
	// Check point 3 again to see that cache is working
	fputs("***\tTesting Point 3: should be cached\n", stderr);	// DEBUG
	cacheImage(testPoint3);
	
    exit(0);
}