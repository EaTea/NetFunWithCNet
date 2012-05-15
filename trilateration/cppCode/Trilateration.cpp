#include "trilat.h"

Point __trilaterate(const Circle& a, const Circle& b, const Circle& c);

Point trilaterate(const Circle& a, const Circle& b, const Circle& c)
{
	Point ca, cb, cc;
	ca = a.getCenter();
	cb = b.getCenter();
	cc = c.getCenter();
	float dab = getDistance(ca, cb);
	//float dac = getDistance(ca, cc);
	//float dbc = getDistance(cb, cc);
	float ra, rb, rc;
	ra = a.getRadius(); rb = b.getRadius(); rc = c.getRadius();
	bool abIntersect;//, acIntersect, bcIntersect;
	abIntersect = dab-ra < rb && rb < dab+ra;
	//acIntersect = dac-ra < rc && rc < dac+ra;
	//bcIntersect = dbc-rb < rc && rc < dbc+rb;
	DEBUG("Trilaterating:\n(%f, %f) %f\n(%f, %f) %f\n(%f,%f) %f\n",
				ca.getLat(), ca.getLon(), ra,
				cb.getLat(), cb.getLon(), rb,
				cc.getLat(), cc.getLon(), rc);
	if(!abIntersect)//|| !acIntersect || !bcIntersect)
	{
		DEBUG("Error: circles must intersect at least two points\n");
		throw TRIANGLES_DO_NOT_INTERSECT;
	}
	return __trilaterate(a, b, c);
}


Point __trilaterate(const Circle& a, const Circle& b, const Circle& c)
{
	float xa, xc, ya, yc, ra, rb, rc;
	Point ca, cb, cc;
	ca = a.getCenter();
	cb = b.getCenter();
	cc = c.getCenter();
	xa = ca.getLat(); xc = cc.getLat();
	ya = ca.getLon(); yc = cc.getLon();
	ra = a.getRadius(); rb = b.getRadius(); rc = c.getRadius();
	float d = getDistance(ca, cb);
	float x = (pow(ra,2)-pow(rb,2)+pow(d,2))/(2*d);
	float y = (pow(ra, 2)-pow(rc,2)+pow(xa-xc,2)+pow(ya-yc,2))/(2*(ya-yc)) - x*(xa-xc)/(ya-yc);
	return Point(x, y);
}
