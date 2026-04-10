#pragma once
#include "point.h"

Point2D proj3Dto2D(const Point3D& coord3D, int off);

Point2D screenMap(Point2D pt, int scale, int w, int h);

Point3D rotatePointX(Point3D p, float rotAngle);
Point3D rotatePointY(Point3D p, float rotAngle);
Point3D rotatePointZ(Point3D p, float rotAngle);