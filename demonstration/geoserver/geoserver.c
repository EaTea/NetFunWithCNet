/*
 * Version 05/06 15:00
 */


#define _POSIX_C_SOURCE 1

#ifndef STDIO_INCLUDE_PROXY_HEADER
#define STDIO_INCLUDE_PROXY_HEADER
#include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>	// for memset()
#include <unistd.h>	// for close()
#include <sys/socket.h>	// for socket(), connect(), send(), and recv()
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>	 // for sockaddr_in and inet_addr()
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

#include "ipod3230.h"
#include "locate.h"

#define SENDTRIES 3	// Number of consecutive zero-byte sends allowed
#define WHITESPACE " \t"	// Characters interpreted as whitespace
#define CACHESECONDS 1800	// Number of seconds before a photo is deemed old
#define CACHESIZE	 1024	// Number of coordinates for which to store photos

#define PHOTO_PORT 3230	// Port of photoserver TCP connection
#define PHOTO_IP "130.95.1.70"	// IP of photoserver TCP connection
//#define PHOTO_IP "127.0.0.1"	// IP of photoserver TCP connection for TESTING purposes

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

// Global Variables
latLon topLeft, botRight;
imageData imageCache[CACHESIZE];
view viewCache[CACHESIZE];


in_port_t servPort;
int servSock;
int clientSock;
int cacheNum = 0;
int cacheCycler = 0;

const char* argv[2];

/*

From beej

 */
float ntohf(uint32_t p)
{
	float f = ((p>>16)&0x7fff); // whole part
	f += (p&0xffff) / 65536.0f; // fraction

	if (((p>>31)&0x1) == 0x1) { f = -f; } // sign bit set

	return f;
}

// Returns a sockaddr_in structure
struct sockaddr_in prepSocketAddr (char* servIP, in_port_t servPort){

	struct sockaddr_in servAddr;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	// Convert IP address from presentation (human-readable) form to network (binary) form, assign
	int rtnVal = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);
	if(rtnVal == 0) {
		printf("inet_pton() failed: %s\n", "invalid address string");
	}
	else if(rtnVal < 0) {
		printf("inet_pton() failed\n");
	}
	// Convert port from host byte order to short network byte order, assign
	servAddr.sin_port = htons(servPort);
	return servAddr;
}

/*
 * Sends all information in the char* msg to socket sock, trying up to SENDTRIES failed attempts in a row
 */
