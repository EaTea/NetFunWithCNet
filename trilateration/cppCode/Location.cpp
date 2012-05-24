#include "trilaterate.h"

#include <utility>

/*
	Given a vector of n Circles, perform nC3 unique trilateration calculations
	Find the centroid of the trilateration points plus the previously calculated
	centroid multiplied by its previous weighting to find the new centroid and the
	new number of samples required to generate it
	Returns through pass by reference
*/
void locatePosition				(
														const std::vector<Circle>&, //circle samples of a specific location
														Point *, //the previously calculated location
														int * //the number of samples involved in calculation of previous location
													);


/*
	Find the floating point representation of distance between two lat lons
	Uses Haversine formula
*/
float distanceBetweenTwoLatLons		(
																		float, //latitude 1
																		float, //longitude 1
																		float, //latitude 2
																		float //longitude 2
																	);

/*
	Converts latitude and longitude to a Point in XY where lower LH corner of the
	bounding box of the area the photoserver covers is considered origin
	Returns Point
*/
Point latLonToPoint								(
																		float, //latitude
																		float //longitude
																	);

/*
	Converts Point to latitude and longitude in where lower LH corner of bounding box of
	area photoserver covers is considered origin
	returns through pass by reference
*/
void pointToLatLon								(	
																		const Point&, //point to convert
																		float*, //latitude
																		float* //longitude
																	);

/*
	Haversine function, as defined by wikipedia to be 
	Haversine(x) = (sin(x/2))^2
*/
float haversine(float);

//the furthest possible distance in the x direction on the map (horizontally)
float X_MAX;
//the furthest possible distance in the y direction on the map (vertically)
float Y_MAX;
//the latitude to treat as the "origin" of our rectangle, is bottom LH corner
float lat_origin;
//the longitude to treat as the "origin" of our rectangle, is bottom LH corner
float lon_origin;
//the latitude difference of the rectangle, that is, the furthest two points' difference in latitude
float lat_diff;
//the longitude difference of the rectangle area, that is, the furthest two points' difference in longitude
float lon_diff;
const float EARTH_RADIUS = 6372797; //in metres, approximation from Wikipedia

//keeps all previously calculated WAP locations along with number of samples used to determine
std::map<std::string, std::pair<Point, int> > macSamples;

float haversine(float f)
{
	return pow(sin(f/2),2);
}

bool findCurrentPosition(const std::vector<std::string>& MACs, const std::vector<int32_t>& rssis, float *lat, float *lon)
{
	//generate circles
	std::vector<Circle> circles;
#ifdef VERBOSE
	PRINT_ERR("Finding current position\n");
	PRINT_ERR("Processing %d MACs\n", MACs.size());
#endif
	for(unsigned int i = 0; i < MACs.size(); i++)
	{
		//no established values for this MAC, skip and continue
		if(macSamples.find(MACs[i]) == macSamples.end()) continue;
		//get the point the WAP specified by this MAC supposedly resides at
		Point p = macSamples[MACs[i]].first;
		//note: number of previous samples is irrelevant
		float radius = fabs(rssis[i]); //TODO: bad hack to convert rssi to distance in m
		Circle c(p, radius);
		circles.push_back(c);
	}

	if(circles.size() < 3)
	{
#ifdef VERBOSE
	PRINT_ERR("Not enough known WAPs at this position\n");
	PRINT_ERR("Needed at least 3 WAP samples but only found %d\n", circles.size());
#endif
		return false;
	}
	Point location;
	try
	{
		//find a new position and write the calculated position to location
		locatePosition(circles,&location, 0);
	}
	catch(int e)
	{
		//couldn't find position, so no samples were generated
		if(e == NO_POINT_SAMPLES_GENERATED)
		{
#ifdef VERBOSE
	PRINT_ERR("Not enough potential positions found during trilateration\n");
#endif
			return false;
		}
		else
			exit(1);
	}
	//converts point to latitude and longitude through pass by reference
	pointToLatLon(location, lat, lon);
	return true;
}

