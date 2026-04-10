#include "render.h"
#include "calc.h"

void meshPointsCalc(SDL_Renderer* mren, Mesh& mesh, rData rd) {
	for (Point3D pt : mesh.verts) {
		Point3D tempt = meshRotateCalc(pt, rd);
		Point2D project2D = proj3Dto2D(tempt, rd.zoom);
		Point2D screenPoint2D = screenMap(project2D, rd.scale, rd.w, rd.h);
		SDL_RenderPoint(mren, screenPoint2D.x, screenPoint2D.y);
	}
}

void meshEdgesCalc(SDL_Renderer* mren, Mesh& mesh, rData rd) {
	for (std::pair<int, int> pt : mesh.edges) {
		int i = pt.first;
		int j = pt.second;
		Point3D p1 = meshRotateCalc(mesh.verts[i], rd);
		Point3D p2 = meshRotateCalc(mesh.verts[j], rd);
		Point2D project2D1 = proj3Dto2D(p1, rd.zoom);
		Point2D project2D2 = proj3Dto2D(p2, rd.zoom);
		Point2D screenPoint2D1 = screenMap(project2D1, rd.scale, rd.w, rd.h);
		Point2D screenPoint2D2 = screenMap(project2D2, rd.scale, rd.w, rd.h);
		SDL_RenderLine(mren, screenPoint2D1.x, screenPoint2D1.y, screenPoint2D2.x, screenPoint2D2.y);
	}
}

Point3D meshRotateCalc(const Point3D& pt, rData rd) {
	Point3D tempPt;
	tempPt = rotatePointX(pt, rd.rotX);
	tempPt = rotatePointY(tempPt, rd.rotY);
	tempPt = rotatePointZ(tempPt, rd.rotZ);
	return tempPt;
}