
#include <cstdio>
#include <cstring>
#include "Audio.h"

/*************************************************************************************************/
void playCallback(void *userdata, uint8_t * stream, int len) {
	static short soundPos = -32000;
	auto obj = reinterpret_cast<CAudio *>(userdata);
	int16_t *buffer = (int16_t *)stream;
	size_t bufferLen = len / sizeof(uint16_t);
	memset(stream, 0, len);
	// Em 44100, temos 44100 bytes em 1 segundo, ou seja, cada sample tem 22,67 us de duração
	// 1 ciclo de máquina do 6502 tem 0,977 us
	// A cada sample de áudio dura 23.2 ciclos de máquina
	if (obj->mCyclesQueue.size() < 2) {
		SDL_PauseAudioDevice(obj->playbackDeviceId, SDL_TRUE);
		return;
	}
	static unsigned long long audioCycleStart = obj->mCyclesQueue.front();
	unsigned long long cycleActual;
//	obj->mCyclesQueue.pop();
	unsigned long long nextCycle = obj->mCyclesQueue.front();
	for (int i = 0; i < bufferLen; i++) {
		cycleActual = audioCycleStart + (unsigned long long)((double)i * 23.2);
		if (cycleActual >= nextCycle) {
			soundPos = -soundPos;
			if (obj->mCyclesQueue.size() == 0) {
				SDL_PauseAudioDevice(obj->playbackDeviceId, SDL_TRUE);
				return;
			} else {
				obj->mCyclesQueue.pop();
				if (obj->mCyclesQueue.size()== 0) {
					SDL_PauseAudioDevice(obj->playbackDeviceId, SDL_TRUE);
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
CAudio::CAudio(CBus *bus) {
	//Default audio spec
	SDL_AudioSpec receivedPlaybackSpec;
	SDL_AudioSpec desiredPlaybackSpec;

	SDL_zero(desiredPlaybackSpec);
	desiredPlaybackSpec.freq = 44100;
	desiredPlaybackSpec.format = AUDIO_S16;
	desiredPlaybackSpec.channels = 1;
	desiredPlaybackSpec.samples = 720;
	desiredPlaybackSpec.userdata = this;
	desiredPlaybackSpec.callback = playCallback;

	//Open playback device
	playbackDeviceId = SDL_OpenAudioDevice(NULL, SDL_FALSE, &desiredPlaybackSpec, &receivedPlaybackSpec, 0);
	if (playbackDeviceId == 0) {
		throw "Error initializing audio";
	}

	bus->addDevice(0xC030, 0xC03F, this);
}

/*************************************************************************************************/
byte CAudio::read(word addr) {
	mCyclesQueue.emplace(mCumulativeCycles + mCycles);
	return 0xFF;
}

/*************************************************************************************************/
void CAudio::write(word addr, byte data) {
	read(addr);
}

/*************************************************************************************************/
void CAudio::update(unsigned long cycles) {
	mCumulativeCycles += cycles;
	if (SDL_GetAudioDeviceStatus(playbackDeviceId) == SDL_AUDIO_PAUSED) {
		if (mCyclesQueue.size() > 0) {
			unsigned long long first = mCyclesQueue.front();
			unsigned long long last = mCyclesQueue.back();
			if (last - first > (16667 * 2)) {
				SDL_PauseAudioDevice(playbackDeviceId, SDL_FALSE);
			}
		}
	}
}

/*************************************************************************************************/
void CAudio::reset() {
	mCumulativeCycles = 0;
}

