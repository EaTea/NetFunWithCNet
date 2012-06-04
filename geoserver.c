#define _POSIX_C_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>      // for memset()
#include <unistd.h>      // for close()
#include <sys/socket.h>  // for socket(), connect(), send(), and recv()
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>   // for sockaddr_in and inet_addr()
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

#include "ipod3230.h"
<<<<<<< HEAD

=======
#include "trilaterate.h"

#define SENDTRIES 3		// Number of consecutive zero-byte sends allowed
#define WHITESPACE " \t"	// Characters interpreted as whitespace
#define CACHESECONDS 1800	// Number of seconds before a photo is deemed old
#define CACHESIZE	 1024	// Number of coordinates for which to store photos

#define PHOTO_PORT 3230		// Port of photoserver TCP connection
#define PHOTO_IP "130.95.1.70"	// IP of photoserver TCP connection
//#define PHOTO_IP "127.0.0.1"	// IP of photoserver TCP connection for TESTING purposes

// TODO
// - Put all common functons (e.g. the Dies) into a common source file, header.


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
>>>>>>> demonstration

static const int MAXPENDING = 5; //Max. outstanding connection requests




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

<<<<<<< HEAD
=======
// Global Variables
latLon topLeft, botRight;
imageData imageCache[CACHESIZE];
view viewCache[CACHESIZE];

int cacheNum = 0;
int cacheCycler = 0;

const char* csvfilename;

/*

From beej

*/
>>>>>>> demonstration
float ntohf(uint32_t p)
{
    float f = ((p>>16)&0x7fff); // whole part
    f += (p&0xffff) / 65536.0f; // fraction
    
    if (((p>>31)&0x1) == 0x1) { f = -f; } // sign bit set
    
    return f;
}

<<<<<<< HEAD
=======
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
	
	setupPermissibleArea(topLeft.lat, topLeft.lon, botRight.lat, botRigh.lon);

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

>>>>>>> demonstration
int SetupTCPServerSocket(unsigned short port){
    
    //Create socket for incoming connections
    int sock;//Socket descriptor for server
    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))< 0)
        DieWithSystemMessage("socket() failed");
    
    //Construct local address structure
    struct sockaddr_in servAddr;                   //Local address
    memset(&servAddr, 0, sizeof(servAddr));        //Zero out structure
    servAddr.sin_family = AF_INET;                 //IPv4 address family
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);  //Any incoming interface
    servAddr.sin_port = htons(port);               //Local port
    
    //Bind to the local address
    if(bind(sock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0)
        DieWithSystemMessage("bind() failed");
    
    //Mark the socked so it will listen for incoming conncetions
    if(listen(sock, MAXPENDING) < 0)
        DieWithSystemMessage("listen() failed");
    
    return sock;
}

int AcceptTCPConnection(int servSock) {
   
    struct sockaddr_in clntAddr; //Client address
    //Set length of client address structure (in-out parameter)
    socklen_t clntAddrLen = sizeof(clntAddr);
    
    //Wait for a client to connect
    int clntSock = accept(servSock, (struct sockaddr *)&clntAddr, &clntAddrLen);
    if(clntSock < 0)
        DieWithSystemMessage("accept() failed");
    
    //clntSock is now connected to a client
    
    char clntName[INET_ADDRSTRLEN]; //String to contain client address
    if(inet_ntop(AF_INET, &clntAddr.sin_addr.s_addr,clntName, sizeof(clntName)) != NULL)
        printf("Handling client %s/%d\n", clntName, ntohs(clntAddr.sin_port));
    else
        puts("Unable to get client address");
    
    return clntSock;

}


void geoHearingProtocol(int clntSocket) {
  
   FILE *instream = fdopen(clntSocket, "r+");
   if(instream == NULL)
        DieWithSystemMessage("fdopen() failed");
   
    IPOD_SAMPLE sample;
<<<<<<< HEAD
    
    
    if( fread(&sample.nAPs, sizeof(int), 1, instream)!= 1){
        DieWithSystemMessage("fread() failed\n");
    }
    
    fread(&sample.lat, sizeof(sample.lat),1,instream);
    fread(&sample.lon, sizeof(sample.lon),1,instream);
    
    sample.nAPs = ntohl(sample.nAPs);
    sample.lat =ntohf(sample.lat);
    sample.lon =ntohf(sample.lon);
    printf("Number of APs: %d\n",sample.nAPs);
    printf("Latitude, Longitude: %f %f\n", sample.lat, sample.lon);

    
    sample.APs = (APINFO*)malloc(sample.nAPs*sizeof(APINFO));
   
    for(int i=0; i<sample.nAPs; i++){
    fread(&sample.APs[i].ssid, sizeof(char) ,32, instream);
    fread(&sample.APs[i].mac, sizeof(char) ,18, instream);
    fread(&sample.APs[i].rssi, sizeof(int32_t),1, instream);	    
    }
    fclose(instream);//close the socket connection
   
    for(int i=0; i<sample.nAPs; i++){
    printf("AP %d SSID: %s\n",i, sample.APs[i].ssid);
   	printf("AP %d MAC: %s\n",i, sample.APs[i].mac);
   	printf("AP %d RSSI: %d\n",i,sample.APs[i].rssi);
    }
=======
    
    
    if( fread(&sample.nAPs, sizeof(int), 1, instream)!= 1){
        DieWithSystemMessage("fread() failed\n");
    }
    
    fread(&sample.lat, sizeof(sample.lat),1,instream);
    fread(&sample.lon, sizeof(sample.lon),1,instream);
    
    sample.nAPs = ntohl(sample.nAPs);
    sample.lat =ntohf(sample.lat);
    sample.lon =ntohf(sample.lon);
    printf("Number of APs: %d\n",sample.nAPs);
    printf("Latitude, Longitude: %f %f\n", sample.lat, sample.lon);

    
    sample.APs = (APINFO*)malloc(sample.nAPs*sizeof(APINFO));
   
    for(int i=0; i<sample.nAPs; i++){
    fread(&sample.APs[i].ssid, sizeof(char) ,32, instream);
    fread(&sample.APs[i].mac, sizeof(char) ,18, instream);
    fread(&sample.APs[i].rssi, sizeof(int32_t),1, instream);	    
    }
    fclose(instream);//close the socket connection
   
    for(int i=0; i<sample.nAPs; i++){
    printf("AP %d SSID: %s\n",i, sample.APs[i].ssid);
   	printf("AP %d MAC: %s\n",i, sample.APs[i].mac);
   	printf("AP %d RSSI: %d\n",i,sample.APs[i].rssi);
    }
    
    processSamples(1, &sample);
  
    
    free(sample.APs);
    
    
>>>>>>> demonstration
}

