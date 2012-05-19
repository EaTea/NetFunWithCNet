#include "trilat.h"

Point __trilaterate(const Circle& a, const Circle& b, const Circle& c);

Point getXUnit(const Point& ca, const Point& cb);

float signedMagnitudeX(const Point& ca, const Point& cc, const Point& xUnit);

Point getYUnit(const Point& ca, const Point& cc, const float xMag, const Point& xUnit);

float signedMagnitudeY(const Point& ca, const Point& cc, const Point& yUnit);

Point trilaterate(const Circle& a, const Circle& b, const Circle& c)
{
	Point ca, cb, cc;
	ca = a.getCenter();
	cb = b.getCenter();
	cc = c.getCenter();

	float dab = getDistance(ca, cb); //distance from a to b
	float dac = getDistance(ca, cc); //distance from a to c
	float dbc = getDistance(cb, cc); //distance from b to c

	float ra, rb, rc;
	ra = a.getRadius(); rb = b.getRadius(); rc = c.getRadius();

	//does a,b intersect? a,c intersect? b,c intersect?
	bool abIntersect, acIntersect, bcIntersect;
	abIntersect = dab-ra < rb && rb < dab+ra;
	acIntersect = dac-ra < rc && rc < dac+ra;
	bcIntersect = dbc-rb < rc && rc < dbc+rb;
	DEBUG("%d %d %d\n", abIntersect, acIntersect, bcIntersect);

	DEBUG("Attempting to trilaterate:\n");
	DEBUG("Centre:(%f, %f) Radius:%f\n",ca.getX(), ca.getY(), ra);
	DEBUG("Centre:(%f, %f) Radius:%f\n",cb.getX(), cb.getY(), rb);
	DEBUG("Centre:(%f, %f) Radius:%f\n",cc.getX(), cc.getY(), rc);
	if(!abIntersect || !acIntersect || !bcIntersect)
	{
		DEBUG("Error: each pair of circles must intersect with at least two points\n");
		throw TRIANGLES_DO_NOT_INTERSECT;
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

	Point xUnitVector = getXUnit(ca, cb);
	float i = signedMagnitudeX(ca, cc, xUnitVector);
	Point yUnitVector = getYUnit(ca, cc, i, xUnitVector);	
	float j = signedMagnitudeY(ca, cc, yUnitVector);

	float d = getDistance(ca, cb);
	float x = (pow(ra,2)-pow(rb,2)+pow(d,2))/(2*d);
	float y = (pow(ra, 2)-pow(rc,2)+pow(i,2)+pow(j,2))/(2*(j)) - x*(i)/(j);

	Point xMagVector = xUnitVector*i, yMagVector = yUnitVector*i;
	Point p = Point(x, y) + xMagVector + yMagVector;
	return p;
}

Point getXUnit(const Point& ca, const Point& cb)
{
	Point nonUnitVector = cb - ca;
	return getUnitPoint(nonUnitVector);
}

float signedMagnitudeX(const Point& ca, const Point& cc, const Point& xUnit)
{
	Point p3mp1 = cc - ca;
	return xUnit ^ p3mp1; //dot product between both vectors
}

Point getYUnit(const Point& ca, const Point& cc, const float xMag, const Point& xUnit)
{
	Point xTimesXMag = xUnit * xMag;
	Point nonUnitVector = cc - ca;
	nonUnitVector = nonUnitVector - xTimesXMag;
	return getUnitPoint(nonUnitVector);
}

float signedMagnitudeY(const Point& ca, const Point& cc, const Point& yUnit)
{
	Point p3mp1 = cc - ca;
	return yUnit ^ p3mp1; //dot product between both vectors
}
