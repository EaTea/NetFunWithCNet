#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */

#include <ipod3230.h>

char *servIP;
in_port_t servPort;
int sock;
struct sockaddr_in servAddr;


int SetupTCPClientSocket(unsigned short port) {
    
    int sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
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
    char *helloString = "Hello World\r\n";
    
    DEBUG("%s(nAPs=%d)\n", __func__, nAPs);
    for(int n=0 ; n<nAPs ; ++n) {
	DEBUG("  %s\n", APs[n].ssid);
	DEBUG("    %s  - %d\n", APs[n].mac, APs[n].rssi);
    }
    
    SetupTCPClientSocket(servPort);
    
    size_t helloStringLen = strlen(helloString);
    ssize_t numBytes = send(sock, helloString, helloStringLen, 0);
    if(numBytes <0)
        DEBUG("%s(): send() failed", __func__);
    else if(numBytes != helloStringLen)
        DEBUG("%s(): send() sent unexpected number of bytes", __func__);
    
    
}

//  FUNCTION now_facing() IS CALLED WHEN ONE OF THE 4 DIRECTION BUTTONS ON
//  THE photos SCREEN IS PRESSED.  THE INDICATED DIRECTION AND THE VECTOR
//  OF APINFO STRUCTURES 'OVERHEARD' AT THE CURRENT LOCATION IS PROVIDED.

void now_facing(FACING dir, int nAPs, const APINFO *APs)
{
    DEBUG("%s(%c, nAPs=%d)\n", __func__, "NSEW"[(int)dir], nAPs);
}

//  FUNCTION finalize_application() IS CALLED JUST BEFORE THE APPLICATION
//  EXITS.  IT PROVIDES AN OPPORTUNITY TO SAVE ANY VOLATILE DATA REQUIRED
//  FOR FUTURE INVOCATIONS OF YOUR APPLICATION.

void finalize_application(void)
{
    DEBUG("%s()\n", __func__);
}

/*
 *  vim: ts=8 sw=4
 */

