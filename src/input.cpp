/*
 * input.cpp - input event handling, camera controls, and keyboard triggers
 *
 * what this file does:
 * 1. handles mouse drag to orbit camera with natural horizontal and vertical controls
 * 2. handles mouse wheel to zoom camera distance
 * 3. handles 'B' key to run cpu vs cuda benchmark suite
 * 4. handles 'M' key to toggle between cpu and cuda rendering mode
 * 5. handles 'P' key to toggle on-screen performance overlay
 * 6. handles 'R' key to toggle continuous model rotation
 */

#include "input.h"
#include <SDL3/SDL.h>

// handle user input events from sdl event queue
void inputEvent(const SDL_Event& event, bool& mrunning, rData& rd) {
	switch (event.type) {
	case SDL_EVENT_QUIT:
		mrunning = false;
		break;

	case SDL_EVENT_MOUSE_MOTION:
		// orbit camera when left mouse button is held down
		if (event.motion.state & SDL_BUTTON_LMASK) {
			// invert horizontal and vertical movement for natural orbit rotation
			rd.camera.yaw -= event.motion.xrel * 0.4f;
			rd.camera.pitch -= event.motion.yrel * 0.4f;

			// clamp vertical orbit angle to avoid upside-down flip
			if (rd.camera.pitch > 89.0f) rd.camera.pitch = 89.0f;
			if (rd.camera.pitch < -89.0f) rd.camera.pitch = -89.0f;
		}
		break;

	case SDL_EVENT_MOUSE_WHEEL:
		// zoom camera distance on mouse scroll
		scrollZoom(event, rd.camera);
		break;

	case SDL_EVENT_KEY_DOWN:
		if (event.key.key == SDLK_B) {
			// trigger cpu vs cuda benchmark suite
			rd.runBenchmarkRequested = true;
		}
		else if (event.key.key == SDLK_M) {
			// toggle compute mode between cpu and cuda
			rd.mode = (rd.mode == computeMode::CPU) ? computeMode::GPU : computeMode::CPU;
		}
		else if (event.key.key == SDLK_P) {
			// toggle on-screen performance overlay
			rd.showProfiling = !rd.showProfiling;
		}
		else if (event.key.key == SDLK_R) {
			// toggle continuous model rotation on or off
			rd.continuousRotation = !rd.continuousRotation;
		}
		break;
	}
}

// adjust camera distance based on mouse wheel scroll direction
void scrollZoom(const SDL_Event& event, Camera& camera) {
	if (event.wheel.y > 0) {
		camera.distance -= 1.5f;
	}
	else if (event.wheel.y < 0) {
		camera.distance += 1.5f;
	}

	// prevent camera from moving behind target
	if (camera.distance < 1.0f) {
		camera.distance = 1.0f;
	}
}