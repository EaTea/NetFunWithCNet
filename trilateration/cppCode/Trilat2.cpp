#include "trilat.h"

Point __trilaterate(const Circle&, const Circle&, const Circle&);

void findIntersections(std::vector<Point>&, const Circle&, const Circle&);

Point centroidIntersection(const Circle& a, const Circle& b);

Point polygonCentroid(std::vector<Point>&);

Point trilaterate(const Circle& c1, const Circle& c2, const Circle& c3)
{
	Point p1, p2, p3;
	p1 = c1.getCenter();
	p2 = c2.getCenter();
	p3 = c3.getCenter();

	float r1, r2, r3;
	r1 = c1.getRadius();
	r2 = c2.getRadius();
	r3 = c3.getRadius();

	float d12, d13, d23;
	d12 = getDistance(p1,p2);
	d13 = getDistance(p1,p3);
	d23 = getDistance(p2,p3);

	DEBUG("Attempting to trilaterate:\n");
	DEBUG("Centre:(%f, %f) Radius:%f\n",p1.getX(), p1.getY(), r1);
	DEBUG("Centre:(%f, %f) Radius:%f\n",p2.getX(), p2.getY(), r2);
	DEBUG("Centre:(%f, %f) Radius:%f\n",p3.getX(), p3.getY(), r3);

	bool inter12, inter13, inter23;
	inter12 = d12-r1 < r2 && r2 < d12+r1;
	inter13 = d13-r1 < r3 && r3 < d13+r1;
	inter23 = d23-r2 < r3 && r3 < d23+r3;

	if(!(inter12 && inter13 && inter23))
	{
		DEBUG("Error: each pair of circles must intersect with at least two points\n");
		throw CIRCLES_DO_NOT_INTERSECT;
	}
	return __trilaterate(c1, c2, c3);
}

Point __trilaterate(const Circle& c1, const Circle& c2, const Circle& c3)
{
	std::vector<Point> intersections;
	findIntersections(intersections,c1,c2);
	findIntersections(intersections,c1,c3);
	findIntersections(intersections,c2,c3);
	
	return polygonCentroid(intersections);
}

Point polygonCentroid(std::vector<Point>& p)
{
	//indices of 2 closest intersection points
	//assumed for form a cluster
	unsigned int i0 = 0, i1 = 1;
	float minD = getDistance(p[i0],p[i1]);
	for(unsigned int i = 0; i < p.size(); i++)
	{
		for(unsigned int j = i+1; j < p.size(); j++)
		{
			float d = getDistance(p[i],p[j]);
			if(d < minD)
			{
				i0 = i;
				i1 = j;
				minD = d;
			}
		}
	}
	//index of 3rd closest intersection point
	unsigned int i2 = (i0+1)%p.size() == i1 ? (i1+1)%p.size() : (i0+1)%p.size();
	float min2D = getDistance(p[i0],p[i2]);
	for(unsigned int i = 0; i < p.size(); i++)
	{
		if(i == i0 || i == i1) continue;
		float d = getDistance(p[i0],p[i]);
		if(d < min2D)
		{
			i2 = i;
			min2D = d;
		}
	}
	DEBUG("Points used are as follows:\n");
	DEBUG("%f %f\n",p[i1].getX(),p[i1].getY());
	DEBUG("%f %f\n",p[i2].getX(),p[i2].getY());
	DEBUG("%f %f\n",p[i0].getX(),p[i0].getY());
	Point centroid;
	centroid.setX((p[i0].getX()+p[i1].getX()+p[i2].getX())/3.0);
	centroid.setY((p[i0].getY()+p[i1].getY()+p[i2].getY())/3.0);
	DEBUG("%f %f\n",centroid.getX(),centroid.getY());
	return centroid;
}

void findIntersections(std::vector<Point>& p, const Circle& a, const Circle& b)
{
	Point mid = centroidIntersection(a,b);
	DEBUG("Circles intersect at: (%f %f)\n",mid.getX(),mid.getY());
	Point ca = a.getCenter();
	Point cb = b.getCenter();

	float deltaX, deltaY;
	deltaX = cb.getX() - ca.getX();
	deltaY = cb.getY() - ca.getY();

	float s = getDistance(ca,mid);
	float ra = a.getRadius();
	float d = getDistance(ca, cb);

	float u = sqrt(pow(ra,2)-pow(s,2));
	p.push_back(Point(ca.getX() - deltaX * u / d, ca.getY() + deltaY * u / d));
	p.push_back(Point(ca.getX() + deltaX * u / d, ca.getY() - deltaY * u / d));
}

Point centroidIntersection(const Circle& a, const Circle& b)
{
	Point ca, cb;
	ca = a.getCenter();
	cb = b.getCenter();

	float ra, rb;
	ra = a.getRadius();
	rb = b.getRadius();

	float deltaX, deltaY;
	deltaX = cb.getX() - ca.getX();
	deltaY = cb.getY() - ca.getY();

	float d = getDistance(ca,cb);

	float s = (pow(d,2)+pow(ra,2)-pow(rb,2))/(2*d);

	float newX, newY;
	newX = ca.getX()+deltaX * s/d;
	newY = ca.getY()+deltaY * s/d;

	return Point(newX,newY);
}
