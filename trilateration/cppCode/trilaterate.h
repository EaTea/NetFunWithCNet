#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#include "debug.h"

//following are exceptions
#define RADIUS_LESS_THAN_0 1
#define CIRCLES_DO_NOT_INTERSECT 2
#define NO_POINT_SAMPLES_GENERATED 4
#define CIRCLE_CENTRES_ARE_COLINEAR 8

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
