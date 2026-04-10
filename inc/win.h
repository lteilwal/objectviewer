#pragma once
#include "mesh.h"
#include <SDL3/SDL.h>

class SDLApp {
public:
	bool mrunning = false;
	SDL_Window* mwin;
	SDL_Renderer* mren;
	int w, h;
	SDL_Event event;
	Mesh mesh;
	float rotX = 0, rotY = 0, rotZ = 0;

	int launchWindow();
	void runTick();
	void Input();
	void Update();
	void Render();

	void meshPointsCalc(Mesh& mesh, int off, int scale);
	void meshEdgesCalc(Mesh& mesh, int off, int scale);
	Point3D meshRotateCalc(const Point3D& pt, int off, int scale);
};