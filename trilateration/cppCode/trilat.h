#include <iostream>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <math.h>

#include "../../ipod3230.h"

#define RADIUS_LESS_THAN_0 1

class Location {
	private:
		//latitude of the point
		float 	lat;
		//longitude of the point
		float 	lon;
	public: 
		float 	getLat	() const;
		float 	getLon	() const;
		void 		setLat	(
											float		//new latitude
										);
		void 		setLon	(
											float		//new longitude
										);
						Location(
											float=0,//latitude, default 0
											float=0	//longitude, default 0
										);
		//Effectively an object clone
						Location(
											const Location&		//reference to Point to clone
										);
};

class Circle {
	private:
		//circle center
		Location		center;
		//circle radius, must be >= zero
		float				radius;
	public:
		//returns a clone of the center
		Location		getCenter		() const;
		//returns the radius
		float				getRadius		() const;
		//sets the center of the circle to the given point
		void				setCenter		(
															Location&	//new center, must clone Point
														);
		//sets the radius to the given value, new radius must be >= 0
		void				setRadius		(
															float		//new radius, must be >= 0
														);
		//constructor using Center and Radius
								Circle			(
															Location&,	//new center
															float=0		//new radius, must be >= 0
														);
		//constructor that takes the center of the Circle and point on the circumference
								Circle			(
															Location&,	//new center
															Location&	//point on circumference of circle
														);
		//construct a circle from another circle, effectively a clone
								Circle			(
															const Circle&	//circle to clone from
														);
};

		//Calculate the distance between two points
		float		getDistance	(
													Location&,
													Location&
												);

		//Calculate the intersection of three points
		//throws exception if can't be used
		Location	trilaterate	(
														Circle&,
														Circle&,
														Circle&
													);
