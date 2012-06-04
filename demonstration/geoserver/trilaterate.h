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
#include "locate.h"

//exception codes, assumed to be integers
#define RADIUS_LESS_THAN_0 1
#define CIRCLES_DO_NOT_INTERSECT 2
#define NO_POINT_SAMPLES_GENERATED 4
#define CIRCLE_CENTRES_ARE_COLINEAR 8

//if this line is uncommented it will specify that the program should output
//verbosely with all its debug statements after recompilation
#define VERBOSE

/*
	Implemented in Point.cpp (all functions)
	Representation of a Point in the 2d plane
	Essentially, assume this is a 3D Point in the plane z = 0
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
		//usage is Point * int
		Point 	operator*	(
												const float //dot product value
											) const;


};

/*
	Implemented in Circle.cpp (all functions)
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
		//if receives radius < 0, throws exception code RADIUS_LESS_THAN_0
		void				setRadius		(
															float		//new radius, must be >= 0
														);
		//constructor using Center and Radius
		//if new radius is less than 0, sets radius of circle to 0
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
		//throws exception if the triplet of circles do not each intersect or are colinear
		//throws CIRCLES_DO_NOT_INTERSECT if circles do not have sufficient number of intersections
		//throws CIRCLE_CENTRES_ARE_COLINEAR if circle centres are colinear
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
														float *, //latitude, if successful latitude will be stored here
														float * //longitude, if successful longitude will be stored here
													);
