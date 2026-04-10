#include "fileprocessor.h"
#include "point.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <SDL3/SDL.h>
#include <sstream>

void loadFile(std::string fileName, std::vector<Point3D>& verts, std::vector<std::pair<int, int>>& edges) {

	std::ifstream objfile(fileName);
	if (!objfile) {
		SDL_Log("Couldn't find mesh file");
	}

	std::string buffer;
	while (std::getline(objfile, buffer)) {
		getMesh(buffer, verts, edges);
	}
}

void getMesh(std::string buffer, std::vector<Point3D>& verts, std::vector<std::pair<int, int>>& edges) {
	size_t n = buffer.size();
	std::string type;
	std::stringstream Str(buffer);
	Str >> type;
	if (type=="v") {
		float x, y, z;
		Str >> x >> y >> z;
		verts.push_back(Point3D(x, y, z));
	}
	else if (type == "f") {
		std::string currentString;
		std::vector<int> intVertex;
		while (Str >> currentString) {
			std::string tempStr;
			for (auto c = currentString.begin(); *c != '/'; c++) {
				tempStr += *c;
			}
			int tempInt = stoi(tempStr);
			intVertex.push_back(tempInt - 1);
		}
		for (int i = 0; i < intVertex.size(); i++) {
			int j = (i + 1) % intVertex.size();
			edges.push_back(std::pair<int, int>(intVertex[i], intVertex[j]));
		}
	}
}