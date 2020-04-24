
#include <cstdio>
#include <cstring>
#include <cassert>
#include "Audio.h"

/* Static Constants */

const double CAudio::mClocksPerSample = 1022727.0 / (double)SAMPLERATE;	// 6502 clock / sampleRate


/*************************************************************************************************/
void CAudio::playCallback(void *userdata, uint8_t *stream, int len) {
	static short soundPos = -32767;
	auto obj = reinterpret_cast<CAudio *>(userdata);
	int16_t *buffer = (int16_t *)stream;
	size_t bufferLen = len / sizeof(uint16_t);
	memset(stream, 0, len);
	const std::lock_guard<std::mutex> lock(obj->myMutex);
	if (obj->mCyclesQueue.size() < 1) {
		SDL_PauseAudioDevice(obj->mPlayDevId, SDL_TRUE);
		return;
	}
	static unsigned long long audioCycleStart = 0;
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
	desiredPlaybackSpec.freq = 44100;
	desiredPlaybackSpec.format = AUDIO_S16SYS;
	desiredPlaybackSpec.channels = 1;
	desiredPlaybackSpec.samples = 735;	// 16.66 ms
	desiredPlaybackSpec.userdata = this;
	desiredPlaybackSpec.callback = playCallback;

	//Open playback device
	mPlayDevId = SDL_OpenAudioDevice(NULL, SDL_FALSE, &desiredPlaybackSpec, &receivedPlaybackSpec, 0);
	if (mPlayDevId == 0) {
		throw "Error initializing audio";
	}

	bus->addDevice(0xC030, 0xC03F, this);
}

/*************************************************************************************************/
byte CAudio::read(word addr) {
	if (mCpu->getClock() == 0) {
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
void CAudio::update(unsigned long cycles) {
	if (mCpu->getClock() == 0) {
		return;
	}
	const std::lock_guard<std::mutex> lock(myMutex);
	if (SDL_GetAudioDeviceStatus(mPlayDevId) == SDL_AUDIO_PAUSED) {
		if (mCyclesQueue.size() > 0) {
			unsigned long long first = mCyclesQueue.front();
			unsigned long long last = mCyclesQueue.back();
			if (last - first > (16667 * 2)) {
				SDL_PauseAudioDevice(mPlayDevId, SDL_FALSE);
			}
			if (mCpu->getCumulativeCycles() - last > (16667 * 2)) {
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

