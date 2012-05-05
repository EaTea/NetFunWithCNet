#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ipod3230.h>

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
    DEBUG("%s(\"%s\", %d)\n", __func__, hostname, port);
}

//  FUNCTION now_hearing() IS CALLED WHEN THE 'Sample WiFi' BUTTON
//  ON THE mapping SCREEN IS PRESSED.  THE CURRENT latitude AND longitude
//  (AS INDICATED BY THE MAP'S CROSSHAIRS) ARE PROVIDED, TOGETHER WITH A
//  VECTOR OF APINFO STRUCTURES 'OVERHEARD' AT THE CURRENT LOCATION.

void now_hearing(float lat, float lon, int nAPs, const APINFO *APs)
{
    DEBUG("%s(nAPs=%d)\n", __func__, nAPs);
    for(int n=0 ; n<nAPs ; ++n) {
	DEBUG("  %s\n", APs[n].ssid);
	DEBUG("    %s  - %d\n", APs[n].mac, APs[n].rssi);
    }
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


int main(int argc, char *argv[]){
    
    // 1. Create stream socket using TCP
    
    // 2. Construct server address structure
    
    // 3. Convert address
    
    // 4. Establish connection to server
    
    // 5. Send message to server
    
    // 6. Receive message from server
    
}