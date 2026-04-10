#include "win.h"
#include "mesh.h"
#include "input.h"
#include "fileprocessor.h"
#include "calc.h"
#include <SDL3/SDL.h>

int SDLApp::launchWindow(){
	
	if(!SDL_Init(SDL_INIT_VIDEO)){
		SDL_Log("Error creating window");
	}
	mwin = SDL_CreateWindow("MeshViewer", 1280, 720, SDL_WINDOW_RESIZABLE);
	SDL_GetWindowSize(mwin, &w, &h);

	mren = SDL_CreateRenderer(mwin, "opengl");
	if (mren == nullptr) {
		SDL_Log("Error creating renderer");
	}

	std::string fileName = "mesh.obj";
	loadFile(fileName, mesh.verts, mesh.edges);

	runTick();

	return 0;
}
 
void SDLApp::runTick() {
	mrunning = true;
	while (mrunning) {
		Input();
		Update();
		Render();
	}
}

void SDLApp::Input() {
	while (SDL_PollEvent(&event)) {
		inputEvent(event, mrunning);
	}
}

void SDLApp::Update() {
	rotY += 0.1f;
}

void SDLApp::Render() {
	SDL_SetRenderDrawColor(mren, 30, 30, 30, 255);
	SDL_RenderClear(mren);

	SDL_SetRenderDrawColor(mren, 255, 50, 50, 255);
	meshPointsCalc(mesh, 5, 1000);
	meshEdgesCalc(mesh, 5, 1000);

	SDL_RenderPresent(mren);
}

void SDLApp::meshPointsCalc(Mesh& mesh, int off, int scale) {
	for (Point3D pt:mesh.verts) {
		Point3D tempt = meshRotateCalc(pt, off, scale);
		Point2D project2D = proj3Dto2D(tempt, off);
		Point2D screenPoint2D = screenMap(project2D, scale, w, h);
		SDL_RenderPoint(mren, screenPoint2D.x, screenPoint2D.y);
	}
}

void SDLApp::meshEdgesCalc(Mesh& mesh, int off, int scale) {
	for (std::pair<int,int> pt : mesh.edges) {
		int i = pt.first;
		int j = pt.second;
		Point3D p1 = meshRotateCalc(mesh.verts[i], off, scale);
		Point3D p2 = meshRotateCalc(mesh.verts[j], off, scale);
		Point2D project2D1 = proj3Dto2D(p1, off);
		Point2D project2D2 = proj3Dto2D(p2, off);
		Point2D screenPoint2D1 = screenMap(project2D1, scale, w, h);
		Point2D screenPoint2D2 = screenMap(project2D2, scale, w, h);
		SDL_RenderLine(mren, screenPoint2D1.x, screenPoint2D1.y, screenPoint2D2.x, screenPoint2D2.y);
	}
}

Point3D SDLApp::meshRotateCalc(const Point3D& pt, int off, int scale) {
	Point3D tempPt;
	tempPt = rotatePointX(pt, rotX);
	tempPt = rotatePointY(tempPt, rotY);
	tempPt = rotatePointZ(tempPt, rotZ);
	return tempPt;
}