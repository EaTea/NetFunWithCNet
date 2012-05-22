#include "trilaterate.h"

void PRINT_ERR(const char* s,...)
{
	va_list argptr;
	va_start(argptr, s);
	vfprintf(stderr, s, argptr);
	va_end(argptr);
}

float getDistance(const Point& l1, const Point& l2)
{
	float x1, x2, y1, y2;
	x1 = l1.getX();
	x2 = l2.getX();
	y1 = l1.getY();
	y2 = l2.getY();
	float x, y;
	x = x1 - x2;
	y = y1 - y2;
	return sqrt(pow(x, 2)+pow(y, 2));
}

Point getUnitPoint(const Point& p)
{
	Point origin;
	float distanceToOrigin = getDistance(p,origin);
	Point unitPoint;
	unitPoint.setX(p.getX()/distanceToOrigin);
	unitPoint.setY(p.getY()/distanceToOrigin);
	return unitPoint;
}

float perpProduct(const Point& p1, const Point& p2)
{
	float f = p1.getX()*p2.getY()-p1.getY()*p2.getX();
	return f;
}

bool areColinear(const Point& p1, const Point& p2, const Point& p3)
{
	Point p1mp2 = p2 - p1;	
	Point p1mp3 = p3 - p1;
	return perpProduct(p1mp2,p1mp3) == 0;
}
