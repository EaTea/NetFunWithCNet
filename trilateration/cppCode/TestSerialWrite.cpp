#include "locate.h"
#include "serialise.h"
using namespace std;

void testSerialWrite(const char* filename)
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
	vector<string> v;
	v.push_back("00-B0-D0-86-BB-F7");
	v.push_back("00:1b:63:84:45:e6");
	v.push_back("00-14-22-01-23-45");
	for(int s = 0; s < 3; s++)
	{
		std::vector<float> lat;
		std::vector<float> lon;
		std::vector<int> rad;
		for(int i = 0; i < 100; i++)
		{
			float latitude = lat1+((float)(rand()%100))/100.0*(lat2-lat1);
			float longitude = lon1+((float)(rand()%100))/100.0*(lon2-lon1);
			float radius = -1 * (rand()%65+40);
			lat.push_back(latitude);
			lon.push_back(longitude);
			rad.push_back(radius);
		}

		if(!generateWAPPosition(lat,lon,rad,v[s]))
		{
			PRINT_ERR("Did not generate enough points for %s\n",v[s].c_str());
		}
	}
	writeSamplesToFile(filename);
}
