/* 
 * File: trilateration.c
 * AUthor: doug@neverfear.org
 *
 * Implements an approach to trilateration
 * 
 */




#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define PLUS_MINUS '�'
#define PI 3.1415926535897932384626433832795

struct coordinate {
	double x;
	double y;
	double z;
	double r;
};

struct transformation {
	double x;
	double y;
	double z;
	double alpha;
	char zyswapped;
};

#define TRUE  1
#define FALSE 0

#define DEG2RADs(x) ((PI / 180.0) * x)
#define RAD2DEGs(x) ((180.0 / PI) * x)

#define SQR(x) (x * x)

#define PRECISION_F "%.3f"

unsigned int last_random = 1;


/* Generate a random number 0 -> highest */
unsigned int GenerateRandom(unsigned int highest) {
	srand(time(NULL) + last_random);
	last_random = rand() % highest;
	return last_random;
}


/* Generate 3 points in the vector space around target for testing purposes */
void GeneratePoints(struct coordinate target, struct coordinate * terms) {
	double alpha;
	int i;
	
	for(i = 0; i < 3; i++) {
		alpha = DEG2RADs(GenerateRandom(360));
		
		terms[i].x = target.x + (target.r * cos(alpha));
		terms[i].y = target.y + (target.r * sin(alpha));
		terms[i].z = target.z;
		terms[i].r = target.r;
		
		/*
		printf("Generated Term %d {\n", i);
		printf("  x = " PRECISION_F "\n", terms[i].x);
		printf("  y = " PRECISION_F "\n", terms[i].y);
		printf("  z = " PRECISION_F "\n", terms[i].z);
		printf("}\n");
		*/
	}
	
}

/* Perform the trilateration for the given 3 terms */
struct coordinate Trilaterate(struct coordinate * terms) {
	
	struct coordinate result;
	double i = terms[2].x - terms[0].x;
	double j = terms[2].y - terms[0].y;
	double d = terms[1].x;
	
	result.x = (SQR(terms[0].r) - SQR(terms[1].r) + SQR(d)) / (2 * d);
	
	result.y = ( (SQR(terms[0].r) - SQR(terms[2].r) + SQR(i) + SQR(j) ) / (2 * j) ) - ((i / j) * result.x);
	
	result.z = sqrt(abs(SQR(terms[0].r) - SQR(result.x) - SQR(result.y)));
	
	return result;
}


/*
   Modify our coordinate system and apply the new system to the given terms.
   The modifications leave the following state to be true:
   terms[0].x == 0 && terms[0].y == 0 && terms[0].z == 0
   terms[1].y == 0
   
   Returns information about the alterations used to restore the coordinate system later.
*/
struct transformation CoordinateTransformation(struct coordinate * terms) {
	int i;
	double temp;
	struct transformation map;
	map.x = -terms[0].x;
	map.y = -terms[0].y;
	map.z = -terms[0].z;
	
	// Center coordinate system
	for(i = 0; i < 3; i++) {
		terms[i].x += map.x;
		terms[i].y += map.y;
		terms[i].z += map.z;
	}
	
	// Rotate coordinate system such that terms[1].y == 0
	double a; // width
	double b; // height
	double c; // hypotenuse
	
	a = (terms[1].x);
	b = (terms[1].y);
	c = sqrt(SQR(a) + SQR(b));
	
	map.alpha = acos(((SQR(a) + SQR(c)) - SQR(b)) / (2.0 * a * c));
	
	if ((a > 0 && b > 0) || (a < 0 && b > 0)) {
		map.alpha *= -1;
	}
	
	for(i = 0; i < 3; i++) {
		// Rotate x and y
		double x, y;
		x = terms[i].x * cos(map.alpha) - terms[i].y * sin(map.alpha);
		y = terms[i].x * sin(map.alpha) + terms[i].y * cos(map.alpha);
		terms[i].x = x;
		terms[i].y = y;
		
		/*
		printf("Rotated Term %d {\n", i);
		printf("  x = " PRECISION_F "\n", terms[i].x);
		printf("  y = " PRECISION_F "\n", terms[i].y);
		printf("  z = " PRECISION_F "\n", terms[i].z);
		printf("}\n");
		*/
	}
	
	
	if (terms[2].y - terms[0].y == 0) { // TODO: What is this for? - I forgot! Whoops
		// Swap z with y
		for(i = 0; i < 3; i++) {
			temp = terms[i].y;
			terms[i].y = terms[i].z;
			terms[i].z = temp;
		}
		map.zyswapped = TRUE;
		
	} else {
		map.zyswapped = FALSE;
	}
	
	return map;
	
}

/* Reverse and restore the coordinate system for the given coordinate */
struct coordinate InverseCoordinateTransformation(struct coordinate coord, struct transformation map) {
	double temp;
	
	if (map.zyswapped) { // TODO: What is this for? - I forgot! Whoops
		temp = coord.y;
		coord.y = coord.z;
		coord.z = temp;
	}
	
	double x = coord.x * cos(-map.alpha) - coord.y * sin(-map.alpha);
	double y = coord.x * sin(-map.alpha) + coord.y * cos(-map.alpha);
	
	coord.x = x;
	coord.y = y;
	
	coord.x -= map.x;
	coord.y -= map.y;
	coord.z -= map.z;
	
	
	return coord;
}


/* Executable entry label */
int main(int argc, char ** argv) {
	
	unsigned long distance;
	struct coordinate terms[3];
	struct coordinate result;
	struct coordinate target;
	struct transformation restore;
	int i;
	
	// Here you specify the distance the target is from all points
	distance = 50;
	
	// Here you specify the target
	target.x = 123.0;
	target.y = 567.0;
	target.z = -10.0;
	target.r = distance;
	
	for(i = 1; i <= 20; i++) {
		printf("\n** TEST %d ** \n", i);
		
		// Generate a triplet of test points
		GeneratePoints(target, terms);
		
		// Alter the coordinate system to center terms[0] and rotate the plane to ensure terms[1].y == 0
		restore = CoordinateTransformation(terms);
		
		// Trilaterate the 3 points and get the intersection
		result = Trilaterate(terms);
		
		// Restore the previous coordinate system
		result = InverseCoordinateTransformation(result, restore);
		
		// Dance or something
		printf("Result = (" PRECISION_F ", " PRECISION_F ", %c " PRECISION_F ")\n", result.x, result.y, PLUS_MINUS, result.z);
		
	}
	terms[0].x = 0;
	terms[0].y = 0;
	terms[0].z = 0;
	terms[0].r = 5;
	terms[1].x = 3;
	terms[1].y = 4;
	terms[1].z = 0;
	terms[1].r = 5;
	terms[2].x = 0;
	terms[2].y = 2;
	terms[2].z = 0;
	terms[2].r = 6;
	// Alter the coordinate system to center terms[0] and rotate the plane to ensure terms[1].y == 0
	restore = CoordinateTransformation(terms);
		
	// Trilaterate the 3 points and get the intersection
	result = Trilaterate(terms);
		
	// Restore the previous coordinate system
	result = InverseCoordinateTransformation(result, restore);
		
	// Dance or something
	printf("Result = (" PRECISION_F ", " PRECISION_F ", %c " PRECISION_F ")\n", result.x, result.y, PLUS_MINUS, result.z);
}