int sendToSocket (int sock, char* msg){
	size_t msgLen = strlen(msg);

	int bytesSent = 0;	// number of bytes sent
	int bytesLeft = msgLen;	// how many bytes still need to be sent
	int justSent;
	int zeroSends = 0;

	// Send all information even if over several attempts
	while(bytesSent < msgLen && zeroSends < SENDTRIES) {
		justSent = send(sock, msg+bytesSent, bytesLeft, 0);
		if (justSent < 0) {
			char detail[50];
			sprintf(detail, "%d bytes sent before send() failed", bytesSent);
			printf("send(): %s\n", detail);
			return -1;
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
		printf("send(): %s\n", detail);
		return -1;
	}
	else if(bytesSent != msgLen) {
		char detail[50];
		sprintf(detail, "%d bytes sent, %d bytes should have been sent", bytesSent, (int)msgLen);
		printf("send(): %s\n", detail);
		return -1;
	}
	fprintf(stderr, "Sent: %s", msg);	// DEBUG
	return justSent == -1? -1 : 0; 	// return -1 on failure, 0 on success
}

/*
 * Fills the provided buffer (OF SIZE BUFSIZ) with the message returned from the photoserver for a given command.
 * Returns the size of the message INCLUDING the end-of-line sequence and NOT INCLUDING any binary data after it
 * The returned value is thus equal to the index into the first character of data after the end of line sequence.
 * Will return -1 in the event of an error
 */
int photo_command(int sock_photo, struct sockaddr_in addr_photo, char* command, char* buffer){
	int size;
	do {
		sendToSocket(sock_photo, command);
		// Receive message
		fputs("Received: ", stderr);	// DEBUG
		int numBytes = recv(sock_photo, buffer, BUFSIZ-1, 0);
		if(numBytes < 0) {
			printf("recv() failed\n");
			return -1;
		}
		else if(numBytes==0) {
			printf("recv(): %s\n", "connection closed prematurely");
			// And if it's closed prematurely, then what do we do?
			// (i.e. should this enter a reconnection loop?)
			return -1;
		}

		// Find end of line sequence
		size = 0;
		while(buffer[size] != '\r') size++;
		size++;
		buffer[size] = '\0';	// Delimit end of message so it can be printed

		fputs(buffer, stderr);	// Debugging
		fputs("\n\n", stderr);	// Debugging

		// Check for YES
		if (strstr(buffer, "YES") == NULL) {
			char detail[100];
			sprintf(detail, "Photoserver Computer Says %s", buffer);
			printf("recv(): %s\n", detail);
			return -1;
		}
	} while(strstr(buffer, "server-busy") != NULL);	// Attempt to communicate until photoserver is not busy
	return size +1;
}

/*
 * Sets the global latLons topLeft and botRight to the top left and bottom right corners of the area
 * served by the photoserver
 */
int photo_getArea () {

	// Prepare Socket Name & Address Info
	int sock_photo = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sock_photo<0) {
		printf("socket() for photoserver failed\n");
	}
	struct sockaddr_in addr_photo = prepSocketAddr(PHOTO_IP, PHOTO_PORT);

	// Attempt to connect
	if(connect(sock_photo, (struct sockaddr *)&addr_photo, sizeof(addr_photo))<0) {
		printf("connect() to photoserver failed\n");
		return -1;
	}

	// Send command: ask for area
	char buffer[BUFSIZ];
	if(photo_command(sock_photo, addr_photo, "area \r\n", buffer)<0) return -1;
	char* word = strtok(buffer, WHITESPACE);
	// Grab the expected four floats and record them in the provided latLon structs
	topLeft.lat = atof(word = strtok(NULL, WHITESPACE));
	topLeft.lon = atof(word = strtok(NULL, WHITESPACE));
	botRight.lat = atof(word = strtok(NULL, WHITESPACE));
	botRight.lon = atof(word = strtok(NULL, WHITESPACE));

	setupPermissibleArea(topLeft.lat, topLeft.lon, botRight.lat, botRight.lon);

	close(sock_photo);
	return 0;
}

/*
 * Prepares an imageData object for the given location / direction combination
 * Returns -1 if no image for given location, 0 otherwise
 * Returns -2 if an internal error occurs (i.e. call of another function failed)
 */
