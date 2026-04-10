#include "input.h"
#include <SDL3/SDL.h>

void inputEvent(SDL_Event event, bool& mrunning, int& zoom) {
	switch (event.type) {
	case SDL_EVENT_QUIT:
		mrunning = false;
		break;
	case SDL_EVENT_MOUSE_WHEEL:
		scrollZoom(event, zoom);
		break;
	}
}

void scrollZoom(SDL_Event event, int& zoom) {
	if (event.wheel.y > 0) {
		zoom -= 1;
	}
	else {
		zoom += 1;
	}
}