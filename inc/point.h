#pragma once

struct Point3D {
	float x, y, z;
	Point3D(float px, float py, float pz) {
		x = px;
		y = py;
		z = pz;
	}
	Point3D() {
		x = 0;
		y = 0;
		z = 0;
	}
};

struct Point2D {
	float x, y;
	Point2D(float px, float py) {
		x = px;
		y = py;
	}
	Point2D() {
		x = 0;
		y = 0;
	}
};