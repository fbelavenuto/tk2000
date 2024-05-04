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
#include "Machine.h"

/*************************************************************************************************/
CMachine::CMachine() {
	// First create Bus
	
	// Reset all devices
	mBus.resetAll();

	// Attach observers
	mWindow.attach(&mKeyboard);
	mWindow.attach(this);
}

/*************************************************************************************************/
CMachine::~CMachine() {
	mWindow.detach(this);
	mWindow.detach(&mKeyboard);
}

/*************************************************************************************************/
// Receiving keyboard notification from event loop
void CMachine::notify(SDL_KeyboardEvent& e) {
	if (e.state == SDL_PRESSED) {
		switch (e.keysym.sym) {
		case SDLK_F5:
			mBus.resetAll();
			break;

		case SDLK_F6:
			mTape.play();
			break;
		}
	}
}

/*************************************************************************************************/
bool CMachine::setTapeFile(const char *filename) {
	return mTape.insertCt2(filename);
}

/*************************************************************************************************/
bool CMachine::loop() {

	//Main loop flag
	bool quit = false;
	constexpr const int usToRun = (int)(MS_TO_RUN * 1000.0);

	// CPU Thread
	std::thread cpuThread([&]() {
		std::chrono::time_point<std::chrono::high_resolution_clock> previous, now;
		while (!quit) {
			previous = std::chrono::high_resolution_clock::now();
			const unsigned long cyclesToRun = (unsigned long)((double)usToRun / (1000000.0 / mCpu.getClock()));
			const unsigned long long actualCycles = mCpu.getCumulativeCycles();
			while ((mCpu.getCumulativeCycles() - actualCycles) < cyclesToRun) {
				mCpu.executeOpcode();
			}
			// Do updates
			mBus.updateAll(mCpu.getCumulativeCycles());
			now = std::chrono::high_resolution_clock::now();
			auto timePast = now - previous;
			if (!mCpu.getFullSpeed() && timePast < std::chrono::microseconds(usToRun)) {
				std::this_thread::sleep_for(std::chrono::microseconds(usToRun) - timePast);
			}
		}
	});

	//While application is running
	while (!quit) {
		quit = mWindow.loop();
		// Render screen
		mWindow.render();
	}
	cpuThread.join();
	return true;
}