bool generateWAPPosition(const std::vector<float>& lats, const std::vector<float>& lons, const std::vector<int32_t>& rssis, const std::string& macAddress)
{
#ifdef VERBOSE
	PRINT_ERR("Approximating position of WAP %s\n", macAddress.c_str());
	PRINT_ERR("Found %d samples associated with %s\n", lats.size(),macAddress.c_str());
#endif
	//constructing all circles from lat, lons, rssis
	std::vector<Circle> circles;
	size_t nCircles = std::min(lats.size(), std::min(lons.size(), rssis.size()));
	for(size_t i = 0; i < nCircles; i++)
	{
		Point p = latLonToPoint(lats[i],lons[i]);
		float rad = fabs(rssis[i]); //TODO: temporary conversion technique
		Circle c(p, rad);
		circles.push_back(c);
	}
	//not enough samples
	if(circles.size() < 3)
	{
#ifdef VERBOSE
	PRINT_ERR("Not enough potential samples for this WAP\n");
	PRINT_ERR("Needed at least 3 WAP samples but only found %d\n", circles.size());
#endif
		return false;
	}
	else
	{
		try
		{
			Point p; //at origin
			int nPrevious = 0;
			//previous values for this macAddress found
			if(macSamples.find(macAddress) != macSamples.end())
			{
				p = macSamples[macAddress].first;
				nPrevious = macSamples[macAddress].second;
			}
			//calculate the new position of this point
			locatePosition(circles, &p, &nPrevious);
			//insert new values into map, old ones are automatically handled
			macSamples[macAddress] = std::pair<Point, int>(p,nPrevious);
		}
		catch(int e)
		{ 
			//no point samples generated for this WAP
			if(e == NO_POINT_SAMPLES_GENERATED)
			{
#ifdef VERBOSE
	PRINT_ERR("Not enough potential positions found during trilateration\n");
#endif
				return false;
			}
			//some other error, exit with error
			else
			{
				PRINT_ERR("Encountered unexpected error %d, exiting\n",e);
				exit(e);
			}
		}
	}
	return true;
}

