
#pragma once

#include <SDL2/SDL.h>
#include "Device.h"
#include "Bus.h"

class CKeyboard : public CDevice {
public:
	CKeyboard(CBus *bus);
	byte read(word addr);
	void write(word addr, byte data);
	void update(unsigned long cycles);
	void reset();
	void processEvent(SDL_KeyboardEvent e);
private:
	bool mCtrl = false;
	bool mShift = false;
	int mCol = 0;
	int mRow = 0;
	byte mKbOut = 0;
	bool mKbOutCtrl = 0;
};