package edu.networks.triangulator;
/**
	Representation class of a point in 2D space. Does not take into account Earth
	curvature or sphere points, made only for R2.
*/
public class Point implements Cloneable {
	private double x;
	private double y;

	public double getX() {
		return x;
	}

	public double getY() {
		return y;
	}

	public void setX(double x) {
		this.x = x;
	}

	public void setY(double y) {
		this.y = y;
	}

	public Point(double x, double y) {
		this.setX(x);
		this.setY(y);
	}

	public Point() {
		this(0,0);
	}
	
	public Object clone() {
		return new Point(this.getX(), this.getY());
	}
}