void locatePosition(const std::vector<Circle>& v, Point* prev, int *nPrev)
{
	double xSum, ySum;
	uint32_t nPoints;
	xSum = ySum = 0;
	nPoints = 0;

#ifdef VERBOSE
	PRINT_ERR("Performing all trilateration calculations\n");
#endif
	//perform nChoose3 trilateration calculations
	for(size_t i = 0; i < v.size(); i++)
	{
		for(size_t j = i+1; j < v.size(); j++)
		{
			for(size_t k = j+1; k < v.size(); k++)
			{
				try
				{
					Point p = trilaterate(v[i],v[j],v[k]);
					//add points to sum
					xSum += (double)p.getX();
					ySum += (double)p.getY();
					//increase number of points
					nPoints++;
				}
				catch(int e)
				{
					if(e == CIRCLES_DO_NOT_INTERSECT || CIRCLE_CENTRES_ARE_COLINEAR)
						continue;
					else
					{
						PRINT_ERR("Trilateration encountered unexpected error %d, closing\n",e);
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
	
	//add previous estimated location times number of samples
	xSum += prev->getX()*(*nPrev);
	ySum += prev->getY()*(*nPrev);

	//divide by total number of samples used in calculation
	xSum /= (*nPrev + nPoints);
	ySum /= (*nPrev + nPoints);

	//set point co-ordinates
	prev->setX((float)xSum);
	prev->setY((float)ySum);

	//set new number of samples used
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
	haverLat = haversine(lat2_ra-lat1_ra);
	haverLon = haversine(lon2_ra-lon1_ra);
	cosLat2 = cos(lat2_ra);
	cosLat1 = cos(lat1_ra);

	float sum = sqrt(haverLat + cosLat1*cosLat2*haverLon);
	float tmp = asin(sum);
	float distance = 2 * EARTH_RADIUS * tmp; //in metres

	return distance;
}

Point latLonToPoint(float lat, float lon)
{
	float x, y;
	//crude method to account for variations in radii parallel to equator as latitude changes
	x = (distanceBetweenTwoLatLons(lat_origin,lon_origin,lat_origin,lon)+distanceBetweenTwoLatLons(lat,lon_origin,lat,lon))/2;
	y = distanceBetweenTwoLatLons(lat_origin,lon,lat,lon);
	Point p(x,y);
	return p;
}

void pointToLatLon(const Point& p, float *lat, float *lon)
{
	float x, y;
	x = p.getX();
	y = p.getY();

	//returns the lat and lon through pass by reference
	//easy way to do this, too difficult in other implementations
	//note: permissible due to difference in latitude longitude being minute
	*lat = lat_origin + x/X_MAX*lat_diff;
	*lon = lon_origin + y/Y_MAX*lon_diff;
}

//set up the trilateration and location code by first setting the area where
//location code can be effectively used
void setupPermissibleArea(float lat1, float lon1, float lat2, float lon2)
{
	//crude method to account for variations in radii parallel to equator as latitude changes
	X_MAX = (distanceBetweenTwoLatLons(lat1,lon1,lat1,lon2)+distanceBetweenTwoLatLons(lat2,lon1,lat2,lon2))/2;
	Y_MAX = distanceBetweenTwoLatLons(lat1,lon1,lat2,lon1);

	//get bottom left hand corner and set it as lat/lon origin
	lat_origin = std::min(lat1,lat2);
	lon_origin = std::min(lon1,lon2);

	//set difference as absolute value
	lat_diff = fabs(lat1-lat2);
	lon_diff = fabs(lon1-lon2);
}

void readSamplesFromFile(const char* fileName)
{
#ifdef VERBOSE
	PRINT_ERR("Reading samples from %s\n", fileName);
#endif
	//open filestream
	FILE* fileToRead = fopen(fileName,"r");
	//file does not exit
	if(fileToRead == NULL)
		return;

	//make line buffer
	char line[BUFSIZ];
	float lat, lon;
	char name[18]; //TODO: a hardcoded value, perhaps should be in a constants header?
	int n;

	//get line of csv file
	while(fgets(line, sizeof line, fileToRead ))
	{
		//sscanf a file formatted as csv in following form:
		//MAC Address,latitude,longitude,number of previous samples
		sscanf(line,"%[^\t\n,],%f,%f,%d",name,&lat,&lon,&n);
		if(ferror(fileToRead)) //if there was an error reading this line
		{
			PRINT_ERR("Error whilst reading from file %s, expected a line of \"%%s,%%f,%%f,%%d\" but encountered error %d on stream\n", fileName,ferror(fileToRead));
			//if(feof(fileToRead)) //unexpected eof
			break;
			/*else
				clearerr(fileToRead); //continue
			continue;*/
		}
		std::string cppName(name);
		//insert into previous sample mape
		macSamples[cppName] = std::pair<Point,int>(latLonToPoint(lat,lon),n);
	}
	//close stream
	fclose(fileToRead);
}

void writeSamplesToFile(const char* fileName)
{
#ifdef VERBOSE
	PRINT_ERR("Writing samples to %s\n", fileName);
#endif
	//open filestream
	FILE* fileToWrite = fopen(fileName,"w+");

	//construct iterator to beginning of map 
	std::map<std::string, std::pair<Point, int> >::iterator sample = macSamples.begin();
	for(; sample != macSamples.end(); sample++)
	{
		float lat, lon;
		//convert to latitude and longitude
		pointToLatLon((sample->second).first, &lat, &lon);
		//write in csv format as follows:
		//MAC Address,latitude,longitude,number Of Samples
		fprintf(fileToWrite,"%s,%f,%f,%d\n",(sample->first).c_str(),lat,lon,(sample->second).second);
	}

	//close filestream
	fclose(fileToWrite);
}
