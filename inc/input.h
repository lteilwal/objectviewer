#pragma once
#include <SDL3/SDL.h>

void inputEvent(SDL_Event event, bool& mrunning, int& zoom);

void scrollZoom(SDL_Event event, int& zoom);