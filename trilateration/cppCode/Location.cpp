#include "locate.h"

//the furthest possible distance in the x direction on the map (horizontally)
float X_MAX_LIMIT;
//the furthest possible distance in the y direction on the map (vertically)
float Y_MAX_LIMIT;
const float EARTH_RADIUS = 6372797; //in metres, approximation from Wikipedia

void locatePosition(const std::vector<Circle>& v, Point* prev, int *nPrev)
{
	float xSum, ySum;
	uint32_t nPoints;
	xSum = ySum = 0;
	nPoints = 0;

	for(size_t i = 0; i < v.size(); i++)
	{
		for(size_t j = i+1; j < v.size(); j++)
		{
			for(size_t k = j+1; k < v.size(); k++)
			{
				try
				{
					Point p = trilaterate(v[i],v[j],v[k]);
					xSum += p.getX();
					ySum += p.getY();
					nPoints++;
				}
				catch(int e)
				{
					if(e == CIRCLES_DO_NOT_INTERSECT)
						continue;
					else
					{
						PRINT_ERR("Trilateration encountered unexpected error, closing\n");
						exit(e);
					}
				}
			}
		}
	}

	//nPoints is 0 so we could not find any trilateration points
	//samples are unreliable
	if(nPoints == 0)
		throw NO_POINT_SAMPLES_GENERATED;
	
	//add previous estimated location by the number of samples
	xSum += prev->getX()*(*nPrev);
	ySum += prev->getY()*(*nPrev);

	xSum /= (*nPrev + nPoints);
	ySum /= (*nPrev + nPoints);

	prev->setX(xSum);
	prev->setY(ySum);

	*nPrev = nPoints+*nPrev;
}

float distanceBetweenTwoLatLons(float lat1, float lon1, float lat2, float lon2)
{
	//calculate pi manually
	double pi = atan(1)*4;

	//convert lats and lons to radii from equator and greenwich resp.
	float lat1_ra, lon1_ra, lat2_ra, lon2_ra;
	lat1_ra = lat1/180.0*pi;
	lon1_ra = lon1/180.0*pi;
	lat2_ra = lat2/180.0*pi;
	lon2_ra = lon2/180.0*pi;
	
	//haversine formula to calculate distance between two latlons
	float haverLat, haverLon, cosLat2, cosLat1;
	haverLat = pow(sin((lat2_ra-lat1_ra)/2),2);
	haverLon = pow(sin((lon2_ra-lon1_ra)/2),2);
	cosLat2 = cos(lat2_ra);
	cosLat1 = cos(lat1_ra);
	float sum = sqrt(haverLat + cosLat1*cosLat2*haverLon);
	float tmp = asin(sum);
	float distance = 2 * EARTH_RADIUS * tmp; //in metres

	return distance;
}

void boundingBoxInMetres(float lat1, float lon1, float lat2, float lon2)
{
	
}
