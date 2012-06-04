#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#include <vector>
#include <map>
#include <string>
#include <algorithm>

#include "ipod3230.h"

//following are exceptions
#define RADIUS_LESS_THAN_0 1
#define CIRCLES_DO_NOT_INTERSECT 2
#define NO_POINT_SAMPLES_GENERATED 4
#define CIRCLE_CENTRES_ARE_COLINEAR 8

//if this line is uncommented it will specify that the program should output
//verbosely with all its debug statements after recompilation
//#define VERBOSE

/*
	Implemented in Point.cpp
	Representation of a Point in the 2d plane
*/
class Point {
	private:
		//latitude of the point
		float 	x;
		//longitude of the point
		float 	y;
	public: 
		float 	getX	() const;
		float 	getY	() const;
		void 		setX		(
											float		//new latitude
										);
		void 		setY		(
											float		//new longitude
										);
						Point		(
											float=0,//latitude, default 0
											float=0	//longitude, default 0
										);
		//Effectively an object clone
						Point		(
											const Point&		//reference to Point to clone
										);
		//Minus operator
		Point operator-			(
													const Point& //other location
												) const;
		//Plus operator
		Point operator+			(
													const Point& //other location
												) const;
		//Dot-product operator
		float 	operator^	(
												const Point& //dot product value
											) const;
		//Scalar multiplication operator
		Point 	operator*	(
												const float //dot product value
											) const;


};

/*
	Implemented in Circle.cpp
	Representation of a 2D circle, will essentially contract to a single Point if
	radius is specified as 0
*/
class Circle {
	private:
		//circle center
		Point		center;
		//circle radius, must be >= zero
		float				radius;
	public:
		//returns a clone of the center
		Point		getCenter		() const;
		//returns the radius
		float				getRadius		() const;
		//sets the center of the circle to the given point
		void				setCenter		(
															Point&	//new center, must clone Point
														);
		//sets the radius to the given value, new radius must be >= 0
		void				setRadius		(
															float		//new radius, must be >= 0
														);
		//constructor using Center and Radius
								Circle			(
															Point&,	//new center
															float=0		//new radius, must be >= 0
														);
		//constructor that takes the center of the Circle and point on the circumference
								Circle			(
															Point&,	//new center
															Point&	//point on circumference of circle
														);
		//construct a circle from another circle, effectively a clone
								Circle			(
															const Circle&	//circle to clone from
														);
};


		//implemented in Util.cpp prints a string to stderr like printf
		void PRINT_ERR(const char*,...);

		//Implemented in Util.cpp
		//Calculate the distance between two points
		float		getDistance	(
													const Point&,
													const Point&
												);

		//Implemented in Trilateration.c
		//Calculate the intersection of three points
		//throws exception if the triplet of circles do not each intersect
		Point	trilaterate			(
														const Circle&,
														const Circle&,
														const Circle&
													);

		//Implemented in Util.cpp
		//convert a point to a unit vector
		//that is, suppose that Point p is distance d from the origin
		//then getUnitPoint(p) returns p_unit such that
		//d * p_unit = p and the distance from p_unit to the origin is 1
		Point getUnitPoint		(
														const Point&
													);
		/*
			Implemented in Util.cpp
			Find the Perpendicular dot product of two points in 2D space
			Essentially a cross product but both points in the plane Z = 0
		*/
		float perpProduct			(
														const Point&,
														const Point&
													);

		/*
			Implemented in Util.cpp
			Returns true if three points are colinear, uses the cross product or
			perpendicular dot product to determine if points are colinear, that is
			all three are on the SAME line
		*/
		bool areColinear			(
														const Point&,
														const Point&,
														const Point&
													);

/*
	Implemented in Location.cpp
	Reads all the values from a file specified by the const char* parameter
	to the map in Location.cpp
	Does not modify file contents
	Assumes file has a csv structure and will stop reading file early if it
	encounters file not in csv format
*/
void readSamplesFromFile	(
														const char* //filename (path) of the file to read data from
													);
/*
	Implemented in Location.cpp
	Serialises all the values in Location.h's map to a file specified by the
	const char* parameter
	Overwrites current contents of the file
	Writes in csv format
*/
void writeSamplesToFile		(
														const char* //filename (path) of file to write data to
													);

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
*/
bool	findCurrentPosition	(
														const std::vector<std::string>&, //WAPs that can be heard, identified by MAC
														const std::vector<int32_t>&, //rssis of WAPs
														float *, //latitude
														float * //longitude
													);

/*
	Implemented in SampleProcessing.cpp
	Given n iPod samples, processes the samples and trilaterates the samples
	accordingly, by finding which of the MACs these samples are related to and
	recording them, then performing trilateration if there are at least 20 samples
*/
void processSamples	(
											int, //number of samples
											IPOD_SAMPLE * //pointer to samples
										);

/*
	Implemented in SampleProcessing.cpp
	Given all the APs broadcasting, processes the samples, trilaterates and returns
	the most likely position that these MACs intersect at
	returns 0 if successfully trilaterated, and 1 otherwise
*/
int processLocation	(
											int, //number of APs
											APINFO *, //pointer to APs
											float*, //latitude
											float* //longitude
										);
