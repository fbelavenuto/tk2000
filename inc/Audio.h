
#pragma once

#include <queue>
#include <mutex>
#include <SDL2/SDL.h>
#include "DataTypes.h"
#include "Device.h"
#include "Bus.h"
#include "Cpu6502.h"

class CAudio : public CDevice {
public:
	CAudio(CBus *bus, CCpu6502 *cpu);
	byte read(word addr);
	void write(word addr, byte data);
	void update();
	void reset();
private:
	CCpu6502 *mCpu = nullptr;
	static const int SAMPLERATE = 44100;
	static const double mClocksPerSample;
	std::mutex myMutex;
	std::queue<unsigned long long> mCyclesQueue;
	SDL_AudioDeviceID mPlayDevId;
	static void playCallback(void *userdata, uint8_t * stream, int len);
};
