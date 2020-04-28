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

#include <cstdio>
#include <cstring>
#include <cassert>
#include <stdexcept>
#include "Audio.h"

/* Static Constants */

const double CAudio::mClocksPerSample = 1022727.0 / (double)SAMPLERATE;	// 6502 clock / sampleRate


/*************************************************************************************************/
void CAudio::playCallback(void *userdata, uint8_t *stream, int len) {
	static short soundPos{ -32767 };
	auto obj = reinterpret_cast<CAudio *>(userdata);
	int16_t *buffer = (int16_t *)stream;
	int bufferLen = len / sizeof(uint16_t);
	memset(stream, 0, len);
	const std::lock_guard<std::mutex> lock(obj->myMutex);
	if (obj->mCyclesQueue.size() == 0) {
		SDL_PauseAudioDevice(obj->mPlayDevId, SDL_TRUE);
		return;
	}
	static unsigned long long audioCycleStart{ 0 };
	if (audioCycleStart == 0) {
		audioCycleStart = obj->mCyclesQueue.front();
	}
	unsigned long long cycleActual;
	unsigned long long nextCycle = obj->mCyclesQueue.front();
	for (int i = 0; i < bufferLen; i++) {
		cycleActual = audioCycleStart + (unsigned long long)((double)i * mClocksPerSample);
		if (cycleActual >= nextCycle) {
			soundPos = -soundPos;
			if (obj->mCyclesQueue.size() == 0) {
				SDL_PauseAudioDevice(obj->mPlayDevId, SDL_TRUE);
				audioCycleStart = 0;
				return;
			} else {
				obj->mCyclesQueue.pop();
				if (obj->mCyclesQueue.size()== 0) {
					SDL_PauseAudioDevice(obj->mPlayDevId, SDL_TRUE);
					audioCycleStart = 0;
					return;
				}
				nextCycle = obj->mCyclesQueue.front();
			}
		}
		buffer[i] = soundPos;
	}
	audioCycleStart = cycleActual;
}

/*************************************************************************************************/
CAudio::CAudio(CBus *bus, CCpu6502 *cpu) : mCpu(cpu) {
	assert(bus != nullptr);
	assert(mCpu != nullptr);

	//Default audio spec
	SDL_AudioSpec receivedPlaybackSpec;
	SDL_AudioSpec desiredPlaybackSpec;

	SDL_zero(desiredPlaybackSpec);
	desiredPlaybackSpec.freq = SAMPLERATE;
	desiredPlaybackSpec.format = AUDIO_S16SYS;
	desiredPlaybackSpec.channels = 1;
	desiredPlaybackSpec.samples = (uint16_t)(SAMPLERATE / (1.0 / 0.1));	// Samples for 100ms
	desiredPlaybackSpec.userdata = this;
	desiredPlaybackSpec.callback = playCallback;

	//Open playback device
	mPlayDevId = SDL_OpenAudioDevice(NULL, SDL_FALSE, &desiredPlaybackSpec, &receivedPlaybackSpec, 0);
	if (mPlayDevId == 0) {
		std::string error{ "Error creating screen renderer! SDL Error: " };
		error.append(SDL_GetError());
		throw std::runtime_error(error);
	}

	bus->addDevice(0xC030, 0xC03F, this);
}

/*************************************************************************************************/
CAudio::~CAudio() {
	SDL_CloseAudioDevice(mPlayDevId);
}

/*************************************************************************************************/
byte CAudio::read(const word addr) {
	if (mCpu->getFullSpeed()) {
		return 0xFF;
	}
	const std::lock_guard<std::mutex> lock(myMutex);
	mCyclesQueue.emplace(mCpu->getCumulativeCycles());
	return 0xFF;
}

/*************************************************************************************************/
void CAudio::write(word addr, byte data) {
	read(addr);
}

/*************************************************************************************************/
void CAudio::update() {
	if (mCpu->getFullSpeed()) {
		return;
	}
	const std::lock_guard<std::mutex> lock(myMutex);
	if (SDL_GetAudioDeviceStatus(mPlayDevId) == SDL_AUDIO_PAUSED) {
		if (mCyclesQueue.size() > 0) {
			const unsigned long long first = mCyclesQueue.front();
			const unsigned long long last = mCyclesQueue.back();
			// 100 ms
			if (last - first > (102354)) {
				SDL_PauseAudioDevice(mPlayDevId, SDL_FALSE);
			}
			if (mCpu->getCumulativeCycles() - last > (102354)) {
				mCyclesQueue.emplace(mCpu->getCumulativeCycles());	// force update
			}
		}
	}
}

/*************************************************************************************************/
void CAudio::reset() {
	const std::lock_guard<std::mutex> lock(myMutex);
	SDL_PauseAudioDevice(mPlayDevId, SDL_TRUE);
	while (mCyclesQueue.size() > 0) {
		mCyclesQueue.pop();
	}
}

