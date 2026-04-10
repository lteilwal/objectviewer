#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <point.h>

void loadFile(std::string fileName, std::vector<Point3D>& verts, std::vector<std::pair<int, int>>& edges);

void getMesh(std::string buffer, std::vector<Point3D>& verts, std::vector<std::pair<int, int>>& edges);