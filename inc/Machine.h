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
#include "AudioSDL.h"

/*************************************************************************************************/
class CMachine final : public CObserver<SDL_KeyboardEvent> {
public:
	CMachine();
	~CMachine();
	// CObserver
	void notify(SDL_KeyboardEvent&) override;
	// Native
	bool setTapeFile(const char *filename);
	bool loop();
	void setCpuPause(bool pause);
private:
	CBus mBus{};
	CCpu6502 mCpu{ mBus };
	CRom mRom{ mBus };
	CRam mRam{ mBus };
	CVideo mVideo{ mBus, mRam.mRam };
	CAudio mAudio{ mBus, mCpu };
	CKeyboard mKeyboard{ mBus };
	CTape mTape{ mBus };
	CWindowSDL mWindow{ mVideo };
	CAudioSDL mAudioSDL{ mAudio };
	bool mCpuPaused = true;
};
