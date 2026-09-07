/*
 * win.cpp - application lifecycle, main loop timing, and profiling dispatch
 *
 * what this file does:
 * 1. initializes sdl3 window and hardware accelerated renderer
 * 2. executes main loop measuring frame time, fps, and stage timings
 * 3. renders on-screen mode indicator and profiler overlay
 * 4. handles continuous model rotation toggle in update loop
 * 5. dispatches benchmark suite execution when triggered by user
 */

#include "win.h"
#include "mesh.h"
#include "input.h"
#include "fileprocessor.h"
#include "calc.h"
#include "render.h"
#include <SDL3/SDL.h>
#include <chrono>
#include <iostream>
#include <iomanip>

// initialize sdl, window, renderer, and load mesh file
int SDLApp::launchWindow() {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Error creating window");
	}
	mwin = SDL_CreateWindow("MeshViewer", 1280, 720, SDL_WINDOW_RESIZABLE);

	SDL_GetWindowSize(mwin, &renData.w, &renData.h);

	mren = SDL_CreateRenderer(mwin, "opengl");
	if (mren == nullptr) {
		SDL_Log("Error creating renderer");
	}

	std::string fileName = "mesh.obj";
	loadFile(fileName, mesh.verts, mesh.edges);

	// display startup banner and control instructions in console
	std::cout << "========================================================================\n";
	std::cout << "                              MESHVIEWER                                \n";
	std::cout << "========================================================================\n";
	std::cout << "mesh loaded: " << mesh.verts.size() << " vertices, " << mesh.edges.size() << " edges\n";
	std::cout << "controls:\n";
	std::cout << "  [left click + drag] : orbit camera (yaw and pitch)\n";
	std::cout << "  [mouse wheel]       : zoom camera in and out\n";
	std::cout << "  [key 'R']           : toggle continuous model rotation (ON/OFF)\n";
	std::cout << "  [key 'M']           : toggle compute mode (CPU <-> CUDA)\n";
	std::cout << "  [key 'P']           : toggle on-screen performance overlay\n";
	std::cout << "  [key 'B']           : run cpu vs cuda benchmark suite (console)\n";
	std::cout << "========================================================================\n\n";

	runTick();

	return 0;
}

// main application loop with frame timing and profiler dispatch
void SDLApp::runTick() {
	mrunning = true;
	while (mrunning) {
		// record start of frame timestamp
		auto frameStart = std::chrono::high_resolution_clock::now();

		Input();
		Update();
		Render();

		// record end of frame timestamp and calculate frame time
		auto frameEnd = std::chrono::high_resolution_clock::now();
		renData.frameTimeMs = std::chrono::duration<float, std::milli>(frameEnd - frameStart).count();
		if (renData.frameTimeMs > 0.0f) {
			renData.fps = 1000.0f / renData.frameTimeMs;
		}
		frameCount++;

		// execute benchmark suite if triggered from input
		if (renData.runBenchmarkRequested) {
			runBenchmarkSuite(mesh, renData, 50);
			renData.runBenchmarkRequested = false;
		}
	}
}

// poll and process input events
void SDLApp::Input() {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_EVENT_WINDOW_RESIZED) {
			SDL_GetWindowSize(mwin, &renData.w, &renData.h);
		}
		inputEvent(event, mrunning, renData);
	}
}

// update object transformation
void SDLApp::Update() {
	auto updateStart = std::chrono::high_resolution_clock::now();

	// rotate object continuously when rotation is enabled
	if (renData.continuousRotation) {
		float dt = (renData.frameTimeMs > 0.0f) ? (renData.frameTimeMs / 1000.0f) : (1.0f / 60.0f);
		renData.modelTransform.rotation.y += 30.0f * dt;
	}

	auto updateEnd = std::chrono::high_resolution_clock::now();
	renData.updateTimeMs = std::chrono::duration<float, std::milli>(updateEnd - updateStart).count();
}

// render scene, on-screen overlay, and measure stage timings
void SDLApp::Render() {
	// clear screen background
	SDL_SetRenderDrawColor(mren, 30, 30, 30, 255);
	SDL_RenderClear(mren);

	// transform mesh vertices using active compute mode
	std::vector<Point2D> screenPoints;
	transformMeshVertices(mesh, screenPoints, renData);

	// measure draw time for wireframe points and lines
	auto drawStart = std::chrono::high_resolution_clock::now();
	SDL_SetRenderDrawColor(mren, 255, 50, 50, 255);
	renderMeshPoints(mren, screenPoints);
	renderMeshEdges(mren, mesh, screenPoints);
	auto drawEnd = std::chrono::high_resolution_clock::now();
	renData.drawTimeMs = std::chrono::duration<float, std::milli>(drawEnd - drawStart).count();

	// render minimal on-screen mode and performance overlay
	renderOverlay(mren, renData);

	// measure present time
	auto presentStart = std::chrono::high_resolution_clock::now();
	SDL_RenderPresent(mren);
	auto presentEnd = std::chrono::high_resolution_clock::now();
	renData.presentTimeMs = std::chrono::duration<float, std::milli>(presentEnd - presentStart).count();
}