int photo_getImageData (imageData *image, view *view) {

	// Prepare Socket Name & Address Info
	int sock_photo = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sock_photo<0) {
		fputs("socket() to obtain photo failed\n", stderr);
		return -2;
	}
	struct sockaddr_in addr_photo = prepSocketAddr(PHOTO_IP, PHOTO_PORT);

	// Attempt to connect
	if(connect(sock_photo, (struct sockaddr *)&addr_photo, sizeof(addr_photo))<0) {
		fputs("connect() to photoserver failed\n", stderr);
		return -2;
	}

	// Prepare message to photoserver
	char direction;
	if(view->facing == FACING_N) direction = 'N';
	else if(view->facing == FACING_S) direction = 'S';
	else if(view->facing == FACING_E) direction = 'E';
	else if(view->facing == FACING_W) direction = 'W';
	char msg[50];
	sprintf(msg, "view %f %f %c \r\n", view->lat, view->lon, direction);
	// Send message: ask for image
	char buffer[BUFSIZ];
	int start = photo_command(sock_photo, addr_photo, msg, buffer);
	if(start < 0) return -2;

	//  ** Grab the expected words and store them in the image struct  **
	// Image size in Bytes
	char* word = strtok(buffer, WHITESPACE);	// "YES"
	image->nBytes = atoi(word = strtok(NULL, WHITESPACE));

	// Check if image is available for this view
	if(image->nBytes == 0) return -1; // Because photoserver has said "YES 0\r\n" (whitespace may vary)

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
	if(mth == 12) {
		printf("photo_getImageData(): %s\n", "Could not identify month");
		return -2;
	}
	timeInfo->tm_mon = mth;
	// Year
	timeInfo->tm_year = atoi(word = strtok(NULL, WHITESPACE))+100;
	// Hour:Minute:Second
	word = strtok(NULL, WHITESPACE);
	sscanf(word, "%d:%d:%d", &timeInfo->tm_hour, &timeInfo->tm_min, &timeInfo->tm_sec);

	time_t t = mktime(timeInfo);
	image->timeStamp = t;

	// --- Read the image, writing it to a file

	int bytesRead = 0;							// number of bytes read
	int bytesLeft = image->nBytes;				// how many bytes still need to be read
	int justRead = sizeof(buffer) - start-1;	// in case any binary data was left from previous
												// recv call in photo_command (checked below)

	// Delete file if it exists (caching system ensures that this function will only be called
	// if no image exists for the given VIEW, meaning that if a file by this name exists, it must
	// contain another image which is being removed from the cache)
	remove(image->fileName);
	// Attempt to open input file with write access
	FILE* fileStream = fopen(image->fileName, "wb");
	// Check whether pointer is null (file is invalid)
	if(fileStream == NULL) {
		perror("Cannot open file\n");
		fclose(fileStream);
		return -2;
	}

	// Check if image data already started in the buffer by examining buffer for PNG header signature
	if(buffer[start+1] == 'P' && buffer[start+2] == 'N' && buffer[start+3] == 'G') {
		fprintf(stderr, "First message was %d long, rest is start of PNG, %d long.\n", start, justRead);	// Debugging
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
			printf("photo_getImageData(): %s\n", detail);
		}
		else if (justRead == 0){
			fclose(fileStream);
			printf("recv(): %s\n", "connection closed prematurely");
		}
		bytesRead += justRead;
		bytesLeft -= justRead;
		fwrite(buffer, 1, justRead, fileStream);
	}

	// Deal with and report error and edge cases
	if(bytesRead != image->nBytes) {
		char detail[50];
		sprintf(detail, "%d bytes read, %d bytes should have been read", bytesRead, (int)image->nBytes);
		fclose(fileStream);
		printf("photo_getImageData(): %s\n", detail);
	}
	fclose(fileStream);
	close(sock_photo);
	return 0;
}

/*
 * Returns the index of the view 'point' in the cache, -1 if no image for given location,
 * or -2 if an internal error occurs (i.e. call of another function failed)
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
		fprintf(stderr, "Cached image %s found, recent enough?... ", imageCache[index].fileName);	// Debugging
		time_t now;
		time(&now);
		if ( (int) difftime(now, imageCache[index].timeStamp) < CACHESECONDS ) {
		fputs("YES\n\n", stderr);	// Debugging
		// Existing image is recent enough: don't bother fetching a newer one
		return index;
		}
		fputs("NO\n", stderr);	// Debugging
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
	if (err	< 0) {
		// No image existed: clear data structures and cache slot
		memset(&imageCache[index], sizeof(imageCache[index]), 0);
		memset(&viewCache[index], sizeof(viewCache[index]), 0);
		cacheNum = cacheNum -1;
		return err;
	}
	else return index;
}


/*
 * This function will initialise the Geoserver socket, prep the memory, bind it,
 * and then set it to listening.
 */
