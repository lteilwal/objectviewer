#pragma once
#include "mesh.h"
#include <SDL3/SDL.h>
#include "renderdata.h"

class SDLApp {
public:
	bool mrunning = false;
	SDL_Window* mwin;
	SDL_Renderer* mren;
	SDL_Event event;
	Mesh mesh;
	rData renData;

	int launchWindow();
	void runTick();
	void Input();
	void Update();
	void Render();
};