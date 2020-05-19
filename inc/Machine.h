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

#pragma once

#include "Common.h"
#include "Bus.h"
#include "Cpu6502.h"
#include "Video.h"
#include "Ram.h"
#include "Rom.h"
#include "Audio.h"
#include "Keyboard.h"
#include "Tape.h"
#include "WindowSDL.h"
#include "Observer.h"

/*************************************************************************************************/
class CMachine final : public CObserver<SDL_KeyboardEvent> {
public:
	CMachine();
	~CMachine();
	// CObserver
	void notify(SDL_KeyboardEvent*) override;
	// Native
	bool setTapeFile(const char *filename);
	bool loop();
private:
	std::unique_ptr<CCpu6502> mCpu{};
	std::unique_ptr<CVideo> mVideo{};
	std::unique_ptr<CBus> mBus{};
	std::unique_ptr<CRam> mRam{};
	std::unique_ptr<CRom> mRom{};
	std::unique_ptr<CAudio> mAudio{};
	std::unique_ptr<CKeyboard> mKeyboard{};
	std::unique_ptr<CTape> mTape{};
	std::unique_ptr<CWindowSDL> mWindow{};
};
