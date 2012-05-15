#include "trilat.h"
int main()
{
	/*
	Point l;
	Circle a(l,(float)5.0), b(l,(float)5.0), c(l,(float)5.0); //test case for all being the same
	Point t1 = trilaterate(a, b, c);
	std::cout << "All are exactly the same circle" << std::endl;
	std::cout << t1.getLat() << ' ' << t1.getLon() << std::endl;
	
	Point b2 = Point((float)15.0,(float) 0.0);
	Point c2 = Point((float)-16.0,(float)0.0);
	b = Circle(b2,(float)5.0);
	c = Circle(c2,(float)0.2); //all three do not intersect
	t1 = trilaterate(a, b, c);
	std::cout << "Two intersect, one does not" << std::endl;
	std::cout << t1.getLat() << ' ' << t1.getLon() << std::endl;

	
	b2 = Point((float)2.0,(float) 0.0);
	c2 = Point((float)-6.0,(float)0.0);
	b = Circle(b2,(float)5.0);
	c = Circle(c2,(float)0.2); //two intersect, one does not
	t1 = trilaterate(a, b, c);
	std::cout << "Two intersect, one does not" << std::endl;
	std::cout << t1.getLat() << ' ' << t1.getLon() << std::endl;
	
	b2 = Point((float)1.0,(float) 0.0);
	c2 = Point((float)-1.0,(float)0.0);
	b = Circle(b2,(float)0.2);
	c = Circle(c2,(float)0.2); //both inside bigger, no intersections
	t1 = trilaterate(a, b, c);
	std::cout << "Two triangles inside a bigger one" << std::endl;
	std::cout << t1.getLat() << ' ' << t1.getLon() << std::endl;
	
	b2 = Point((float)3.0,(float) 0.0);
	c2 = Point((float)7.0,(float)0.0);
	b = Circle(b2,(float)2.0);
	c = Circle(c2,(float)2.0); //touches at exactly one point
	t1 = trilaterate(a, b, c);
	std::cout << "All three triangles intersect at exactly one point" << std::endl;
	std::cout << t1.getLat() << ' ' << t1.getLon() << std::endl;*/

	Point l;
	Circle a(l,(float)5.0), b(l,(float)5.0), c(l,(float)5.0); //test case for all being the same
	Point t1;
	Point b2 = Point((float)15.0,(float) 0.0);
	Point c2 = Point((float)-16.0,(float)0.0);

	b2 = Point((float)3.0,(float) 0.0);
	c2 = Point((float)7.0,(float)5.0);
	b = Circle(b2,(float)3.0);
	c = Circle(c2,(float)6.0); //three way intersection
	t1 = Point(trilaterate(a, b, c));
	std::cout << "Three way intersection" << std::endl;
	std::cout << t1.getLat() << ' ' << t1.getLon() << std::endl;

	b2 = Point((float)6.0,(float) 0.0);
	c2 = Point((float)3.0,(float)10.0);
	b = Circle(b2,(float)5.0);
	c = Circle(c2,(float)6.0); //three way intersection
	t1 = trilaterate(a, b, c);
	std::cout << "two intersect, one touches at a specific point" << std::endl;
	std::cout << t1.getLat() << ' ' << t1.getLon() << std::endl;
}
