
#pragma once

#include <mutex>
#include <SDL2/SDL.h>
#include "DataTypes.h"
#include "Device.h"
#include "Bus.h"

class CAudio : public CDevice {
public:
	CAudio(CBus *bus);
	byte read(word addr);
	void write(word addr, byte data);
	void update(unsigned long cycles);
	void reset();
private:
	static const int BUFFERSIZE = 735 * 2;
	static const int SAMPLERATE = 44100;
	std::mutex myMutex;
	const double mClocksPerSample = 1022727.0 / (double)SAMPLERATE;	// 6502 clock / sampleRate
	unsigned long long mCumulativeCycles = 0;
	unsigned long long mLastCycle = 0;
	bool mSoundFlag = false;
	int16_t mBuffer[BUFFERSIZE];
	int mReadPos = 0;
	int mWritePos = 0;
	SDL_AudioDeviceID mPlayDevId;
	void updateAudio();
	static void playCallback(void *userdata, uint8_t * stream, int len);
};
