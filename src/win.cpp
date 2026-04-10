#include "win.h"
#include "mesh.h"
#include "input.h"
#include "fileprocessor.h"
#include "calc.h"
#include "render.h"
#include <SDL3/SDL.h>

int SDLApp::launchWindow(){
	
	if(!SDL_Init(SDL_INIT_VIDEO)){
		SDL_Log("Error creating window");
	}
	mwin = SDL_CreateWindow("MeshViewer", 1280, 720, SDL_WINDOW_RESIZABLE);

	renData.rotX = 0;
	renData.rotY = 0;
	renData.rotZ = 0;
	renData.zoom = 5;
	renData.scale = 200;

	SDL_GetWindowSize(mwin, &renData.w, &renData.h);

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
		inputEvent(event, mrunning, renData.zoom);
	}
}

void SDLApp::Update() {
	renData.rotZ = 180;
	renData.rotY += 0.05f;
}

void SDLApp::Render() {
	SDL_SetRenderDrawColor(mren, 30, 30, 30, 255);
	SDL_RenderClear(mren);

	SDL_SetRenderDrawColor(mren, 255, 50, 50, 255);
	meshPointsCalc(mren, mesh, renData);
	meshEdgesCalc(mren, mesh, renData);

	SDL_RenderPresent(mren);
}