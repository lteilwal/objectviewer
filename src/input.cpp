#include "input.h"
#include <SDL3/SDL.h>

void inputEvent(SDL_Event event, bool& mrunning) {
	//if (event.button.button == SDL_BUTTON_LEFT) {
	//	SDL_Log("0");
	//}
	//else if (event.quit == SDL_BUTTON_LEFT) {
	//	SDL_Log("0");
	//}
	switch (event.type) {
	case SDL_EVENT_QUIT:
		mrunning = false;
		break;
	}
}