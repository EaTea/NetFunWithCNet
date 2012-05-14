#include "trilat.h"

Location	Circle::getCenter					() const
{
	Location l(center);
	return l;
}

float			Circle::getRadius					() const
{
	return radius;
}

void			Circle::setCenter					(
																			Location& newCenter
																		)
{
	center = newCenter;
}

void			Circle::setRadius					(
																			float newRadius
																		)
{
	if(newRadius < 0)
	{
		DEBUG("Error: %f was passed as radius parameter but radius should be more than or equal to 0", newRadius);
		throw RADIUS_LESS_THAN_0;
	}
	radius = newRadius;
}

Circle::Circle											(
																			Location& newCenter,
																			float newRadius
																		)
{
	this->setCenter(newCenter);
	try
	{
		setRadius(newRadius);
	}
	catch(int e)
	{
		switch(e)
		{
			case RADIUS_LESS_THAN_0:
				setRadius(0);
				break;
			default:
				throw;
		}
	}
}

Circle::Circle											(
																			Location& newCenter,
																			Location& ptOnCircumference
																		)
{
	setCenter(newCenter);
	try
	{
		setRadius(getDistance(newCenter, ptOnCircumference));
	}
	catch(int e)
	{
		switch(e)
		{
			case RADIUS_LESS_THAN_0:
				setRadius(0);
				break;
			default:
				throw;
		}
	}
}

Circle::Circle											(
																			const Circle& other
																		)
{
	Location l(other.getCenter());
	this->setCenter(l);
	try
	{
		this->setRadius(other.getRadius());
	}
	catch(int e)
	{
		switch(e)
		{
			case RADIUS_LESS_THAN_0:
				setRadius(0);
				break;
			default:
				throw;
		}
	}
}
