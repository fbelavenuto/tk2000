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

#include "Device.h"
#include "Bus.h"
#include "Cpu6502.h"

class CAudio : public CDevice {
public:
	CAudio(CBus *bus, CCpu6502 *cpu);
	~CAudio() override;
	byte read(const word addr) override;
	void write(const word addr, const byte data) override;
	void update() override;
	void reset() override;
	//
	int getSampleRate() const;
private:
	CCpu6502 *mCpu{};
	int mSampleRate{ 44100 };
	std::mutex myMutex;
	std::queue<unsigned long long> mCyclesQueue;
	SDL_AudioDeviceID mPlayDevId;
	static void playCallback(void *userdata, uint8_t * stream, int len);
};
