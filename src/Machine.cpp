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

/* Constants */

TMenuLst mainLst{ "Start", "Reset (F5)", "CPU speed", "Insert tape", "Play tape (F6)", "Rewind tape" };
TMenuLst cpuSpeedLst{ "1 MHz", "2 MHz", "Full speed" };
TMenuLst filesLst{ "." };

namespace fs = std::filesystem;

/*************************************************************************************************/
void CMachine::fillFilesLst() {
	auto p{ fs::current_path() };
	std::vector<std::string> dirs, files;
	if (p.string().length() > 3) {
		dirs.emplace_back("[..]");
	}
	for (auto& entry : fs::directory_iterator(p)) {
		auto filename = entry.path().filename();
		auto status = entry.status();
		if (fs::is_directory(status)) {
			dirs.emplace_back("[" + filename.string() + "]");
		} else {
			if (filename.extension().string() == ".ct2")
				files.emplace_back(filename.string());
		}
	}
	std::sort(dirs.begin(), dirs.end());
	std::sort(files.begin(), files.end());
	auto& lst = mFilesMenu->getMenuLst();
	lst.clear();
	for (auto& d : dirs) {
		lst.emplace_back(d);
	}
	for (auto& d : files) {
		lst.emplace_back(d);
	}
	mFilesMenu->getTitle() = fs::current_path().string();
	mFilesMenu->setMenuLst(lst);
}


/*************************************************************************************************/
CMachine::CMachine() {

	// Reset all devices
	mBus.resetAll();

	// Attach observers
	mWindow.attach(this);

	// Construct menus
	mMainMenu = new CMenu(mainLst, mWindow.getRenderer(), "Main");
	mCpuMenu = new CMenu(cpuSpeedLst, mWindow.getRenderer(), "CPU Speed");
	mFilesMenu = new CMenu(filesLst, mWindow.getRenderer(), "Choose file");
}

/*************************************************************************************************/
CMachine::~CMachine() {
	mWindow.detach(this);
}

/*************************************************************************************************/
// Receiving keyboard notification from event loop
void CMachine::notify(SDL_KeyboardEvent& e) {
	bool inMenu = nullptr != mWindow.getMenu();
	bool processed = false;
	if (e.state == SDL_PRESSED) {
		switch (e.keysym.sym) {
		case SDLK_ESCAPE:
			if (inMenu) {
				mWindow.setMenu(nullptr);
				processed = true;
			}
			break;

		case SDLK_F2:
			if (inMenu) {
				mWindow.setMenu(nullptr);
			} else {
				mMainMenu->getMenuLst()[0] = mCpuPaused ? "Start" : "Pause";
				mWindow.setMenu(mMainMenu);
			}
			processed = true;
			break;

		case SDLK_F5:
			mCpuPaused = false;
			mBus.resetAll();
			processed = true;
			break;

		case SDLK_F6:
			mTape.play();
			processed = true;
			break;
		}
	}
	// Send if not processed and not in menu
	if (!inMenu && !processed) {
		mKeyboard.keyEvent(e);
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
			if (!mCpuPaused) {
				const unsigned long cyclesToRun = (unsigned long)((double)usToRun / (1000000.0 / mCpu.getClock()));
				const unsigned long long actualCycles = mCpu.getCumulativeCycles();
				while ((mCpu.getCumulativeCycles() - actualCycles) < cyclesToRun) {
					mCpu.executeOpcode();
				}
				// Do updates
				mBus.updateAll(mCpu.getCumulativeCycles());
			}
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
		int menuSel = mWindow.getMenuSel();
		if (menuSel != -1) {
			CMenu* menu = nullptr;
			// menu selected
			if (mWindow.getMenu() == mMainMenu) {
				switch (menuSel) {
				case 0:								// Start/stop
					mCpuPaused = !mCpuPaused;
					break;

				case 1:								// Reset
					mCpuPaused = false;
					mBus.resetAll();
					break;

				case 2:								// CPU speed
					menu = mCpuMenu;
					break;

				case 3:								// Insert tape
					fillFilesLst();
					menu = mFilesMenu;
					break;

				case 4:								// Play tape
					mTape.play();
					mCpuPaused = false;
					break;

				case 5:								// Rewind tape
					mCpuPaused = false;
					break;
				}
			} else if (mWindow.getMenu() == mCpuMenu) {
				switch (menuSel) {
				case 0:								// 1x
					mCpu.setFullSpeed(false);
					mCpu.setClock(CPU_CLOCK);
					mCpuPaused = false;
					break;

				case 1:								// 2x
					mCpu.setFullSpeed(false);
					mCpu.setClock(CPU_CLOCK*2);
					mCpuPaused = false;
					break;

				case 2:								// Full
					mCpu.setFullSpeed(true);
					mCpu.setClock(CPU_CLOCK);
					mCpuPaused = false;
					break;

				}
			} else if (mWindow.getMenu() == mFilesMenu) {
				const std::string& cur = mFilesMenu->getMenuLst()[menuSel];
				if (cur == "[..]") {
					std::error_code error;
					fs::current_path(fs::current_path().parent_path(), error);
					fillFilesLst();
					menu = mFilesMenu;
				} else if (cur[0] == '[') {
					fs::current_path(fs::current_path().append(cur.substr(1, cur.length()-2)));
					fillFilesLst();
					menu = mFilesMenu;
				} else {
					mTape.insertCt2(cur.c_str());
				}
			}
			mWindow.setMenu(menu);
		}
	}
	cpuThread.join();
	return true;
}

/*************************************************************************************************/
void CMachine::setCpuPause(bool pause) {
	mCpuPaused = pause;
}