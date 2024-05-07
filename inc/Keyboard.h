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
#include "Observer.h"

class CKeyboard final : public CDevice {
public:
	CKeyboard(CBus& bus);
	// CDevice
	byte read(const word addr, const uint64_t cycles) override;
	void write(const word addr, const byte data, const uint64_t cycles) override;
	void update(const uint64_t cycles) override {};
	void reset() override;
	void keyEvent(SDL_KeyboardEvent&);
private:
	byte mMatrix[8]{ 0 };
	bool mCtrl{ false };
	bool mShift{ false };
	byte mKbOut{ 0 };
	bool mKbOutCtrl{ 0 };
};
