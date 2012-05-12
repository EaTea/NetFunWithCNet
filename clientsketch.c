#define _POSIX_C_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>		// for memset()
#include <unistd.h>		// for close()
#include <sys/socket.h>	// for socket(), connect(), send(), and recv()
#include <arpa/inet.h>	 // for sockaddr_in and inet_addr()
//#include <ipod3230.h>
#include "ipod3230.h"

static const char *GEOSERVER_IP = "130.95.80.196";	//Geoserver's actual IP address
static const int GEOSERVER_PORT = 4;					//Will assign to port 4

static IPOD_SAMPLE *samples;
static uint8_t nSamples;

//	FUNCTION init_application() IS CALLED AS SOON AS THE iPHONE'S GUI
//	HAS BEEN DRAWN. IT PROVIDES AN OPPORTUNITY TO INITIALIZE YOUR APPLICATION.
//	init_application() SHOULD RETURN 0 IF SUCCESSFUL, OR ANY OTHER VALUE
//	IF THE APPLICATION CANNOT BE INITIALIZED.

int init_application(void)
{
	printf("Yes");
	DEBUG("%s()\n", __func__);
	return 0;
}

//	FUNCTION set_geoserver() IS CALLED TO SET YOUR geoserver's HOSTNAME
//	AND PORT NUMBER WHEN THE 'Save' BUTTON ON THE SETTINGS SCREEN IS PRESSED

void set_geoserver(const char *hostname, int16_t port)
{
	DEBUG("%s(\"%s\", %d)\n", __func__, hostname, port);
}

//	FUNCTION now_hearing() IS CALLED WHEN THE 'Sample WiFi' BUTTON
//	ON THE mapping SCREEN IS PRESSED.	THE CURRENT latitude AND longitude
//	(AS INDICATED BY THE MAP'S CROSSHAIRS) ARE PROVIDED, TOGETHER WITH A
//	VECTOR OF APINFO STRUCTURES 'OVERHEARD' AT THE CURRENT LOCATION.

void now_hearing(float lat, float lon, int nAPs, const APINFO *APs)
{
	DEBUG("%s(nAPs=%d)\n", __func__, nAPs);
	for(int n=0 ; n<nAPs ; ++n) {
		DEBUG("	%s\n", APs[n].ssid);
		DEBUG("	%s	- %d\n", APs[n].mac, APs[n].rssi);
	}
}

//	FUNCTION now_facing() IS CALLED WHEN ONE OF THE 4 DIRECTION BUTTONS ON
//	THE photos SCREEN IS PRESSED.	THE INDICATED DIRECTION AND THE VECTOR
//	OF APINFO STRUCTURES 'OVERHEARD' AT THE CURRENT LOCATION IS PROVIDED.

void now_facing(FACING dir, int nAPs, const APINFO *APs)
{
	DEBUG("%s(%c, nAPs=%d)\n", __func__, "NSEW"[(int)dir], nAPs);
}

//	FUNCTION finalize_application() IS CALLED JUST BEFORE THE APPLICATION
//	EXITS.	IT PROVIDES AN OPPORTUNITY TO SAVE ANY VOLATILE DATA REQUIRED
//	FOR FUTURE INVOCATIONS OF YOUR APPLICATION.

