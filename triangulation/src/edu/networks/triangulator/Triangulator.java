package edu.networks.triangulator;

public class Triangulator {

	
	//4 May 2012: assumes an intersection exists!
	//4 May 2012: assumes a transformation has been done to get these in correct
	//format (see wikipedia page)
	public Point triangulate(Circle a, Circle b, Circle c) {
		//according to the wikipedia article:
		//https://en.wikipedia.org/wiki/Trilateration
		double aRad = a.getRadius();
		double bRad = b.getRadius();
		double cRad = c.getRadius();
		
		Point aCen = a.getCenter();
		Point bCen = b.getCenter();
		Point cCen = c.getCenter();
		
		double d = Math.abs(aCen.getX() - bCen.getX());
		double i = Math.abs(aCen.getX() - cCen.getX());
		double j = Math.abs(aCen.getY() - cCen.getY());
		
		double newX = (Math.pow(aRad, 2) - Math.pow(bRad, 2) + Math.pow(d, 2))
				/ (2 * d);
		double newY = (Math.pow(aRad, 2) - Math.pow(cRad, 2) + Math.pow(i, 2)
				+ Math.pow(j, 2)) / (2 * j) - i * newX / j;
		
		return new Point(newX, newY);
	}
}
