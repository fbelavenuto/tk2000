/*
 */

#include <cstdlib>
#include <memory>
#include <SDL2/SDL.h>
#include "Machine.h"

/* Defines */


/* Variables */
auto theMachine = std::make_unique<CMachine>();


/*****************************************************************************/
int main(int argc, char* args[]) {

	// Initialize log
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	//Set texture filtering to linear
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0")) {	// 0 = Nearest pixel sampling
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Warning: SDL_HINT_RENDER_SCALE_QUALITY error!");
	}

	theMachine->init();
	theMachine->loop();

	return 0;
}
