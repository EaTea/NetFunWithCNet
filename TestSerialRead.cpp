#include "trilaterate.h"
using namespace std;

void testSerialRead(const char* filename)
{
	if(filename == NULL)
	{
		PRINT_ERR("USAGE: pass name of file to use for serialisation test\n");
		exit(1);
	}
	float lat1, lat2, lon1, lon2;
	lat1 = -32.5542;
	lat2 = -32.542;
	lon1 = 100.5432;
	lon2 = 100.55;
	setupPermissibleArea(lat1,lon1,lat2,lon2);
	readSamplesFromFile(filename);
}
