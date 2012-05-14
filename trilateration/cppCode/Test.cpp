#include "trilat.h"

int main()
{
	Location l;
	Circle a(l,(float)5.0), b(l,(float)5.0), c(l,(float)5.0); //test case for all being the same
	Location t1 = trilaterate(a, b, c);
	std::cout << "All are exactly the same circle" << std::endl;
	std::cout << t1.getLat() << ' ' << t1.getLon() << std::endl;

	Location b2 = Location((float)15.0,(float) 0.0);
	Location c2 = Location((float)-16.0,(float)0.0);
	b = Circle(b2,(float)5.0);
	c = Circle(c2,(float)0.2); //all three do not intersect
	t1 = trilaterate(a, b, c);
	std::cout << "Two intersect, one does not" << std::endl;
	std::cout << t1.getLat() << ' ' << t1.getLon() << std::endl;

	b2 = Location((float)2.0,(float) 0.0);
	c2 = Location((float)-6.0,(float)0.0);
	b = Circle(b2,(float)5.0);
	c = Circle(c2,(float)0.2); //two intersect, one does not
	t1 = trilaterate(a, b, c);
	std::cout << "Two intersect, one does not" << std::endl;
	std::cout << t1.getLat() << ' ' << t1.getLon() << std::endl;
	
	b2 = Location((float)1.0,(float) 0.0);
	c2 = Location((float)-1.0,(float)0.0);
	b = Circle(b2,(float)0.2);
	c = Circle(c2,(float)0.2); //both inside bigger, no intersections
	t1 = trilaterate(a, b, c);
	std::cout << "Two triangles inside a bigger one" << std::endl;
	std::cout << t1.getLat() << ' ' << t1.getLon() << std::endl;

	b2 = Location((float)3.0,(float) 0.0);
	c2 = Location((float)7.0,(float)0.0);
	b = Circle(b2,(float)2.0);
	c = Circle(c2,(float)2.0); //touches at exactly one point
	t1 = trilaterate(a, b, c);
	std::cout << "All three triangles intersect at exactly one point" << std::endl;
	std::cout << t1.getLat() << ' ' << t1.getLon() << std::endl;

	b2 = Location((float)3.0,(float) 0.0);
	c2 = Location((float)7.0,(float)5.0);
	b = Circle(b2,(float)3.0);
	c = Circle(c2,(float)8.0); //three way intersection
	t1 = trilaterate(a, b, c);
	std::cout << "Three way intersection" << std::endl;
	std::cout << t1.getLat() << ' ' << t1.getLon() << std::endl;
}
