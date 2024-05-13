// Copyright (c) 2024 FBLabs
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

TMenuLst mainLst{ "Start", "Soft Reset (F5)", "Hard reset", "CPU speed", "Insert tape", "Play tape (F6)", "Rewind tape" };
TMenuLst cpuSpeedLst{ "1 MHz", "2 MHz", "Full speed" };
TMenuLst filesLst{ "." };

namespace fs = std::filesystem;
std::string curpath = fs::current_path().string();

/*************************************************************************************************/
void CMachine::fillFilesLst() {
	
	std::vector<std::string> dirs, files;
	// workaroud for windows, show drive letters
	if (curpath.empty()) {
		std::string t = "a:/";
		for (char l = 'a'; l <= 'z'; l++) {
			t[0] = l;
			try {
				if (fs::exists(t)) {
					dirs.emplace_back(t);
				}
			} catch (std::exception& e) {
				// do nothing
			}
		}
	}
	if (fs::exists(curpath)) {
		for (auto& entry : fs::directory_iterator(curpath)) {
			auto filename = entry.path().filename();
			auto status = entry.status();
			if (fs::is_directory(status)) {
				dirs.emplace_back("[" + filename.string() + "]");
			} else {
				if (filename.extension().string() == ".ct2")
					files.emplace_back(filename.string());
			}
		}
	}
	std::sort(dirs.begin(), dirs.end());
	std::sort(files.begin(), files.end());
#ifdef _WIN32
	if (!curpath.empty())
#else
	if (curpath.length() > 3)
#endif
	{
		dirs.insert(dirs.begin(), "[..]");
	}
	auto& lst = mFilesMenu->getMenuLst();
	lst.clear();
	for (auto& d : dirs) {
		lst.emplace_back(d);
	}
	for (auto& d : files) {
		lst.emplace_back(d);
	}
	if (curpath.length() == 0) {
		mFilesMenu->getTitle() = "[DRIVES]";
	} else {
		mFilesMenu->getTitle() = curpath;
	}
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
		mWindow.render(mCpuPaused == false, mTape.getPlayState());
		int menuSel = mWindow.getMenuSel();
		if (menuSel != -1) {
			CMenu* menu = nullptr;
			// menu selected
			if (mWindow.getMenu() == mMainMenu) {
				switch (menuSel) {
				case 0:								// Start/stop
					mCpuPaused = !mCpuPaused;
					break;

				case 1:								// Soft Reset
					mCpuPaused = false;
					mBus.resetAll();
					break;

				case 2:								// Hard Reset
					mCpuPaused = false;
					mRam.init();
					mBus.resetAll();
					break;

				case 3:								// CPU speed
					menu = mCpuMenu;
					break;

				case 4:								// Insert tape
					fillFilesLst();
					menu = mFilesMenu;
					break;

				case 5:								// Play tape
					mTape.play();
					break;

				case 6:								// Rewind tape
					mTape.rewind();
					break;
				}
			} else if (mWindow.getMenu() == mCpuMenu) {
				switch (menuSel) {
				case 0:								// 1x
					mCpu.setFullSpeed(false);
					mCpu.setClock(CPU_CLOCK);
					break;

				case 1:								// 2x
					mCpu.setFullSpeed(false);
					mCpu.setClock(CPU_CLOCK*2);
					break;

				case 2:								// Full
					mCpu.setFullSpeed(true);
					mCpu.setClock(CPU_CLOCK);
					break;

				}
			} else if (mWindow.getMenu() == mFilesMenu) {
				const std::string& cur = mFilesMenu->getMenuLst()[menuSel];
				if (cur == "[..]") {				// Up dir
					std::error_code error;
#ifdef _WIN32
					if (curpath.length() == 3) {
						curpath.clear();
					} else
#endif
					{
						fs::current_path(fs::current_path().parent_path(), error);
						curpath = fs::current_path().string();
					}
					fillFilesLst();
					menu = mFilesMenu;
				} else if (cur[0] == '[') {			// Into in dir
					fs::current_path(fs::current_path().append(cur.substr(1, cur.length()-2)));
					curpath = fs::current_path().string();
					fillFilesLst();
					menu = mFilesMenu;
				} else if (curpath.length() == 0) {
					curpath = cur;
					fs::current_path(curpath);
					fillFilesLst();
					menu = mFilesMenu;
				} else {
					mTape.insertCt2(cur.c_str());	// Load file
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