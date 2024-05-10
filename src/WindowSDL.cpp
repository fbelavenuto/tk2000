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

#include "pch.h"
#include "WindowSDL.h"
#include "Video.h"
#include "media.h"


/*****************************************************************************/
void CWindowSDL::setScanline(bool val) noexcept {
	mScanLines = val;
}

/*****************************************************************************/
void CWindowSDL::setFullScreen(bool val) noexcept {
	mFullScreen = val;
	if (mFullScreen) {
		SDL_DisplayMode dm;
		SDL_GetDesktopDisplayMode(0, &dm);
		SDL_SetWindowSize(mWindow, dm.w, dm.h);
		SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN);
	} else {
		SDL_SetWindowFullscreen(mWindow, 0);
		SDL_SetWindowSize(mWindow, VIDEOWIDTH * 2, VIDEOHEIGHT * 2);
	}
}

/*****************************************************************************/
CWindowSDL::CWindowSDL(CVideo& video) :
	mVideo(video)
{
	mWindow = SDL_CreateWindow("TK2000 Emulator", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, VIDEOWIDTH * 2, VIDEOHEIGHT * 2, SDL_WINDOW_SHOWN);

	if (!mWindow) {
		std::string error{ "Window could not be created! SDL Error: " };
		error.append(SDL_GetError());
		throw std::runtime_error(error);
	}
	SDL_Surface* sIcon{};
	sIcon = SDL_LoadBMP_RW(SDL_RWFromConstMem(iconBMP, sizeof(iconBMP)), true);
	if (sIcon) {
		SDL_SetColorKey(sIcon, SDL_TRUE, SDL_MapRGB(sIcon->format, 255, 0, 255));
		SDL_SetWindowIcon(mWindow, sIcon);
		SDL_FreeSurface(sIcon);
	}

	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!mRenderer) {
		std::string error{ "Error creating renderer! SDL Error: " };
		error.append(SDL_GetError());
		throw std::runtime_error(error);
	}
	mScreen = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING,
		VIDEOWIDTH, VIDEOHEIGHT * 2);
	if (!mScreen) {
		std::string error{ "Error creating screen texture! SDL Error: " };
		error.append(SDL_GetError());
		throw std::runtime_error(error);
	}
	SDL_Surface* surface = SDL_LoadBMP_RW(SDL_RWFromConstMem(cpuBMP, sizeof(cpuBMP)), true);
	SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 0, 0));
	mCpuIcon = SDL_CreateTextureFromSurface(mRenderer, surface);
	SDL_FreeSurface(surface);
	surface = SDL_LoadBMP_RW(SDL_RWFromConstMem(tapeBMP, sizeof(tapeBMP)), true);
	SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 0, 0));
	mTapeIcon = SDL_CreateTextureFromSurface(mRenderer, surface);
	SDL_FreeSurface(surface);
}

/*****************************************************************************/
CWindowSDL::~CWindowSDL() {
	SDL_DestroyTexture(mCpuIcon);
	SDL_DestroyTexture(mTapeIcon);
	SDL_DestroyTexture(mScreen);
	SDL_DestroyRenderer(mRenderer);		//Free resources and close SDL
	SDL_DestroyWindow(mWindow);			//Destroy window
}

/*****************************************************************************/
void CWindowSDL::render(bool cpuState, bool tapeState) {
	SDL_SetRenderTarget(mRenderer, nullptr);
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	char* fb = (char*)mVideo.getFrameBuffer();
	char* ptr;
	int pitch;
	SDL_LockTexture(mScreen, nullptr, (void **)&ptr, &pitch);
	memset(ptr, 0, VIDEOHEIGHT * 2 * pitch);
	for (int i = 0; i < VIDEOHEIGHT; i++) {
		memcpy(ptr, fb, pitch);
		ptr += pitch;
		if (!mScanLines) {
			memcpy(ptr, fb, pitch);
		}
		ptr += pitch;
		fb += pitch;
	}
	SDL_UnlockTexture(mScreen);
	if (mMenu) {
		SDL_SetTextureBlendMode(mScreen, SDL_BLENDMODE_BLEND);
		SDL_SetTextureColorMod(mScreen, 100, 100, 100);
	} else {
		SDL_SetTextureBlendMode(mScreen, SDL_BLENDMODE_NONE);
		SDL_SetTextureColorMod(mScreen, 255, 255, 255);
	}
	SDL_RenderCopy(mRenderer, mScreen, NULL, NULL);

	// Render states
	SDL_SetTextureBlendMode(mCpuIcon, SDL_BLENDMODE_BLEND);
	if (cpuState) {
		SDL_SetTextureColorMod(mCpuIcon, 0, 160, 0);
	} else {
		SDL_SetTextureColorMod(mCpuIcon, 160, 0, 0);
	}
	SDL_SetTextureAlphaMod(mCpuIcon, 128);

	SDL_SetTextureBlendMode(mTapeIcon, SDL_BLENDMODE_BLEND);
	if (tapeState) {
		SDL_SetTextureColorMod(mTapeIcon, 0, 160, 0);
	} else {
		SDL_SetTextureColorMod(mTapeIcon, 160, 0, 0);
	}
	SDL_SetTextureAlphaMod(mTapeIcon, 128);

	SDL_Rect r{VIDEOWIDTH * 2 - 32, VIDEOHEIGHT * 2 - 16, 16, 16};
	SDL_RenderCopy(mRenderer, mCpuIcon, nullptr, &r);
	r.x += 16;
	SDL_RenderCopy(mRenderer, mTapeIcon, nullptr, &r);

	// Render menu if active
	if (mMenu) {
		mMenu->render();
	}
	// Present the screen
	SDL_RenderPresent(mRenderer);
}

/*****************************************************************************/
bool CWindowSDL::loop() {
	//Event handler
	SDL_Event e;
	bool processed = false;

	//Handle events on queue
	while (SDL_PollEvent(&e) != 0) {
		switch (e.type) {
		case SDL_QUIT:
			return true;

		case SDL_RENDER_TARGETS_RESET:
			if (!mFullScreen) {
				SDL_SetWindowSize(mWindow, VIDEOWIDTH * 2, VIDEOHEIGHT * 2);
			}
			break;

		case SDL_KEYDOWN:
			switch (e.key.keysym.sym) {
			case SDLK_UP:
				if (mMenu) {
					processed = true;
					mMenu->up();
				}
				break;

			case SDLK_DOWN:
				if (mMenu) {
					processed = true;
					mMenu->down();
				}
				break;

			case SDLK_RETURN:
			case SDLK_KP_ENTER:
				if (mMenu) {
					processed = true;
					mMenuSel = mMenu->getSelected();
				}
				break;

			case SDLK_F7:
				setScanline(!mScanLines);
				processed = true;
				break;

			case SDLK_F8:
				setFullScreen(!mFullScreen);
				processed = true;
				break;

			}
			if (!processed) {
				// Notify all observers
				notify(e.key);
			}
			break;

		case SDL_KEYUP:
			// Notify all observers
			notify(e.key);
			break;

		}
	}
	return false;
}

/*****************************************************************************/
SDL_Window* CWindowSDL::getWindow() const {
	return mWindow;
}

/*****************************************************************************/
SDL_Renderer* CWindowSDL::getRenderer() const {
	return mRenderer;
}

/*****************************************************************************/
CMenu* CWindowSDL::getMenu() const {
	return mMenu;
}

/*****************************************************************************/
void CWindowSDL::setMenu(CMenu* menu) {
	mMenu = menu;
	mMenuSel = -1;
}

/*****************************************************************************/
int CWindowSDL::getMenuSel() const {
	return mMenuSel;
}