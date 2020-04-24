
#pragma once

#include <queue>
#include "Device.h"
#include "Bus.h"
#include "Cpu6502.h"

class CTape : public CDevice {
public:
	CTape(CBus *bus, CCpu6502 *cpu);
	byte read(word addr);
	void write(word addr, byte data);
	void update(unsigned long cycles);
	void reset();
	void play();
private:
	CCpu6502 *mCpu = nullptr;
	unsigned long long mStartCycle = 0;
	std::queue<word> mQueueCycles;
	word mCyclesNeeded;
	byte mCasOut = 0;
	bool mPlay = false;
};