int SetupTCPServerSocket(unsigned short port){

	//Create socket for incoming connections
	int sock;//Socket descriptor for server
	if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))< 0)
		DieWithSystemMessage("socket() failed");

	int optVal =1; socklen_t optLen= sizeof(optVal);
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *)&optVal, optLen);

	//Construct local address structure
	struct sockaddr_in servAddr;					 //Local address
	memset(&servAddr, 0, sizeof(servAddr));		//Zero out structure
	servAddr.sin_family = AF_INET;				 //IPv4 address family
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);	//Any incoming interface
	servAddr.sin_port = htons(port);				 //Local port

	//Bind to the local address
	if(bind(sock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0)
		DieWithSystemMessage("bind() failed");

	//Mark the socket so it will listen for incoming connections
	if(listen(sock, MAXPENDING) < 0)
		DieWithSystemMessage("listen() failed");

	return sock;
}


/*
 *  This function will accept a client connection on the specified SERVER socket, returning the CLIENT socket
 */
int AcceptTCPConnection(int servSock) {

	struct sockaddr_in clntAddr; //Client address
	//Set length of client address structure (in-out parameter)
	socklen_t clntAddrLen = sizeof(clntAddr);

	//Wait for a client to connect
	int clntSock = accept(servSock, (struct sockaddr *)&clntAddr, &clntAddrLen);
	if(clntSock < 0) {
		printf("accept() failed");
		return -1;
	}

	//clntSock is now connected to a client

	char clntName[INET_ADDRSTRLEN]; //String to contain client address
	if(inet_ntop(AF_INET, &clntAddr.sin_addr.s_addr,clntName, sizeof(clntName)) != NULL)
		printf("Handling client %s/%d\n", clntName, ntohs(clntAddr.sin_port));
	else
		puts("Unable to get client address");

	return clntSock;

}

/*
 * This is the application protocol for when the iPod presses "Now Hearing". It involves
 * storing the number of Access Points heard and then reading in the APINFO, and then sending
 * the information to the triangulation component for processing.
 *
 * Takes a CLIENT socket (connected to an iPod client app), and also serializes to the given filename
 */
void geoHearingProtocol(int clntSocket, const char *filename) {

	FILE *instream = fdopen(clntSocket, "r+"); //file wrapper for client socket
	if(instream == NULL){
		printf("fdopen() failed");
		return;
	 }

	IPOD_SAMPLE sample;

	//couldn't read sample number of APs
	if( fread(&sample.nAPs, sizeof(int), 1, instream)!= 1){
		printf("fread() nAPs failed\n");
		return;
	}
	uint32_t networkLat, networkLon;
	fread(&networkLat, sizeof(networkLat),1,instream);
	fread(&networkLon, sizeof(networkLon),1,instream);

	sample.nAPs = ntohl(sample.nAPs);								// Network to host byte order conversion
	sample.lat = ntohf(networkLat);
	sample.lon = ntohf(networkLon);
	printf("Number of APs: %d\n",sample.nAPs);						// Verbose / debugging
	printf("Latitude, Longitude: %f %f\n", sample.lat, sample.lon); // Verbose / debugging

	int a , b, c;													//temporary variables for sample corruption checking
	sample.APs = (APINFO*)malloc(sample.nAPs*sizeof(APINFO));		//allocation memory for the APINFO
	for(int i=0; i<sample.nAPs; i++){
		a=fread(&sample.APs[i].ssid, sizeof(char) ,32, instream);	//No need for host and network byte order transformations
		b=fread(&sample.APs[i].mac, sizeof(char) ,18, instream);	//since we are reading in one byte at a time over a TCP connection.
		c=fread(&sample.APs[i].rssi, sizeof(int32_t),1, instream);
		if(a!=32 || b!=18 || c!=1){
			printf("Could not read sample %d\n",i);
			//sample couldn't be read, don't continue reading it in and assume rest of the sample is corrupt
			free(sample.APs);
			return;
		}
	}
	fclose(instream);			//close the socket connection

	for(int i=0; i<sample.nAPs; i++){
		printf("AP %d SSID: %s\n",i, sample.APs[i].ssid);
		printf("AP %d MAC: %s\n",i, sample.APs[i].mac);
		printf("AP %d RSSI: %d\n",i,sample.APs[i].rssi);
	}

	processSamples(1, &sample);	//triangulation/trilateration processing of sample

	free(sample.APs);

	writeSamplesToFile(filename); //Write information to file.

}
/*
 * This function is the application protocol for when the iPod implements "Now Facing".
 * It will read in the information from the iPod and then query the previous photoserver functions.
 * Once the photoserver responds, the function will then send the appropriate information to the iPod,
 * and, if necessary, pass an image to it.
 *
 * Takes a CLIENT socket (connected to an iPod client app)
 */
