#pragma once
#include <point.h>
#include <vector>

class Mesh{
public:

	std::vector<Point3D> verts;
	std::vector<std::pair<int,int>> edges;

	Mesh();
};