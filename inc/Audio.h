
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
};
