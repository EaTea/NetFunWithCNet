package edu.networks.triangulator;

public class Circle {
	private Point center;
	private double radius;
	
	public double getRadius() {
		return radius;
	}
	public void setRadius(double radius) {
		this.radius = radius;
	}
	
	public Point getCenter() {
		return center;
	}
	public void setCenter(Point center) {
		//TODO: Should this be typechecked? Not a real big issue is it?
		this.center = (Point)center.clone();
	}
	
	public Circle(Point center, double radius) {
		this.setCenter(center);
		this.setRadius(radius);
	}
	
	public Circle(Point center, Point onCircumference) {
		this.setCenter(center);
		this.setRadius(Util.distance(center, onCircumference));
	}
}
