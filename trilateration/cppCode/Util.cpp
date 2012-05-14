#include "trilat.h"

void DEBUG(const char* s,...)
{
	va_list argptr;
	va_start(argptr, s);
	vfprintf(stderr, s, argptr);
	va_end(argptr);
}

float getDistance(Location& l1, Location& l2)
{
	float x1, x2, y1, y2;
	x1 = l1.getLat();
	x2 = l2.getLat();
	y1 = l1.getLon();
	y2 = l1.getLon();
	float x, y;
	x = x1 - x2;
	y = y1 - y2;
	return sqrt(pow(x, 2)+pow(y, 2));
}