void SendImage(int clntSocket){

	FILE *outstream = fdopen(clntSocket, "r");
	if(outstream == NULL){
		//if can't open an outstream, kill program?
		//perhaps in hindsight this should simply return at this point
		printf("FDOPEN FAILED TO OPEN\n");
		return;
	}

	//1. fread nAPs
	int numAPs;
	fread(&numAPs, sizeof(int), 1, outstream);
	numAPs = ntohl(numAPs);
	printf("number of APs: %d\n", numAPs);

	//2. fread *APs
	APINFO *APs = (APINFO *)malloc(numAPs*sizeof(APINFO));
	for(int i=0; i<numAPs; i++){
		fread(&APs[i].ssid, sizeof(char) ,32, outstream);
		fread(&APs[i].mac, sizeof(char) ,18, outstream);
		fread(&APs[i].rssi, sizeof(int32_t),1, outstream);
	}
	//3. fread dir
	FACING dir;
	int n = fread(&dir, sizeof(char), 4, outstream);
	printf("read dir passed: %d\n", n);


	fclose(outstream);
	clientSock = AcceptTCPConnection(servSock);
	if(clientSock == -1) return;
	FILE *outstream2 = fdopen(clientSock, "w");

	//4. processLocation
	float lat;
	float lon;

	//process location fails and returns 0 in the event of failure
	int k = processLocation(numAPs, APs, &lat, &lon);
	printf("processLocation: %d\n", k);
	if(k != 0){
		printf("Could not Trilaterate: %d\n", k);
		int noBytesToSend = 0;
		noBytesToSend = htonl(noBytesToSend);
		if(fwrite(&noBytesToSend,sizeof(int),1, outstream2) != 1){
			printf("fwrite() imageNBytes failed\n");
		}
	} else {
		//calculation was successful, we have a location available for this position
		//5. Contact Photoserver

		view imagePoint;
		imagePoint.lat = lat;
		imagePoint.lon = lon;
		imagePoint.facing = dir;
		printf("Estimated Location: %f %f with direction %d\n",lat,lon,dir);	// Verbose / debugging
		//does this image exist in the cache? If not, make it so (insert it into the cache) and
		//return its index in the cache
		int index = cacheImage(imagePoint);

		const char *filename = imageCache[index].fileName;
		FILE *image2send = fopen(filename, "rb");
		if(image2send == NULL){
			//could not find an image to send
			int noBytesToSend = 0;
			noBytesToSend = htonl(noBytesToSend);
			if(fwrite( &noBytesToSend,sizeof(noBytesToSend),1, outstream2) != 1){
				printf("fwrite() imageNBytes failed\n");
			}

		} else {


			//Calculate size of PNG: unnecessary since its in the image Cache
			/*
			fseek(image2send, 0L, SEEK_END);
			int imageNBytes = ftell(image2send);
			fseek(image2send, 0L, SEEK_SET);
			*/

			int imageNBytes = imageCache[index].nBytes;
			int NETWORKimageNBytes = htonl(imageNBytes);

			if(fwrite( &NETWORKimageNBytes,sizeof(NETWORKimageNBytes),1, outstream2) != 1) {
				printf("fwrite() imageNBytes failed\n");
				fclose(image2send);
			} else {
				//send the image
				printf("imageNBytes Written: %d\n", imageNBytes);
				char buffer[imageNBytes];
				fread(buffer, sizeof(char), imageNBytes, image2send);

				printf("fopened() filename\n");
				int numbat = fwrite(buffer, sizeof(char), imageNBytes, outstream2);
				if(numbat!= imageNBytes) {
					printf("fwrite() FAILED!!!!: wrote %d bytes\n", numbat);
				}
				printf("fwrote() image\n");
				fclose(image2send);
				printf("fclose() the image stream\n");
			}
		}
	}

	//close and remake TCP connection
	fclose(outstream2);
	printf("fclose() the outstream\n");
}

