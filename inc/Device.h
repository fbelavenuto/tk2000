
#pragma once

#include "DataTypes.h"

class CDevice {
public:
	virtual byte read(word addr) = 0;
	virtual void write(word addr, byte data) = 0;
	virtual void reset() = 0;
	virtual void update(unsigned long cycles) = 0;
	void setCycles(unsigned long cycles) {
		mCycles = cycles;
	}
protected:
	unsigned long mCycles;
};
