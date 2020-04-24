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

#include <chrono>
#include <thread>
#include "Machine.h"

/* Constants */

/*************************************************************************************************/
CMachine::CMachine() {

}

/*************************************************************************************************/
CMachine::~CMachine() {
	//Free resources and close SDL
	SDL_DestroyRenderer(mRenderer);
	//Destroy window
	SDL_DestroyWindow(mWindow);
	//Quit SDL subsystems
	SDL_Quit();
}

/*************************************************************************************************/
bool CMachine::init() {
	mWindow = SDL_CreateWindow("TK2000 Emulator", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, VIDEOWIDTH * 2, VIDEOHEIGHT * 2 + 20, SDL_WINDOW_SHOWN);

	if (!mWindow) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, 
			"Window could not be created! SDL Error: %s\n", SDL_GetError());

		return false;
	}
	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
	if (!mRenderer) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, 
			"Error creating renderer! SDL Error: %s\n", SDL_GetError());

		return false;
	}
	mBus = std::make_unique<CBus>();
	mCpu = std::make_unique<CCpu6502>(mBus.get());
	mCpu->setClock(1022727);
	mRam = std::make_unique<CRam>(mBus.get());
	mRom = std::make_unique<CRom>(mBus.get());
	mVideo = std::make_unique<CVideo>(mRenderer, mBus.get(), mRam.get());
	mAudio = std::make_unique<CAudio>(mBus.get(), mCpu.get());
	mKeyboard = std::make_unique<CKeyboard>(mBus.get());
	mTape = std::make_unique<CTape>(mBus.get(), mCpu.get());

	mCpu->reset();
	mRam->reset();
	mRom->reset();
	mVideo->reset();
	mAudio->reset();
	mKeyboard->reset();
	mTape->reset();

	return true;
}

/*************************************************************************************************/
bool CMachine::loop() {
	std::chrono::time_point<std::chrono::high_resolution_clock> previous;
	//Main loop flag
	bool quit = false;
	constexpr const int msToRun = 100000;
	constexpr const int frameRateMs = 1000000 / 60;

	//Event handler
	SDL_Event e;

	// CPU Thread
	std::thread cpuThread([&]() {
		std::chrono::time_point<std::chrono::high_resolution_clock> previous, now;
		while (!quit) {
			previous = std::chrono::high_resolution_clock::now();
			unsigned long cyclesToRun = (unsigned long)((double)msToRun / (1000000.0 / mCpu->getClock()));	// 100ms
			unsigned long long actualCycles = mCpu->getCumulativeCycles();
			while ((mCpu->getCumulativeCycles() - actualCycles) < cyclesToRun) {
				mCpu->executeOpcode();
			}
			// Do updates
			mAudio->update();
			mVideo->update();
			mTape->update();
			now = std::chrono::high_resolution_clock::now();
			auto timePast = now - previous;
			if (!mCpu->getFullSpeed() && timePast < std::chrono::microseconds(msToRun)) {
				std::this_thread::sleep_for(std::chrono::microseconds(msToRun) - timePast);
			}
		}
	});


	//While application is running
	while (!quit) {
		previous = std::chrono::high_resolution_clock::now();
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0) {
			//User requests quit
			if (e.type == SDL_QUIT) {
				quit = true;
			} else if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_F5) {
					mCpu->reset();
					mRam->reset();
					mRom->reset();
					mVideo->reset();
					mAudio->reset();
					mKeyboard->reset();
					mTape->reset();
				} else if (e.key.keysym.sym == SDLK_F6) {
					mTape->play();
				} else if (e.key.keysym.sym == SDLK_F7) {
					mVideo->setScanline(!mVideo->getScanline());
				} else {
					mKeyboard->processEvent(e.key);
				}
			} else if (e.type == SDL_KEYUP) {
				mKeyboard->processEvent(e.key);
			}
		}

		// Render screen
		SDL_SetRenderTarget(mRenderer, nullptr);
		SDL_SetRenderDrawColor(mRenderer, 128, 128, 128, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(mRenderer);
		mVideo->render();
		SDL_RenderPresent(mRenderer);
		auto timePast = std::chrono::high_resolution_clock::now() - previous;
		//fprintf(stderr, "Wasted %f mS\n", wasted.count());
		if (timePast < std::chrono::microseconds(frameRateMs)) {
			std::this_thread::sleep_for(std::chrono::microseconds(frameRateMs) - timePast);
		/*} else {
			auto wasted = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(timePast);
			fprintf(stderr, "Vide wasted %f mS\n", wasted.count());*/
		}
	}
	cpuThread.join();
	return true;
}
