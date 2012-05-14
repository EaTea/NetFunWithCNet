#include "trilat.h"

Location trilaterate(Circle& a, Circle& b, Circle& c)
{
	float xa, xc, ya, yc, ra, rb, rc;
	Location ca, cb, cc;
	ca = a.getCenter();
	cb = b.getCenter();
	cc = c.getCenter();
	xa = ca.getLat(); xc = cc.getLat();
	ya = ca.getLon(); yc = cc.getLon();
	ra = a.getRadius(); rb = b.getRadius(); rc = c.getRadius();
	float d = getDistance(ca, cb);
	float x = (pow(ra,2)-pow(rb,2)+pow(d,2))/(2*d);
	float y = (pow(ra, 2)-pow(rc,2)+pow(xa-xc,2)+pow(ya-yc,2))/(2*(ya-yc)) - x*(xa-xc)/(ya-yc);
	return Location(x, y);
}
