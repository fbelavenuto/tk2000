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

#include <cassert>
//#include <cstdio>
#include "Bus.h"
#include "Cpu6502.h"


/*************************************************************************************************/
CBus::CBus() {
	for (int i = 0; i < 0x10000; i++) {
		mDevices[i] = nullptr;
		mDevices2[i] = nullptr;
	}
}

/*************************************************************************************************/
CBus::~CBus() {

}

/*************************************************************************************************/
byte CBus::readByte(const word addr) {
	if (mDevices[addr] == nullptr) {
		return 0xFF;
	}
	byte result = mDevices[addr]->read(addr);
	if (mDevices2[addr] != nullptr) {
		result |= mDevices2[addr]->read(addr);
	}
	return result;
}

/*************************************************************************************************/
void CBus::writeByte(const word addr, const byte data) {
	if (mDevices[addr] != nullptr) {
		mDevices[addr]->write(addr, data);
	}
}

/*************************************************************************************************/
word CBus::readWord(const word addr) {
	return readByte(addr) | readByte(addr + 1) << 8;
}

/*************************************************************************************************/
void CBus::writeWord(const word addr, const word data) {
	writeByte(addr, data & 0xFF);
	writeByte(addr + 1, data >> 8);
}

/*************************************************************************************************/
void CBus::addDevice(const word addr, CDevice *dev) {
	if (mDevices[addr] == nullptr) {
		mDevices[addr] = dev;
	} else {
		mDevices2[addr] = dev;
	}
}

/*************************************************************************************************/
void CBus::addDevice(const word addrStart, const word addrEnd, CDevice *dev) {
	assert(addrEnd >= addrStart);
	for (int i = addrStart; i <= addrEnd; i++) {
		if (mDevices[i] == nullptr) {
			mDevices[i] = dev;
		} else {
			mDevices2[i] = dev;
		}
	}
}
