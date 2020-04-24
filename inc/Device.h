
#pragma once

#include "DataTypes.h"

class CDevice {
public:
	virtual byte read(word addr) = 0;
	virtual void write(word addr, byte data) = 0;
	virtual void reset() = 0;
	virtual void update() = 0;
};
