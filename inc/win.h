/*
 * win.h - sdl application lifecycle and main window management
 *
 * what this file does:
 * 1. initializes sdl3 window and hardware renderer
 * 2. manages the core application loop (input, update, render)
 * 3. holds mesh, render context, and frame counter for profiling
 */

#pragma once
#include "mesh.h"
#include <SDL3/SDL.h>
#include "renderdata.h"
#include "calc.h"

// main sdl application controller
class SDLApp {
public:
	bool mrunning = false;
	SDL_Window* mwin;
	SDL_Renderer* mren;
	SDL_Event event;
	Mesh mesh;
	rData renData;

	// frame counter for periodic profiler output
	int frameCount = 0;

	// launch application window and start main loop
	int launchWindow();
	void runTick();
	void Input();
	void Update();
	void Render();
};