#pragma once
#include <point.h>
#include <vector>

class Mesh{
public:
	int vertCount;
	int edgeCount;
	std::vector<Point3D> verts;
	std::vector<std::pair<int,int>> edges;

	Mesh();
};