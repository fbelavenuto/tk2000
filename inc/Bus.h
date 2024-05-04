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

#include "Common.h"
#include "Device.h"

class CBus final {
public:
	CBus();
	~CBus();
	byte readByte(const word addr);
	void writeByte(const word addr, byte data);
	word readWord(const word addr);
	void writeWord(const word addr, word data);
	void addDevice(const char* name, CDevice* dev);
	CDevice* getDevice(const char* name);
	void registerAddr(const char* name, const word addr);
	void registerAddr(const char* name, const word addrStart, const word addrEnd);
	void resetAll();
	void updateAll();
private:
	CDevice *mDevices[0x10000][2];
	std::map<const char*, CDevice*> mMapDevices;
};

using TBus = std::shared_ptr<CBus>;
