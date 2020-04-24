
#pragma once

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
	static const int BUFFERSIZE = 1024 * 3;
	static const int SAMPLERATE = 61362;
	const double mClocksPerSample = 1022727.0 / (double)SAMPLERATE;	// 6502 clock / sampleRate
	unsigned long long mCumulativeCycles = 0;
	unsigned long long mLastCycle = 0;
	bool mSoundFlag = false;
	int16_t mBuffer[BUFFERSIZE];
	int mReadPos = 0;
	int mWritePos = 0;
	SDL_AudioDeviceID mPlayDevId;
	void updateAudio();
};
