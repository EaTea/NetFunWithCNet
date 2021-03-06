//	ipod3230.h,	written by Chris McDonald, chris@csse.uwa.edu.au

#include <stdint.h>
#include <stdarg.h>

//	THE FOLLOWING ENUMERATED TYPE INDICATES THE DIRECTION (AS COMPASS POINTS)
//	FACED WHEN REQUESTING A PHOTO FROM YOUR geoserver

typedef enum {
	FACING_N, FACING_S, FACING_E, FACING_W
} FACING;

typedef enum {
	PHOTO_REQ, FINAL, SAMPLE_DATA
} PACKET_TYPE;

//	AN SSID (SERVICE SET IDENTIFIER) IS THE NAME OF A WIRELESS LAN.
//	ALL WIRELESS DEVICES ON A WLAN MUST EMPLOY THE SAME SSID IN ORDER
//	TO COMMUNICATE WITH EACH OTHER.	SSIDs ARE CASE SENSITIVE TEXT
//	STRINGS, CONSISTING OF ALPHANUMERIC CHARACTERS.

#define	SSID_LEN	32
#define MAC_LEN 18

//	THE APINFO STRUCTURE DESCRIBES A CAPTURED BEACON FRAME, WHICH WAS
//	TRANSMITTED BY A DEVICE OF THE INDICATED SSID AND MAC-ADDRESS (BOTH
//	NUL-BYTE TERMINATED STRINGS).
//	
//	THE RELATIVE SIGNAL STRENGTH INDICATOR (RSSI) IS AN INTEGRAL MEASURE
//	OF THE POWER OF THE ARRIVING BEACON FRAME.
//	ITS VALUES RANGE FROM 0 (powerful) DOWN TO -95 (very weak).

typedef struct {
	char	ssid[SSID_LEN+1];	// e.g.		SNAP\0
	char	mac[MAC_LEN];		// e.g.		00:12:da:94:1a:b0\0
	int32_t	rssi;			// e.g.		-65
} APINFO;

/*
	The IPOD_SAMPLE structure is a programmatic representation of the samples
	taken using the app. The latitude and longitude, the number of APs and the
	information about each AP heard at a location are represented within this
	struct.
*/

typedef struct {
	float lat;
	float lon;
	int nAPs;
	APINFO * APs;
} IPOD_SAMPLE;

//	FUNCTION init_application() IS CALLED AS SOON AS THE iPHONE'S GUI
//	HAS BEEN DRAWN. IT PROVIDES AN OPPORTUNITY TO INITIALIZE YOUR APPLICATION.
//	init_application() SHOULD RETURN 0 IF SUCCESSFUL, OR ANY OTHER VALUE
//	IF THE APPLICATION CANNOT BE INITIALIZED.

extern	int init_application(void);

//	FUNCTION set_geoserver() IS CALLED TO SET YOUR geoserver's HOSTNAME
//	AND PORT NUMBER WHEN THE 'Save' BUTTON ON THE settings SCREEN IS PRESSED

extern	void set_geoserver(const char *hostname, int16_t port);

//	FUNCTION now_hearing() IS CALLED WHEN THE 'Now hearing?' BUTTON
//	ON THE mapping SCREEN IS PRESSED.	THE CURRENT latitude AND longitude
//	(AS INDICATED BY THE MAP'S CROSSHAIRS) ARE PROVIDED, TOGETHER WITH A
//	VECTOR OF APINFO STRUCTURES 'OVERHEARD' AT THE CURRENT LOCATION.

extern	void now_hearing(float lat, float lon, int nAPs, const APINFO *APs);

//	FUNCTION now_facing() IS CALLED WHEN ONE OF THE 4 DIRECTION BUTTONS ON
//	THE photos SCREEN IS PRESSED.	THE INDICATED DIRECTION AND THE VECTOR
//	OF APINFO STRUCTURES 'OVERHEARD' AT THE CURRENT LOCATION IS PROVIDED.

extern	void now_facing(FACING dir, int nAPs, const APINFO *APs);

//	THE PROVIDED FUNCTION set_current_photo() IS CALLED TO SET/CHANGE
//	THE PHOTO DISPLAYED ON THE photos SCREEN.	THE SINGLE ARGUMENT
//	PROVIDES THE FILENAME OF A SMALL PNG IMAGE.	THE FUNCTION RETURNS 0
//	IF THE INDICATED IMAGE CAN BE ACCESSED AND DISPLAYED, 1 OTHERWISE.

extern	int	set_current_photo(const char *png_filename);

//	THE PROVIDED FUNCTION set_popup() IS CALLED TO DISPLAY A SHORT
//	MESSAGE IN A POPUP DIALOGUE BOX ON THE SCREEN.

extern	void set_popup(const char *message);

//	FUNCTION finalize_application() IS CALLED JUST BEFORE THE APPLICATION
//	EXITS.	IT PROVIDES AN OPPORTUNITY TO SAVE ANY VOLATILE DATA REQUIRED
//	FOR FUTURE INVOCATIONS OF YOUR APPLICATION.

extern	void finalize_application(void);

//	FUNCTION DEBUG() IS A printf()-like FUNCTION TO DISPLAY DEBUG TEXT ON
//	THE debug SCREEN,	e.g.	DEBUG("sending request number %d\n", seqno);

extern void DEBUG(const char *fmt, ...);
