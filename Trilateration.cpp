#include "trilaterate.h"

//Internal functions local to this file only
/*-----------------------------------BEGIN------------------------------------*/

/* Trilateration method, takes Circles a, b and c and solves the non-linear trilateration
	equation to determine the point where they intersect
	Assumes that a's centre is the origin, b's centre lies on the x-axis
	Equations from Wikipedia
*/
Point __trilaterate(const Circle& a, const Circle& b, const Circle& c);

/*
	Gets the new x-basis unit vector after transformation for the required transformed co-ordinate system
*/
Point getXUnit(const Point& ca, const Point& cb);

/*
	Finds the magnitude of the horizontal component of the difference between Points ca and cc in the transformed co-ordinate space
*/
float signedMagnitudeX(const Point& ca, const Point& cc, const Point& xUnit);

/*
	Gets the new y-basis unit vector after transformation into the required co-ordinate system
*/
Point getYUnit(const Point& ca, const Point& cc, const float xMag, const Point& xUnit);

/*
	Finds the magnitude of the vertical component of the difference between Points ca and cc in the transformed co-ordinate space
*/
float signedMagnitudeY(const Point& ca, const Point& cc, const Point& yUnit);

/*------------------------------------END-------------------------------------*/

Point trilaterate(const Circle& a, const Circle& b, const Circle& c)
{
	Point ca, cb, cc;
	ca = a.getCenter();
	cb = b.getCenter();
	cc = c.getCenter();

	//if colinear will cause nan errors in calculation
	if(areColinear(ca,cb,cc))
	{
#ifdef VERBOSE
	PRINT_ERR("Error: circle centres are colinear\n");
#endif
		throw CIRCLE_CENTRES_ARE_COLINEAR;
	}

	float dab = getDistance(ca, cb); //distance from a to b
	float dac = getDistance(ca, cc); //distance from a to c
	float dbc = getDistance(cb, cc); //distance from b to c

	float ra, rb, rc;
	ra = a.getRadius(); rb = b.getRadius(); rc = c.getRadius();

	//does a,b intersect? a,c intersect? b,c intersect?
	bool abIntersect, acIntersect, bcIntersect;
	abIntersect = fabs(dab-ra) < rb && rb < dab+ra;
	acIntersect = fabs(dac-ra) < rc && rc < dac+ra;
	bcIntersect = fabs(dbc-rb) < rc && rc < dbc+rb;

	//debug statements
#ifdef VERBOSE
	PRINT_ERR("Attempting to trilaterate:\n");
	PRINT_ERR("Centre:(%f, %f) Radius:%f\n",ca.getX(), ca.getY(), ra);
	PRINT_ERR("Centre:(%f, %f) Radius:%f\n",cb.getX(), cb.getY(), rb);
	PRINT_ERR("Centre:(%f, %f) Radius:%f\n",cc.getX(), cc.getY(), rc);
#endif
	//insufficient number of intersections will result in throwing exception
	if(!abIntersect || !acIntersect || !bcIntersect)
	{
#ifdef VERBOSE
	PRINT_ERR("Error: each pair of circles must intersect with at least two points\n");
#endif
		throw CIRCLES_DO_NOT_INTERSECT;
	}
	return __trilaterate(a, b, c);
}

Point __trilaterate(const Circle& a, const Circle& b, const Circle& c)
{
	//radii of each circle
	float ra, rb, rc;
	ra = a.getRadius();
	rb = b.getRadius();
	rc = c.getRadius();

	//center of each circle
	Point ca, cb, cc;
	ca = a.getCenter();
	cb = b.getCenter();
	cc = c.getCenter();

	//x basis vector of transformed co-ordinate space in original space
	Point xUnitVector = getXUnit(ca, cb);
	//"horizontal" component (transformed space) of difference between circle centers
	float i = signedMagnitudeX(ca, cc, xUnitVector);
	//y basis vector of transformed co-ordinate space in original space
	Point yUnitVector = getYUnit(ca, cc, i, xUnitVector);	
	//"vertical" component (transformed space) of difference between circle centers
	float j = signedMagnitudeY(ca, cc, yUnitVector);

	//N.B.: ca + xUnitVector * i + yUnitVector * j = cc
	//and ca + xUnitVector * d = cb

	float d = getDistance(ca, cb);
	//the intersection point relative to origin in transformed co-ordinate space
	float x = (powf(ra,2)-powf(rb,2)+powf(d,2))/(2*d);
	float y = ((powf(ra, 2)-powf(rc,2)+powf(i,2)+powf(j,2))/(2*j)) - x*(i)/(j);

	//transform x, y to original co-ordinate frame
	Point xMagVector = xUnitVector*x, yMagVector = yUnitVector*y;
	Point p = ca + xMagVector + yMagVector;
	return p;
}

//N.B.: the following functions, as specified in the beginning of the file, are
//used to transform the initial co-ordinate space to a new co-ordinate space
//by generating new basis unit vectors for the horizontal and vertical
//directions in the transformed space and finding the location of the points
//in the transformed space

Point getXUnit(const Point& ca, const Point& cb)
{
	Point nonUnitVector = cb - ca;
	return getUnitPoint(nonUnitVector); //normalise to unit vector
}

float signedMagnitudeX(const Point& ca, const Point& cc, const Point& xUnit)
{
	Point p3mp1 = cc - ca;
	return xUnit ^ p3mp1; //dot product between both vectors
}

Point getYUnit(const Point& ca, const Point& cc, const float xMag, const Point& xUnit)
{
	Point xTimesXMag = xUnit * xMag;//scalar multiplication
	Point nonUnitVector = cc - ca - xTimesXMag;
	return getUnitPoint(nonUnitVector); //normalise to unit vector
}

float signedMagnitudeY(const Point& ca, const Point& cc, const Point& yUnit)
{
	Point p3mp1 = cc - ca;
	return yUnit ^ p3mp1; //dot product between both vectors
}
