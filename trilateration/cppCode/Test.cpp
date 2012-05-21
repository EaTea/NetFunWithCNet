#include "locate.h"

using namespace std;

Point origin;

void testGeneric(int i, Circle& c1, Circle& c2, Circle& c3)
{
	try {
		Point p = trilaterate(c1, c2, c3);
		PRINT_ERR("Test %d: (%f, %f)\n",i,p.getX(),p.getY());
	}
	catch(int e)
	{
		if(e == CIRCLES_DO_NOT_INTERSECT)
		{
			PRINT_ERR("Test %d: Failure due to no intersection\n", i);
			//throw;
		}
		else
		{
			exit(1);
		}
	}
}

/*
	If all three circles are identical
*/
void testOne()
{
	int i = 1;
	Circle c1(origin, 10);
	Circle c2(c1);
	Circle c3(c2);
	testGeneric(i, c1, c2, c3);
}

/*
	If all three circles are completely cut off from each other
*/
void testTwo()
{
	int i = 2;
	Circle c1(origin, 3);
	Point b(10, 10);
	Circle c2(b, 2);
	Point c(-10, -10);
	Circle c3(c, 2);
	testGeneric(i, c1, c2, c3);
}

/*
	If two circles are identical and one is non-intersecting
*/
void testThree()
{
	int i = 3;
	Circle c1(origin,3);
	Circle c2(c1);
	Point c(-10,10);
	Circle c3(c, 4);
	testGeneric(i, c1, c2, c3);
}

/*
	If two circles are identical and third is intersecting
*/
void testFour()
{
	int i = 4;
	Circle c1(origin,3);
	Circle c2(c1);
	Point c(-3,4);
	Circle c3(c, 6);
	testGeneric(i, c1, c2, c3);
}

/*
	If two circles are intersecting and third is not
*/
void testFive()
{
	int i = 5;
	Circle c1(origin,4);
	Point b(7,0);
	Circle c2(b, 4);
	Point c(-10,10);
	Circle c3(c, 2);
	testGeneric(i, c1, c2, c3);
}

/*
	If two circles are intersecting and third intersects at one point
*/
void testSix()
{
	int i = 6;
	Circle c1(origin,5);
	Point b(6,0);
	Circle c2(b, 5);
	Point c(3,10);
	Circle c3(c, 6);
	testGeneric(i, c1, c2, c3);
}

/*
	If all three circles intersect in a region.
*/
void testSeven()
{
	int i = 7;
	Circle c1(origin, 5);
	Point b(6, 0);
	Circle c2(b, 5);
	Point c(3, -8);
	Circle c3(c, 5);
	testGeneric(i, c1, c2, c3);
}

int main()
{
	testOne();
	testTwo();
	testThree();
	testFour();
	testFive();
	testSix();
	testSeven();
}
