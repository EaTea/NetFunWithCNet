#include "trilat.h"

int main()
{
	Location a;
	Location b(4, 3);
	Location c(b);
	std::cout << a.getLat() << ' ' << a.getLon() << std::endl;
	std::cout << b.getLat() << ' ' << b.getLon() << std::endl;
	std::cout << c.getLat() << ' ' << c.getLon() << std::endl;
	Circle c1(a);
	Circle c2(b);
	Circle c3(b, 5);
	Circle c4(a, b);
	Circle c5(c4);
	std::cout << c1.getRadius() << ' ' << c1.getCenter().getLat() << ' ' << c1.getCenter().getLon() << std::endl;
	std::cout << c2.getRadius() << ' ' << c2.getCenter().getLat() << ' ' << c2.getCenter().getLon() << std::endl;
	std::cout << c3.getRadius() << ' ' << c3.getCenter().getLat() << ' ' << c3.getCenter().getLon() << std::endl;
	std::cout << c4.getRadius() << ' ' << c4.getCenter().getLat() << ' ' << c4.getCenter().getLon() << std::endl;
	std::cout << c5.getRadius() << ' ' << c5.getCenter().getLat() << ' ' << c5.getCenter().getLon() << std::endl;
}
