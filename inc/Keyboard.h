
#pragma once

#include <SDL2/SDL.h>
#include "Device.h"
#include "Bus.h"

class CKeyboard : public CDevice {
public:
	CKeyboard(CBus *bus);
	byte read(word addr);
	void write(word addr, byte data);
	void update();
	void reset();
	void processEvent(SDL_KeyboardEvent e);
private:
	byte mMatrix[8];
	bool mCtrl = false;
	bool mShift = false;
	byte mKbOut = 0;
	bool mKbOutCtrl = 0;
};