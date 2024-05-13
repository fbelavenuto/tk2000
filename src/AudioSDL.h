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

#pragma once

#include "Common.h"
#include "Observer.h"
#include "Subject.h"

/*************************************************************************************************/
class CAudioSDL final : public CObserver<sAudioMsg> {
public:
	CAudioSDL(CSubject<sAudioMsg>& sub);
	~CAudioSDL();
	// CObserver
	void notify(sAudioMsg&);
private:
	SDL_AudioDeviceID mPlayDevId;
	const unsigned long BUFSIZE = NUMSAMPLESPERUPDATE;
	const int BUFCOUNT{ 3 };
	int16_t* volatile mAllBuffers;
	SDL_sem* volatile mSemaphore;
	int volatile mIdxReadBuffer;
	int mIdxWriteBuffer;
	unsigned long mWritePos;
	int16_t* getBufferPtr(int index);
	void fillBuffer(void*, int);
	static void cbFillBuffer(void*, uint8_t*, int);
};
