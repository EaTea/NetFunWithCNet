#include "trilaterate.h"

using namespace std;

Point origin;

void testGeneric(int i, Circle& c1, Circle& c2, Circle& c3)
{
	Point p = trilaterate(c1, c2, c3);
	PRINT_ERR("Test %d: (%f, %f)\n",i,p.getX(),p.getY());
}

/*
	If all three circles are identical
*/
bool testOne()
{
	int i = 1;
	Circle c1(origin, 10);
	Circle c2(c1);
	Circle c3(c2);
	try {
		testGeneric(i, c1, c2, c3);
	}
	catch(int e)
	{
		return true;
	}
	return false;
}

/*
	If all three circles are completely cut off from each other
*/
bool testTwo()
{
	int i = 2;
	Circle c1(origin, 3);
	Point b(10, 10);
	Circle c2(b, 2);
	Point c(-10, -10);
	Circle c3(c, 2);
	try {
		testGeneric(i, c1, c2, c3);
	}
	catch(int e)
	{
		return true;
	}
	return false;
}

/*
	If two circles are identical and one is non-intersecting
*/
bool testThree()
{
	int i = 3;
	Circle c1(origin,3);
	Circle c2(c1);
	Point c(-10,10);
	Circle c3(c, 4);
	try {
		testGeneric(i, c1, c2, c3);
	}
	catch(int e)
	{
		return true;
	}
	return false;
}

/*
	If two circles are identical and third is intersecting
*/
bool testFour()
{
	int i = 4;
	Circle c1(origin,3);
	Circle c2(c1);
	Point c(-3,4);
	Circle c3(c, 6);
	try {
		testGeneric(i, c1, c2, c3);
	}
	catch(int e)
	{
		return true;
	}
	return false;
}

/*
	If two circles are intersecting and third is not
*/
bool testFive()
{
	int i = 5;
	Circle c1(origin,4);
	Point b(7,0);
	Circle c2(b, 4);
	Point c(-10,10);
	Circle c3(c, 2);
	try {
		testGeneric(i, c1, c2, c3);
	}
	catch(int e)
	{
		return true;
	}
	return false;
}

/*
	If two circles are intersecting and third intersects at one point
*/
bool testSix()
{
	int i = 6;
	Circle c1(origin,5);
	Point b(6,0);
	Circle c2(b, 5);
	Point c(3,10);
	Circle c3(c, 6);
	try {
		testGeneric(i, c1, c2, c3);
	}
	catch(int e)
	{
		return false;
	}
	return true;
}

/*
	If all three circles intersect in a region.
*/
bool testSeven()
{
	int i = 7;
	Circle c1(origin, 5);
	Point b(6, 0);
	Circle c2(b, 5);
	Point c(3, -8);
	Circle c3(c, 5);
	try {
		testGeneric(i, c1, c2, c3);
	}
	catch(int e)
	{
		return false;
	}
	return true;
}

/*
	If all three circles intersect in a region, not aligned to x axis anymore.
*/
bool testEight()
{
	int i = 8;
	Point a(2, 3);
	Circle c1(origin, 5);
	Point b(3,4);
	Circle c2(b, 5);
	Point c(0, 2);
	Circle c3(c, 6);
	try {
		testGeneric(i, c1, c2, c3);
	}
	catch(int e)
	{
		return false;
	}
	return true;
}

/*
	Run all tests
*/
void testTrilateration()
{
	testOne();
	testTwo();
	testThree();
	testFour();
	testFive();
	testSix();
	testSeven();
	testEight();
}
