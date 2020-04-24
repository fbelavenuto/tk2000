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

#include <memory>
#include <SDL2/SDL.h>
#include "DataTypes.h"
#include "Bus.h"
#include "Cpu6502.h"
#include "Video.h"
#include "Ram.h"
#include "Rom.h"
#include "Audio.h"
#include "Keyboard.h"
#include "Tape.h"

/*************************************************************************************************/
class CMachine {
public:
	CMachine();
	~CMachine();
	bool init();
	bool setTapeFile(const char *filename);
	bool loop();
private:
	SDL_Window *mWindow = nullptr;
	SDL_Renderer *mRenderer = nullptr;
	std::unique_ptr<CCpu6502> mCpu = nullptr;
	std::unique_ptr<CVideo> mVideo = nullptr;
	std::unique_ptr<CBus> mBus = nullptr;
	std::unique_ptr<CRam> mRam = nullptr;
	std::unique_ptr<CRom> mRom = nullptr;
	std::unique_ptr<CAudio> mAudio = nullptr;
	std::unique_ptr<CKeyboard> mKeyboard = nullptr;
	std::unique_ptr<CTape> mTape = nullptr;
};
