package edu.networks.triangulator;

public class Util {

	public static double distance(Point a, Point b) {
		double x = Math.pow(a.getX()-b.getX(), 2);
		double y = Math.pow(a.getY()-b.getY(), 2);
		return Math.sqrt(x+y);
	}
	
}
