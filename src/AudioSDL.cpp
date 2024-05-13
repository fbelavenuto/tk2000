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
#include "AudioSDL.h"


/*************************************************************************************************/
CAudioSDL::CAudioSDL(CSubject<sAudioMsg>& sub) {
	//Default audio spec
	SDL_AudioSpec receivedPlaybackSpec;
	SDL_AudioSpec desiredPlaybackSpec;

	SDL_zero(desiredPlaybackSpec);
	desiredPlaybackSpec.freq = SAMPLERATE;
	desiredPlaybackSpec.format = AUDIO_S16SYS;
	desiredPlaybackSpec.channels = 1;
	desiredPlaybackSpec.samples = (uint16_t)BUFSIZE;
	desiredPlaybackSpec.userdata = this;
	desiredPlaybackSpec.callback = cbFillBuffer;

	//Open playback device
	mPlayDevId = SDL_OpenAudioDevice(NULL, SDL_FALSE, &desiredPlaybackSpec, &receivedPlaybackSpec, 0);
	if (mPlayDevId == 0) {
		std::string error{ "Error opening audio device! SDL Error: " };
		error.append(SDL_GetError());
		throw std::runtime_error(error);
	}
	mIdxWriteBuffer = 0;
	mWritePos = 0;
	mIdxReadBuffer = 0;
	mAllBuffers = new int16_t[(long)BUFSIZE * BUFCOUNT];
	if (!mAllBuffers) {
		throw std::runtime_error("Out of memory");
	}

	mSemaphore = SDL_CreateSemaphore(BUFCOUNT - 1);
	if (!mSemaphore) {
		std::string error{ "Error creating Semaphore! SDL Error: " };
		error.append(SDL_GetError());
		throw std::runtime_error(error);
	}
	SDL_PauseAudioDevice(mPlayDevId, SDL_FALSE);
	//createWaveFile("z.wav");
	sub.attach(this);
}

/*************************************************************************************************/
CAudioSDL::~CAudioSDL() {
	SDL_PauseAudioDevice(mPlayDevId, SDL_TRUE);
	SDL_CloseAudioDevice(mPlayDevId);
	SDL_DestroySemaphore(mSemaphore);
	delete[] mAllBuffers;
	//finishWaveFile();
}

/*************************************************************************************************/
inline int16_t* CAudioSDL::getBufferPtr(int index) {
	assert(index < BUFCOUNT);
	return mAllBuffers + (long)index * BUFSIZE;
}

/*************************************************************************************************/
void CAudioSDL::notify(sAudioMsg& m) {
	while (m.count) {
		unsigned long n = BUFSIZE - mWritePos;
		if (n > m.count)
			n = m.count;

		memcpy(getBufferPtr(mIdxWriteBuffer) + mWritePos, m.bufferPtr, n * sizeof(int16_t));
		m.bufferPtr += n;
		mWritePos += n;
		m.count -= n;

		if (mWritePos >= BUFSIZE) {
			mWritePos = 0;
			mIdxWriteBuffer = (mIdxWriteBuffer + 1) % BUFCOUNT;
			SDL_SemWait(mSemaphore);
		}
	}
}

/*************************************************************************************************/
void CAudioSDL::fillBuffer(void* out, int count) {
	if (SDL_SemValue(mSemaphore) < (uint32_t)(BUFCOUNT - 1))	{
		int16_t* p = getBufferPtr(mIdxReadBuffer);
		//writeToWaveFile((unsigned char*)p, count);
		memcpy(out, p, count);
		mIdxReadBuffer = (mIdxReadBuffer + 1) % BUFCOUNT;
		SDL_SemPost(mSemaphore);
	} else {
		memset(out, 0, count);
	}
}

/*************************************************************************************************/
void CAudioSDL::cbFillBuffer(void* userdata, uint8_t* out, int count) {
	auto obj = reinterpret_cast<CAudioSDL *>(userdata);
	obj->fillBuffer(out, count);
}