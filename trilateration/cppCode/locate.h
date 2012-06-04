#include "debug.h"

#include <vector>
#include <string>
#include <stdarg.h>
#include <stdint.h>

/*
	Implemented in Location.cpp
	A function designed to be called at beginning of program, sets up trilateration
	so that the differences in the bounding rectangle in metres can be calculated
*/
void setupPermissibleArea	(	
														float, //latitude 1
														float, //longitude 1
														float, // latitude 2
														float //longitude 2
													);

/*
	Implemented in Location.cpp
	Generates the WAP position for a given MAC address, given that it is passed a vector
	of floats, a vector of floats and a vector of ints representing latitudes, longitudes
	and rssis respectively.
	Function expects that each of these vectors have the same size, and that at least three
	samples are involved when calling this function.
	Returns true if generation succeeded, false otherwise
*/
bool generateWAPPosition	(
														const std::vector<float>&, //latitudes
														const std::vector<float>&, //longitudes
														const std::vector<int>&, //rssis
														const std::string& //mac address
													);

/*
	Implemented in Location.cpp
	Finds the location in latitude and longitude given a vector of strings and ints
	each of which is expected to be of same size.
	Uses a pass-by-reference to return the Latitude and Longitude
	Returns true if trilateration and locating code succeeded, false otherwise
*/
bool findCurrentPosition	(
														const std::vector<std::string>&, //WAPs that can be heard, identified by MAC
														const std::vector<int32_t>&, //rssis of WAPs
														float *, //latitude
														float * //longitude
													);
