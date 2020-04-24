// Copyright (c) 2020 FBLabs
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#pragma once

#include <queue>
#include "Device.h"
#include "Bus.h"
#include "Cpu6502.h"

class CTape : public CDevice {
public:
	CTape(CBus *bus, CCpu6502 *cpu);
	~CTape();
	byte read(word addr);
	void write(word addr, byte data);
	void update();
	void reset();
	void play();
	bool getPlayState();
private:
	CCpu6502 *mCpu = nullptr;
	unsigned long long mStartCycle = 0;
	std::queue<word> mQueueCycles;
	word mCyclesNeeded;
	byte mCasOut = 0;
	bool mPlay = false;
	void loadCt2(const char *fileName);
};