void finalize_application(void)
{

	DEBUG("%s()\n", __func__);

	int sock;							//Socket descriptor
	struct sockaddr_in geoServAddr;	//Geoserver server address
	const char *geoServIP = GEOSERVER_IP;		 //Geoserver IP address (dotted quad)
	in_port_t geoServPort = GEOSERVER_PORT;	 //Geoserver server port
	
	//TODO:CONSTRUCT MESSAGE TO SEND
	
	// 1. Create stream socket using TCP
	
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if (sock < 0) {
		DEBUG("socket() failed\n");
		exit(1);
	}
	
	// 2. Construct server address structure
	
	memset(&geoServAddr, 0, sizeof(geoServAddr)); //Zero out structure
	geoServAddr.sin_family = AF_INET;			 //IPv4 address family
	
	// 3. Convert address
	
	int rtnVal = inet_pton(AF_INET, geoServIP, &geoServAddr.sin_addr.s_addr);
	if(rtnVal == 0){
		DEBUG("inet_pton() failed due to invalid address string\n");
		exit(1);
	} else if(rtnVal<0){
		DEBUG("inet_pton() failed\n");
	}
	
	geoServAddr.sin_port = htons(geoServPort);
	
	// 4. Establish connection to server
	
	if(connect(sock, (struct sockaddr *)&geoServAddr, sizeof(geoServAddr))<0){ 
		DEBUG("connect() failed\n");
		exit(1);
	}
	
	// 5. Send message to server
	
	// 6. Receive message from server

#define __USE_POSIX 1
#undef __STRICT_ANSI__

	FILE* fsock = fdopen(sock, "w+");
	while(send(sock,(void*)&nSamples, sizeof(uint8_t), MSG_CONFIRM) < 0)
		DEBUG("Encountered error while sending number of samples, retrying\n");

	DEBUG("Successfully sent sample size\n");

	if(fwrite(samples, nSamples*sizeof(IPOD_SAMPLE), sizeof(IPOD_SAMPLE), fsock) != nSamples)
	{
		DEBUG("Encountered error while sending samples\n");
		exit(1);
	}
	//N.B.: sliding window possible but not necessary atm
	/*
	for(int i = 0; i < nSamples; i++)
	{
		send(sock, (void*)samples+i, sizeof());
		while(send(sock, (void*)&(samples[i].lat), sizeof(lat), MSG_CONFIRM) < 0)
			DEBUG("Encountered error while sending sample %d's latitude, retrying\n", i);
		DEBUG("Successfully sent sample %d's latitude\n", i);

		while(send(sock, (void*)&(samples[i].lon), sizeof(lon), MSG_CONFIRM) < 0)
			DEBUG("Encountered error while sending sample %d's longitude, retrying\n", i);
		DEBUG("Successfully sent sample %d's longitude\n", i);

		while(send(sock, (void*)&(samples[i].nAPs), sizeof(samples[i].nAPs), MSG_CONFIRM) < 0)
			DEBUG("Encountered error while sending sample %d's nAPs, retrying\n", i);
		DEBUG("Successfully sent sample %d's nAPs\n", i);

		DEBUG("Successfully sent sample %d\n", i);
		for(uint8_t j = 0; j < samples[i].nAPs; j++)
		{
			APINFO ap = samples[i].APs[j];
			ap.rssi = htonl(ap.rssi);

			while(send(sock, (void*)ap.ssid, strlen(ap.ssid)+1, MSG_CONFIRM) < 0)
				DEBUG("Encountered error while sending APINFO %d of sample %d's ssid, retrying\n", j, i);
			DEBUG("Successfully sent APINFO %d of sample %d's ssid\n", j, i);

			while(send(sock, (void*) ap.mac, MAC_LEN, MSG_CONFIRM) < 0)
				DEBUG("Encountered error while sending APINFO %d of sample %d's MAC address, retrying\n", j, i);
			DEBUG("Successfully sent APINFO %d of sample %d's MAC address\n", j, i);

			while(send(sock, (void*) &ap.rssi, sizeof(ap.rssi), MSG_CONFIRM) < 0)
				DEBUG("Encountered error while sending APINFO %d of sample %d's rssi, retrying\n", j, i);
			DEBUG("Successfully sent APINFO %d of sample %d's rssi\n", j, i);

			DEBUG("Successfully sent APINFO %d of sample %d\n", j, i);
		}
	}*/
	//for each sample
		//while server has not acked
			//determine struct size
			//send struct sample
			//wait for recv
}

/*
 *	vim: ts=4 sw=4
*/

int main(int argc, char *argv[]){

}
