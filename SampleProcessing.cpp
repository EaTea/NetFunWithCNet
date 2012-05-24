#include "trilaterate.h"
#include "locate.h"

//structure for storing sample infos
//note: much of the geoserver was coded indep
struct sample_info
{
	float latitude;
	float longitude;
	int32_t rssi;
};

std::map<std::string, std::vector<sample_info> > samplesSoFar;

void processSamples(int nSamples, IPOD_SAMPLE *samples)
{
#ifdef VERBOSE
	PRINT_ERR("Processing %d samples\n", nSamples);
#endif
	//for each of the samples
	for(int sam = 0; sam < nSamples; sam++)
	{
		int nAPs = samples[sam].nAPs;
		float lat, lon;
		//lat long sample was taken at
		lat = samples[sam].lat;
		lon = samples[sam].lon;
		//convenience pointer to beginning of APs
		APINFO *apIter = samples[sam].APs;

		//for each ap
		for(int ap = 0; ap < nAPs; ap++)
		{
			//c++ string for MAC because they're awesome
			std::string macAddress(apIter[ap].mac);
			sample_info s;
			s.latitude = lat;
			s.longitude = lon;
			s.rssi = apIter[ap].rssi;
			samplesSoFar[macAddress].push_back(s);
		}
	}
	//iterate through the map
	//construct 
	std::map<std::string,std::vector<sample_info> >::iterator it = samplesSoFar.begin();
	for(; it != samplesSoFar.end(); it++)
	{
		//number of processed samples
		int nProc = it->second.size();
		//set a threshold that 10+ samples must be present before trilateration can occur
		if(nProc >= 10)
		{
			std::vector<float> lats;
			std::vector<float> lons;
			std::vector<int32_t> rssis;
			
			std::vector<sample_info>::iterator sInfoIter = it->second.begin();
			for(; sInfoIter != it->second.end(); sInfoIter++)
			{
				lats.push_back(sInfoIter->latitude);
				lons.push_back(sInfoIter->longitude);
				rssis.push_back(sInfoIter->rssi);
			}
			//generate a position for this WAP
			generateWAPPosition(lats,lons,rssis,it->first);
			it->second.clear();
		}
	}
}

int processLocation(int nAPs, APINFO *APs, float *lat, float*lon)
{
#ifdef VERBOSE
	PRINT_ERR("Attempting to find location of client\n");
#endif
	std::vector<std::string> macs;
	std::vector<int> rssis;
	for(int i = 0; i < nAPs; i++)
	{
		macs.push_back(std::string(APs[i].mac));
		rssis.push_back(APs[i].rssi);
	}
	return findCurrentPosition(macs,rssis,lat,lon) ? 0 : 1;
}
