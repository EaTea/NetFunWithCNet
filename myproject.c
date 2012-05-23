#define _POSIX_C_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */

#include "ipod3230.h"

char *servIP;
in_port_t servPort;
int sock;
struct sockaddr_in servAddr;


void SetupTCPClientSocket(unsigned short port) {
    
    sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sock<0)
        DEBUG("%s(%d): socket() failed", __func__, port);
    
    memset(&servAddr, 0, sizeof(servAddr)); //Zero out stucture
	servAddr.sin_family = AF_INET;          //IPv4 address family
    
    //Convert address
    int rtnVal = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);
    if(rtnVal == 0)
        DEBUG("%s(%d): inet_pton() failed invalid address string", __func__, port);
    else if(rtnVal < 0)
        DEBUG("%s(%d): inet_pton() failed", __func__, port);
    servAddr.sin_port = htons(port);
    
    if(connect(sock, (struct sockaddr *)&servAddr, sizeof(servAddr))<0)
        DEBUG("%s(%d): connect() failed", __func__, port);
    
}


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




//  FUNCTION init_application() IS CALLED AS SOON AS THE iPHONE'S GUI
//  HAS BEEN DRAWN. IT PROVIDES AN OPPORTUNITY TO INITIALIZE YOUR APPLICATION.
//  init_application() SHOULD RETURN 0 IF SUCCESSFUL, OR ANY OTHER VALUE
//  IF THE APPLICATION CANNOT BE INITIALIZED.

int init_application(void)
{
    DEBUG("%s()\n", __func__);
    return 0;
}

//  FUNCTION set_geoserver() IS CALLED TO SET YOUR geoserver's HOSTNAME
//  AND PORT NUMBER WHEN THE 'Save' BUTTON ON THE SETTINGS SCREEN IS PRESSED

void set_geoserver(const char *hostname, int16_t port)
{
	size_t iplen = strlen(hostname);
	servIP = (char *)malloc(iplen+1);
	strcpy(servIP, hostname);
	servPort = port;
    DEBUG("%s(\"%s\", %d)\n", __func__, servIP, servPort);
}

//  FUNCTION now_hearing() IS CALLED WHEN THE 'Sample WiFi' BUTTON
//  ON THE mapping SCREEN IS PRESSED.  THE CURRENT latitude AND longitude
//  (AS INDICATED BY THE MAP'S CROSSHAIRS) ARE PROVIDED, TOGETHER WITH A
//  VECTOR OF APINFO STRUCTURES 'OVERHEARD' AT THE CURRENT LOCATION.

void now_hearing(float lat, float lon, int nAPs, const APINFO *APs)
{
        
    
    /* When now hearing is pressed a IPOD_SAMPLE struct is made and then is sent to the geoserver.
     
     */
    IPOD_SAMPLE sample;
        
    sample.lat = lat;
    sample.lon = lon;
    sample.nAPs = nAPs;
    sample.APs = (APINFO *)malloc(nAPs*sizeof(APINFO));
    for(int i=0; i<nAPs; i++){
        sample.APs[i]=APs[i];
    }
    
    SetupTCPClientSocket(servPort);
    DEBUG("SetupTCPClientSocket passed\n");
    FILE *outstream = fdopen(sock, "w");
    DEBUG("Opened outstream\n");
    
    int NETWORKnAPs=htonl(sample.nAPs);
    if(fwrite( &NETWORKnAPs,sizeof(NETWORKnAPs),1, outstream) != 1){
        DEBUG("fwrite()nAPs failed\n");
    }
    
    sample.lat =htonf(sample.lat);
    sample.lon =htonf(sample.lon);
    fwrite(&sample.lat, sizeof(sample.lat),1,outstream);
    fwrite(&sample.lon, sizeof(sample.lon),1,outstream);
    
    fflush(outstream);
    
    DEBUG("nAPs Written: %d\n", sample.nAPs);
    
    
    for(int i=0; i<sample.nAPs; i++){
        int num = fwrite( &sample.APs[i].ssid,sizeof(char),32, outstream);
        DEBUG("SSID Written: %s\n", sample.APs[i].ssid);
        fwrite( &sample.APs[i].mac,sizeof(char),18, outstream);
        DEBUG("MAC Written: %s\n", sample.APs[i].mac);
        fwrite( &sample.APs[i].rssi , sizeof(sample.APs[i].rssi) ,1, outstream);
        DEBUG("RSSI Written: %d\n", sample.APs[i].rssi);
    } 
    
    
    fclose(outstream);
    
    
}

//  FUNCTION now_facing() IS CALLED WHEN ONE OF THE 4 DIRECTION BUTTONS ON
//  THE photos SCREEN IS PRESSED.  THE INDICATED DIRECTION AND THE VECTOR
//  OF APINFO STRUCTURES 'OVERHEARD' AT THE CURRENT LOCATION IS PROVIDED.

void now_facing(FACING dir, int nAPs, const APINFO *APs)
{
    DEBUG("%s(%c, nAPs=%d)\n", __func__, "NSEW"[(int)dir], nAPs);
    
    
    char *filename = "/private/var/tmp/sampleimage.png";
    SetupTCPClientSocket(servPort);
    
    FILE *instream = fdopen(sock, "r");
    if(instream == NULL)
        DEBUG("fdopen() failed\n");
    
    int imageNBytes;
    
    fread(&imageNBytes, sizeof(int), 1, instream);
    imageNBytes = ntohl(imageNBytes);
    DEBUG("Image Size: %d bytes\n", imageNBytes);
    
    FILE *photo =  fopen(filename, "wb+");
    if(photo ==NULL) {
    	DEBUG("fopen() failed\n");
    }
    
    char buffer[imageNBytes];
    int num = fread(buffer, sizeof(char), imageNBytes, instream);
    
    if( num != imageNBytes){
        DEBUG("read an unexpectd number of bytes from instream: %d\n", num);
    }  else {
        DEBUG("read %d bytes from instream\n", num);
    }   
    
    
    num = fwrite(buffer, sizeof(char), imageNBytes, photo);
    DEBUG("wrote %d bytes to photo\n", num);
    
    fclose(photo);
    fclose(instream);
    
    set_current_photo(filename);
}

//  FUNCTION finalize_application() IS CALLED JUST BEFORE THE APPLICATION
//  EXITS.  IT PROVIDES AN OPPORTUNITY TO SAVE ANY VOLATILE DATA REQUIRED
//  FOR FUTURE INVOCATIONS OF YOUR APPLICATION.

void finalize_application(void)
{


    

    
    
}

/*
 *  vim: ts=8 sw=4
 */


    

    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    


