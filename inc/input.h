/*
 * input.h - user input event processing declarations
 *
 * what this file does:
 * 1. declares mouse drag and scroll zoom event handlers
 * 2. declares keyboard event handlers for mode switching and benchmarking
 */

#pragma once
#include <SDL3/SDL.h>
#include "renderdata.h"

// process sdl input events for camera, compute mode, and benchmark
void inputEvent(const SDL_Event& event, bool& mrunning, rData& rd);

// adjust camera distance using mouse wheel
void scrollZoom(const SDL_Event& event, Camera& camera);
void scrollZoom(const SDL_Event& event, Camera& camera);