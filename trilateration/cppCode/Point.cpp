#include "trilat.h"

float Point::getX() const
{
	return x;
}

float Point::getY() const
{
	return y;
}

void Point::setX	(float newX)
{
	x = newX;
}

void Point::setY	(float newY)
{
	y = newY;
}

Point::Point		(float newX, float newY)
{
	this->setX(newX);
	this->setY(newY);
}

Point::Point		(const Point& other)
{
	this->setX(other.getX());
	this->setY(other.getY());
}

Point Point::operator+ (const Point& l) const
{
	Point n;
	n.setX(l.getX()+this->getX());
	n.setX(l.getY()+this->getY());
	return n;
}

Point Point::operator- (const Point& l) const
{
	Point n;
	n.setX(this->getX()-l.getX());
	n.setY(this->getY()-l.getY());
	return n;
}

float Point::operator^ (const Point& l) const
{
	float f = (this->getX() * l.getX()) + (this->getY() * l.getY());
	return f;
}

Point Point::operator* (const float f) const
{
	Point p;
	p.setX(f*this->getX());
	p.setY(f*this->getY());
	return p;
}
