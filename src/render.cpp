/*
 * render.cpp - rendering stages, compute dispatching, and draw timing
 *
 * what this file does:
 * 1. calculates model view projection matrix
 * 2. dispatches vertex transformation to cpu or cuda based on rData::mode
 * 3. records timing breakdown for vertex transformation stage
 * 4. draws transformed points and lines using sdl3 hardware renderer
 * 5. renders minimal on-screen mode indicator and performance overlay
 */

#include "render.h"
#include "calc.h"
#include "kernelCalc.h"
#include <chrono>

// transform all mesh vertices to screen coordinates using cpu or cuda
void transformMeshVertices(const Mesh& mesh, std::vector<Point2D>& screenPoints, rData& rd) {
	// compute combined model view projection matrix
	Mat4 model = rd.modelTransform.getModelMatrix();
	Mat4 view = rd.camera.getViewMatrix();
	float aspect = (float)rd.w / (float)rd.h;
	Mat4 proj = Mat4::makePerspective(60.0f, aspect, 0.1f, 500.0f);
	Mat4 mvp = Mat4::multiply(proj, Mat4::multiply(view, model));

	if (rd.mode == computeMode::CPU) {
		// execute cpu vertex transformation and record time
		rd.transformTimeMs = transformVerticesCPU(mesh.verts, screenPoints, mvp, rd.w, rd.h);
		rd.gpuH2DMs = 0.0f;
		rd.gpuKernelMs = 0.0f;
		rd.gpuD2HMs = 0.0f;
		rd.gpuTotalMs = 0.0f;
	}
	else {
		// execute cuda gpu vertex transformation and record breakdown timings
		GpuTimings timings;
		kernelLaunchCalc(mesh.verts, screenPoints, mvp, rd.w, rd.h, &timings);
		rd.gpuH2DMs = timings.h2dMs;
		rd.gpuKernelMs = timings.kernelMs;
		rd.gpuD2HMs = timings.d2hMs;
		rd.gpuTotalMs = timings.totalMs;
		rd.transformTimeMs = timings.totalMs;
	}
}

// render transformed points to sdl renderer
void renderMeshPoints(SDL_Renderer* mren, const std::vector<Point2D>& screenPoints) {
	for (const Point2D& pt : screenPoints) {
		if (pt.x > -9000.0f) {
			SDL_RenderPoint(mren, pt.x, pt.y);
		}
	}
}

// render wireframe edges connecting transformed points
void renderMeshEdges(SDL_Renderer* mren, const Mesh& mesh, const std::vector<Point2D>& screenPoints) {
	for (const std::pair<int, int>& edge : mesh.edges) {
		int i = edge.first;
		int j = edge.second;

		if (i < (int)screenPoints.size() && j < (int)screenPoints.size()) {
			const Point2D& p1 = screenPoints[i];
			const Point2D& p2 = screenPoints[j];

			// render line if both vertices are in front of camera
			if (p1.x > -9000.0f && p2.x > -9000.0f) {
				SDL_RenderLine(mren, p1.x, p1.y, p2.x, p2.y);
			}
		}
	}
}

// render minimal on-screen mode indicator and performance overlay
void renderOverlay(SDL_Renderer* mren, const rData& rd) {
	// always display current active compute mode
	SDL_SetRenderDrawColor(mren, 255, 255, 255, 255);
	if (rd.mode == computeMode::CPU) {
		SDL_RenderDebugText(mren, 12.0f, 12.0f, "Mode: CPU");
	}
	else {
		SDL_RenderDebugText(mren, 12.0f, 12.0f, "Mode: CUDA");
	}

	// display performance profiler overlay when enabled
	if (rd.showProfiling) {
		SDL_SetRenderDrawColor(mren, 180, 230, 255, 255);
		SDL_RenderDebugTextFormat(mren, 12.0f, 28.0f, "FPS: %.1f", rd.fps);
		SDL_RenderDebugTextFormat(mren, 12.0f, 44.0f, "Frame Time: %.2f ms", rd.frameTimeMs);
		SDL_RenderDebugTextFormat(mren, 12.0f, 60.0f, "Transform: %.2f ms", rd.transformTimeMs);
		SDL_RenderDebugTextFormat(mren, 12.0f, 76.0f, "Draw: %.2f ms", rd.drawTimeMs);
	}
}

// backward compatibility helper for points rendering
void meshPointsCalc(SDL_Renderer* mren, Mesh& mesh, rData& rd) {
	std::vector<Point2D> pts;
	transformMeshVertices(mesh, pts, rd);
	renderMeshPoints(mren, pts);
}

// backward compatibility helper for edges rendering
void meshEdgesCalc(SDL_Renderer* mren, Mesh& mesh, rData& rd) {
	std::vector<Point2D> pts;
	transformMeshVertices(mesh, pts, rd);
	renderMeshEdges(mren, mesh, pts);
}