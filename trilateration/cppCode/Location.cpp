#include "trilat.h"

float Location::getLat() const
{
	return lat;
}

float Location::getLon() const
{
	return lon;
}

void Location::setLat	(float newLat)
{
	lat = newLat;
}

void Location::setLon	(float newLon)
{
	lon = newLon;
}

Location::Location		(float newLat, float newLon)
{
	this->setLat(newLat);
	this->setLon(newLon);
}

Location::Location		(const Location& other)
{
	this->setLat(other.getLat());
	this->setLon(other.getLon());
}
