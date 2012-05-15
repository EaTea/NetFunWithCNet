#include "trilat.h"

float Point::getLat() const
{
	return lat;
}

float Point::getLon() const
{
	return lon;
}

void Point::setLat	(float newLat)
{
	lat = newLat;
}

void Point::setLon	(float newLon)
{
	lon = newLon;
}

Point::Point		(float newLat, float newLon)
{
	this->setLat(newLat);
	this->setLon(newLon);
}

Point::Point		(const Point& other)
{
	this->setLat(other.getLat());
	this->setLon(other.getLon());
}

Point Point::operator+ (const Point& l) const
{
	Point n;
	n.setLat(l.getLat()+this->getLat());
	n.setLat(l.getLon()+this->getLon());
	return n;
}

Point Point::operator- (const Point& l) const
{
	Point n(l);
	n.setLat(-n.getLat());
	n.setLat(-n.getLon());
	return *(this)-n;
}

float Point::operator^ (const Point& l) const
{
	float f = (this->getLat() * l.getLat()) + (this->getLon() * l.getLon());
	return f;
}
