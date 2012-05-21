#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#define RADIUS_LESS_THAN_0 1
#define CIRCLES_DO_NOT_INTERSECT 2
#define NO_POINT_SAMPLES_GENERATED 4

class Point {
	private:
		//latitude of the point
		float 	x;
		//longitude of the point
		float 	y;
	public: 
		float 	getX	() const;
		float 	getY	() const;
		void 		setX	(
											float		//new latitude
										);
		void 		setY	(
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
		Point operator-	(
													const Point& //other location
												) const;
		//Plus operator
		Point operator+	(
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

		//Calculate the distance between two points
		float		getDistance	(
													const Point&,
													const Point&
												);

		//Calculate the intersection of three points
		//throws exception if can't be used
		Point	trilaterate	(
														const Circle&,
														const Circle&,
														const Circle&
													);

		Point getUnitPoint(const Point&);

		void PRINT_ERR(const char*,...);

		Point locatePosition(const std::vector<Circle>&);

		float distanceBetweenTwoLatLons(float,float,float,float);

		void boundingBoxInMetres(float, float, float, float);
