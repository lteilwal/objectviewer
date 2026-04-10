#define _USE_MATH_DEFINES
#include "calc.h"
#include "point.h"
#include <cmath>

Point2D proj3Dto2D(const Point3D& coord3D, int off) {
	Point2D proj2D = { coord3D.x / (coord3D.z + off),coord3D.y / (coord3D.z + off) };
	return proj2D;
}

Point2D screenMap(Point2D pt, int scale, int w, int h) {
	Point2D coord2D;
	coord2D.x = (pt.x * scale) + (w / 2);
	coord2D.y = (pt.y * scale) + (h / 2);
	return coord2D;
}

Point3D rotatePointX(Point3D p, float rotAngle) {
	float radian = rotAngle * ((float)M_PI / 180.0f);
	float pz = p.z * cos(radian) - p.y * sin(radian);
	float py = p.z * sin(radian) + p.y * cos(radian);
	Point3D rotatedPoint(p.x, py, pz);
	return rotatedPoint;
}

Point3D rotatePointY(Point3D p, float rotAngle) {
	float radian = rotAngle * ((float)M_PI / 180.0f);
	float px = p.x * cos(radian) + p.z * sin(radian);
	float pz = -p.x * sin(radian) + p.z * cos(radian);
	Point3D rotatedPoint(px, p.y, pz);
	return rotatedPoint;
}

Point3D rotatePointZ(Point3D p, float rotAngle) {
	float radian = rotAngle * ((float)M_PI / 180.0f);
	float px = p.x * cos(radian) - p.y * sin(radian);
	float py = p.x * sin(radian) + p.y * cos(radian);
	Point3D rotatedPoint(px, py, p.z);
	return rotatedPoint;
}