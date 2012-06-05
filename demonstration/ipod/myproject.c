/*
 * Version 05/06 16:50
 */


#define _POSIX_C_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>	/* for sockaddr_in and inet_addr() */

#include "ipod3230.h"

//Global Variables
char *servIP;
in_port_t servPort;
int sock;
struct sockaddr_in servAddr;
bool isGeoSet = false;

//Function for setting up the client socket for the given server.
void SetupTCPClientSocket(unsigned short port) {

	sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);		//create socket descriptor
	if(sock<0)
		DEBUG("%s(%d): socket() failed", __func__, port);

	memset(&servAddr, 0, sizeof(servAddr)); //Zero out stucture
	servAddr.sin_family = AF_INET;		//IPv4 address family

	//Convert address
	int rtnVal = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);
	if(rtnVal == 0)
		DEBUG("%s(%d): inet_pton() failed invalid address string", __func__, port);
	else if(rtnVal < 0)
		DEBUG("%s(%d): inet_pton() failed", __func__, port);
	servAddr.sin_port = htons(port);

	if(connect(sock, (struct sockaddr *)&servAddr, sizeof(servAddr))<0) //connect
		DEBUG("%s(%d): connect() failed", __func__, port);
}

//Function to tell geoserver what application protocol to implement
void sendPacketType(PACKET_TYPE type) {
    SetupTCPClientSocket(servPort); //need to create socket after fclose
	FILE *outstream = fdopen(sock, "w");

	PACKET_TYPE NETWORKtype = (PACKET_TYPE) htonl(type);
	if(fwrite( &NETWORKtype,sizeof(NETWORKtype),1, outstream) != 1){
		DEBUG("fwrite()packet type failed\n");
	}

	fclose(outstream);
}

//byte order transformation from beej
uint32_t htonf(float f)
{
	uint32_t p;
	uint32_t sign;

	if (f < 0) { sign = 1; f = -f; }
	else { sign = 0; }

	p = ((((uint32_t)f)&0x7fff)<<16) | (sign<<31); // whole part and sign
	p |= (uint32_t)(((f - (int)f) * 65536.0f))&0xffff; // fraction

	return p;
}

//	FUNCTION init_application() IS CALLED AS SOON AS THE iPHONE'S GUI
//	HAS BEEN DRAWN. IT PROVIDES AN OPPORTUNITY TO INITIALIZE YOUR APPLICATION.
//	init_application() SHOULD RETURN 0 IF SUCCESSFUL, OR ANY OTHER VALUE
//	IF THE APPLICATION CANNOT BE INITIALIZED.

int init_application(void)
{
	DEBUG("%s()\n", __func__);
	return 0;
}

//	FUNCTION set_geoserver() IS CALLED TO SET YOUR geoserver's HOSTNAME
//	AND PORT NUMBER WHEN THE 'Save' BUTTON ON THE SETTINGS SCREEN IS PRESSED

void set_geoserver(const char *hostname, int16_t port)
{
	size_t iplen = strlen(hostname);
	servIP = (char *)malloc(iplen+1);
	strcpy(servIP, hostname);
	servPort = port;
	isGeoSet= true;
	DEBUG("%s(\"%s\", %d)\n", __func__, servIP, servPort);
}

//	FUNCTION now_hearing() IS CALLED WHEN THE 'Sample WiFi' BUTTON
//	ON THE mapping SCREEN IS PRESSED.	THE CURRENT latitude AND longitude
//	(AS INDICATED BY THE MAP'S CROSSHAIRS) ARE PROVIDED, TOGETHER WITH A
//	VECTOR OF APINFO STRUCTURES 'OVERHEARD' AT THE CURRENT LOCATION.
//  This will also send the information to the geoserver so that it can process the information
//  and	calculate the triangulation.

void now_hearing(float lat, float lon, int nAPs, const APINFO *APs)
{
	if(!isGeoSet){
		set_popup("Your Geoserver is not set");
		return;
	}
	if(nAPs==0){
		set_popup("No Access Points have been heard");
		return;
	}

	sendPacketType(SAMPLE_DATA); //Sent to geoserver to specify the needed protocol
    SetupTCPClientSocket(servPort);

    // When now hearing is pressed a IPOD_SAMPLE struct is made and then is sent to the geoserver.
	IPOD_SAMPLE sample; //structure specified in ipod3230.h

	sample.lat = lat;
	sample.lon = lon;
	sample.nAPs = nAPs;
	sample.APs = (APINFO *)malloc(nAPs*sizeof(APINFO));
	for(int i=0; i<nAPs; i++){
		sample.APs[i]=APs[i];
	}

	DEBUG("SetupTCPClientSocket passed\n");
	FILE *outstream = fdopen(sock, "w"); //wrap socket descriptor for writing
	DEBUG("Opened outstream\n");

	int NETWORKnAPs=htonl(sample.nAPs); // sends the number of access points as an int in one go.
	// This also requires a change in byte order
	if(fwrite( &NETWORKnAPs,sizeof(NETWORKnAPs),1, outstream) != 1){
		DEBUG("fwrite()nAPs failed\n");
	}
	uint32_t networkLat, networkLon;
	networkLat = htonf(sample.lat);
	networkLon = htonf(sample.lon);
	fwrite(&networkLat, sizeof(networkLat),1,outstream);
	fwrite(&networkLon, sizeof(networkLon),1,outstream);

	DEBUG("nAPs Written: %d\n", sample.nAPs);

	for(int i=0; i<sample.nAPs; i++){
		int num = fwrite( &sample.APs[i].ssid,sizeof(char),32, outstream);
		DEBUG("SSID Written: %s\n", sample.APs[i].ssid);
		fwrite( &sample.APs[i].mac,sizeof(char),18, outstream);
		DEBUG("MAC Written: %s\n", sample.APs[i].mac);
		fwrite( &sample.APs[i].rssi , sizeof(sample.APs[i].rssi) ,1, outstream);
		DEBUG("RSSI Written: %d\n", sample.APs[i].rssi);
	}
	fflush(outstream);
	fclose(outstream);

	free(sample.APs);
}