//Hoped to write samples to file when iPod finalized but could not in time
/*void finalize(int clntSocket, const char *filename){

	writeSamplesToFile(filename);

} */



/*Function to determine what application protocol to implement when iPod contacts.
The PACKET_TYPE typedef struct is defined in ipod3230.h
 */
PACKET_TYPE getPacketType(int clntSocket){

	PACKET_TYPE packettype;

	//open connection
	FILE *instream = fdopen(clntSocket, "r+");

	//no stream could be created from this socket
	if(instream == NULL){
		printf("fdopen() failed");
		clientSock = AcceptTCPConnection(servSock);
		return NOPACKET;
	}

	//could not read a packet successfully, close the stream and return
	//the NOPACKET value to indicate this
	if(fread(&packettype, sizeof(PACKET_TYPE), 1, instream)!= 1){
		printf("fread() failed for packet type");
		fclose(instream);
		//after fclosing the stream should be closed, perhaps the socket too even though the stream
		//failed? if so, would need to uncomment the following:
		//clientSock = AcceptTCPConnection(servSock);	 //Once again, the client socket needs to be remade
		// since fclose kills it.
		return NOPACKET;
	}
	packettype = (PACKET_TYPE)ntohl(packettype);
	printf("Packet type is: %d, %lu\n", packettype, sizeof(packettype));
	fclose(instream);
	if(packettype != NOPACKET && packettype != FINAL)
		clientSock = AcceptTCPConnection(servSock);	 //Once again, the client socket needs to be remade
	// since fclose kills it.
	return packettype;
}



int main(int argc, char *argv[]){

	if(argc != 3)//Test for correct number of arguments
		DieWithUserMessage("Parameter(s)", "<Server Port> <File>");

	in_port_t servPort = atoi(argv[1]); //First arg: local port



	// Prepare topLeft / botRight information
	memset(&topLeft, 0, sizeof(topLeft));
	memset(&botRight, 0, sizeof(botRight));
	int count = 0;
	int photoRet;
	while((photoRet = photo_getArea()) < 0 && count < SENDTRIES) {
		sleep(3);
		count++;
	}
	if(photoRet < 0) {
		DieWithUserMessage("photo_getArea()","could not contact photoserver");
	}

	// Prepare image Cache
	memset(&imageCache, 0, sizeof(imageCache));

	//argv[2] should be the filename that the geoserver is reading from and writing to.
	readSamplesFromFile(argv[2]);	//Second arg: File to write sample data to.

	servSock = SetupTCPServerSocket(servPort); //Server Socket is made and starts to listen

	while(1) {

		clientSock = AcceptTCPConnection(servSock); //A CONNECTION IS MADE.
		if(clientSock == -1) continue;
		PACKET_TYPE packettype = getPacketType(clientSock);
		if(clientSock == -1) continue;

		switch(packettype)						 //Determine which protocol and algorithm to call
		{
			case SAMPLE_DATA:
				geoHearingProtocol(clientSock, argv[2]);
				break;
			case PHOTO_REQ:
				SendImage(clientSock);
				break;
			case FINAL:
			case NOPACKET:							 // Since nothing is written or read in these statements
				FILE *fp=fdopen(clientSock, "w+");	// they need to be flushed.
				fclose(fp);
				break;
		}
	}
	//NOT REACHED
}