void SendImage(int clntSocket){
    
<<<<<<< HEAD
    
    const char *filename = "/Users/20356245/Desktop/sampleimage.png";
    FILE *outstream = fdopen(clntSocket, "wb");
    FILE *image2send = fopen(filename, "rb");
    printf("Opened  both files\n");
    
    
    fseek(image2send, 0L, SEEK_END);
 	int imageNBytes = ftell(image2send);
    fseek(image2send, 0L, SEEK_SET);
    
    
    
=======
    const char *filename = "/Users/20356245/Desktop/sampleimage.png";
    FILE *outstream = fdopen(clntSocket, "wb");
    FILE *image2send = fopen(filename, "rb");
    printf("Opened  both files\n");
    
    //1. fread nAPs
    //2. fread *APs
    //3. fread dir
    
    
    fseek(image2send, 0L, SEEK_END);
 	int imageNBytes = ftell(image2send);
    fseek(image2send, 0L, SEEK_SET);
    
    
    
>>>>>>> demonstration
    int NETWORKimageNBytes = htonl(imageNBytes);
    if(fwrite( &NETWORKimageNBytes,sizeof(NETWORKimageNBytes),1, outstream) != 1){
        printf("fwrite() imageNBytes failed\n");
    }
    
    printf("imageNBytes Written: %d\n", imageNBytes);
    char buffer[imageNBytes];
    fread(buffer, sizeof(char), imageNBytes, image2send);
    
    printf("fopened() filename\n");
    int numbat = fwrite(buffer, sizeof(char), imageNBytes, outstream);
    if(numbat!= imageNBytes){
        printf("fwrite() FAILED!!!!: wrote %d bytes\n", numbat);
    }
    printf("fwrote() image\n");
    fclose(image2send);
    fclose(outstream);
    printf("fclosed() both streams\n");
    
}

<<<<<<< HEAD
=======
void finalize(int clntSocket, const char *filename){
  
    
      
      writeSampleToFile(csvfilename);
  
  
  
}
/*Function to determine what application protocol to implement when iPod contacts.
*/
PACKET_TYPE getPacketType(int clntSocket){
   
  PACKET_TYPE packettype;
  
  FILE *instream = fdopen(clntSocket, "r+");
  if(instream == NULL)
        DieWithSystemMessage("fdopen() failed");
  
  if( fread(&packettype, sizeof(PACKET_TYPE), 1, instream)!= 1){
        DieWithSystemMessage("fread() failed for packet type\n");
    } 
    
  fclose(instream);
  
  return packettype;
}

>>>>>>> demonstration
int main(int argc, char *argv[]){
    
    if(argc != 2)//Test for correct number of arguments
        DieWithUserMessage("Parameter(s)", "<Server Port>");
    
    in_port_t servPort = atoi(argv[1]); //First arg: local port
    
    
<<<<<<< HEAD
    //TODO: request area from photoserver and intitialise etc.
=======
    
    // Prepare topLeft / botRight information
	memset(&topLeft, 0, sizeof(topLeft));
	memset(&botRight, 0, sizeof(botRight));
	photo_getArea(&topLeft, &botRight);
	
    // Prepare image Cache
	memset(&imageCache, 0, sizeof(imageCache));
	memset(&imageCache, 0, sizeof(imageCache));	
	
	
	
    readSamplesFromFile(csvfilename);	
>>>>>>> demonstration
    
    int servSock = SetupTCPServerSocket(servPort);
        
    while(1) {        
        int clientSock = AcceptTCPConnection(servSock);
        
<<<<<<< HEAD
        
        
        
        geoAppProtocol(clientSock);
        SendImage(clientSock);
=======
	PACKET_TYPE packettype = getPacketType(clientSock);
	
	switch(packettype)
	{
	  case SAMPLE_DATA:
	       geoHearingProtocol(clientSock);
	       break;
	  case PHOTO_REQ:
	       SendImage(clientSock);
	       break;
	  case FINAL:
	       finalize(clientSock, csvfilename);
	       break;
	}
	 
>>>>>>> demonstration
    }
    
    //NOT REACHED
}
