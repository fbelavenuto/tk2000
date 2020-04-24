
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
		SDL_WINDOWPOS_UNDEFINED, videoWidth * 2, videoHeight * 2, SDL_WINDOW_SHOWN);

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
	SDL_RenderSetLogicalSize(mRenderer, videoWidth, videoHeight);
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

	Uint32 format = SDL_GetWindowPixelFormat(mWindow);
	//fprintf(stderr, "Format: %d\n", format);
	return true;
}

/*************************************************************************************************/
bool CMachine::loop() {
	std::chrono::time_point<std::chrono::high_resolution_clock> previous, now;
	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;

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
					mCpu->setClock(0);
				} else if (e.key.keysym.sym == SDLK_F8) {
					mCpu->setClock(1022727);
				} else {
					mKeyboard->processEvent(e.key);
				}
			} else if (e.type == SDL_KEYUP) {
				mKeyboard->processEvent(e.key);
			}
		}
		bool fullSpeed = mCpu->getClock() == 0;
		unsigned long cycles;
		if (fullSpeed) {
			cycles = mCpu->execute(100000);
		} else {
			// 16.66ms = 17059 cycles when 1MHz
			int cyclesToExecute = (int)(mCpu->getClock() / 59.95);
			cycles = mCpu->execute(cyclesToExecute);
		}
		// Do updates
		mAudio->update(cycles);

		// Render screen
		SDL_SetRenderTarget(mRenderer, nullptr);
		SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 0);
		SDL_RenderClear(mRenderer);
		mVideo->render();
		SDL_RenderPresent(mRenderer);
		now = std::chrono::high_resolution_clock::now();
		auto timePast = now - previous;
		//fprintf(stderr, "Wasted %f mS\n", wasted.count());
		if (!fullSpeed && timePast < std::chrono::microseconds(16666)) {
			std::this_thread::sleep_for(std::chrono::microseconds(16666) - timePast);
		/*} else {
			auto wasted = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(timePast);
			fprintf(stderr, "Wasted %f mS\n", wasted.count());*/
		}
	}
	return true;
}
