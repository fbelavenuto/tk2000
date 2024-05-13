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

class CTape final : public CDevice {
public:
	CTape(CBus& bus, CCpu6502& cpu);
	// CDevice
	byte read(const word addr, const uint64_t cycles) override;
	void write(const word addr, const byte data, const uint64_t cycles) override {};
	void update(const uint64_t cycles) override {};
	void reset() override;
	// Native
	void play() noexcept;
	void stop() noexcept;
	void rewind() noexcept;
	bool getPlayState() const noexcept;
	bool insertCt2(const char *fileName);
private:
	CCpu6502& mCpu;
	unsigned long long mStartCycle{ 0 };
	std::vector<word> mQueueCycles;
	std::vector<word>::iterator mQueueIt;
	word mCyclesNeeded;
	byte mCasOut{ 0 };
	bool mPlay{ false };
};
