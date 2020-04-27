// Copyright (c) 2020 FBLabs
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include <cstdlib>
#include <memory>
#include <SDL2/SDL.h>
#include "Machine.h"

/* Defines */


/* Variables */
CMachine *theMachine{};


/*****************************************************************************/
int main(int argc, char* argv[]) {

	// Initialize log
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN);

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	//Set texture filtering to linear
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0")) {	// 0 = Nearest pixel sampling
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Warning: SDL_HINT_RENDER_SCALE_QUALITY error!");
	}

	theMachine = new CMachine;
	if (argc == 2) {
		if (!theMachine->setTapeFile(argv[1])) {
			printf("Error inserting %s tape file\n", argv[1]);
		}
	}
	theMachine->loop();
	delete theMachine;
	return 0;
}