//	FUNCTION now_facing() IS CALLED WHEN ONE OF THE 4 DIRECTION BUTTONS ON
//	THE photos SCREEN IS PRESSED.	THE INDICATED DIRECTION AND THE VECTOR
//	OF APINFO STRUCTURES 'OVERHEARD' AT THE CURRENT LOCATION IS PROVIDED.
//  This will give a direction the geoserver who will then request an image from the
//  photoserver for the given direction. If no photo is available a size of 0
//  for the imageSize is passed

void now_facing(FACING dir, int nAPs, const APINFO *APs)
{
	DEBUG("%s(%c, nAPs=%d)\n", __func__, "NSEW"[(int)dir], nAPs);

	if(!isGeoSet){ //Check if geoserver is set.
		set_popup("Your Geoserver is not set");
		return;
	}

	sendPacketType(PHOTO_REQ);
    DEBUG("Passed sendPacketType\n");
    SetupTCPClientSocket(servPort); //Create socket.
    DEBUG("Passed SetupTCPClient Socket\n");

	char *filename = "/private/var/tmp/sampleimage.png"; //Where the image will be stored
														 //in the ipod

	FILE *instream = fdopen(sock, "w"); //Create filestream
	if(instream == NULL)
		DEBUG("fdopen() failed\n");

	//1. fwrite nAPs
	int NETWORKnAPs = htonl(nAPs);
	if(fwrite( &NETWORKnAPs,sizeof(NETWORKnAPs),1, instream) != 1){
		printf("fwrite() NETWORKnAPs failed\n");
	}

    DEBUG("Passed  1\n");
	//2. fwrite *APs
	for(int i=0; i<nAPs; i++){
		fwrite( &APs[i].ssid,sizeof(char),32, instream);
		fwrite( &APs[i].mac,sizeof(char),18, instream);
		fwrite( &APs[i].rssi ,sizeof(APs[i].rssi) ,1, instream);
	}
	DEBUG("Passed  2\n");
	//3. fwrite dir
	if(fwrite( &dir,sizeof(char),sizeof(dir), instream) != sizeof(dir)){
		printf("fwrite() FACING dir failed\n");
	}

    fflush(instream);

    DEBUG("Passed  3\n");

    fclose(instream);
    SetupTCPClientSocket(servPort);
    FILE *instream2 = fdopen(sock, "rb");

	//fseek(instream, 0L, SEEK_SET);
	int imageNBytes; //Size of image .
    int r = fread(&imageNBytes, sizeof(int), 1, instream2);
	if(r!=1){
        DEBUG("fread imageNBytes failed: %d\n", r);
        return;
    }

    DEBUG("Image Size: %d bytes\n", imageNBytes);
    imageNBytes = ntohl(imageNBytes);
	DEBUG("Image Size NTOHL: %d bytes\n", imageNBytes);

	if(imageNBytes== 0){
		set_popup("Sorry. No image found.");
        fclose(instream2);
		return;
	}

	FILE *photo =	fopen(filename, "wb+"); //File pointer to where the image is to be stored.
	if(photo ==NULL) {
		DEBUG("fopen() failed\n");
	}

	char buffer[imageNBytes]; //buffer for the image bytes
	int num = fread(buffer, sizeof(char), imageNBytes, instream);

	if( num != imageNBytes){
		DEBUG("read an unexpected number of bytes from instream: %d\n", num);
	}	else {
		DEBUG("read %d bytes from instream\n", num);
	}

	num = fwrite(buffer, sizeof(char), imageNBytes, photo);
	DEBUG("wrote %d bytes to photo\n", num);

	fclose(photo);
	fclose(instream2);


	set_current_photo(filename); //set the photo that is has been sent as the current photo.
}

//	FUNCTION finalize_application() IS CALLED JUST BEFORE THE APPLICATION
//	EXITS.	IT PROVIDES AN OPPORTUNITY TO SAVE ANY VOLATILE DATA REQUIRED
//	FOR FUTURE INVOCATIONS OF YOUR APPLICATION.

void finalize_application(void)
{
	//Ensure that file streams are flushed when the application is closed.
	sendPacketType(FINAL);
}

/*
 *	vim: ts=8 sw=4
 */
