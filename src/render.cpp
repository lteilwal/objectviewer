#include "render.h"
#include "kernelCalc.h"

void meshPointsCalc(SDL_Renderer* mren, Mesh& mesh, rData rd) {
	std::vector<Point2D> screenPoint2D;
	kernelLaunchCalc(mesh, screenPoint2D, rd);
	meshPointsRender(mren, screenPoint2D);
}

void meshPointsRender(SDL_Renderer* mren, const std::vector<Point2D>& screenPoint2D) {
	SDL_RenderPoints(mren, reinterpret_cast<const SDL_FPoint*>(screenPoint2D.data()), (int)screenPoint2D.size());
}

void meshEdgesCalc(SDL_Renderer* mren, Mesh& mesh, rData rd) {
	std::vector<Point2D> screenPoint2D;
	kernelLaunchCalc(mesh, screenPoint2D, rd);

	int n = (int)mesh.edges.size();
	for (int i = 0; i < n; i++) {
		meshEdgesRender(mren, screenPoint2D[mesh.edges[i].first], screenPoint2D[mesh.edges[i].second]);
	}
}

void meshEdgesRender(SDL_Renderer* mren, Point2D screenPoint2D1, Point2D screenPoint2D2) {
	SDL_RenderLine(mren, screenPoint2D1.x, screenPoint2D1.y, screenPoint2D2.x, screenPoint2D2.y);
}