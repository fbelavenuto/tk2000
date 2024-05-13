// Copyright (c) 2024 FBLabs
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

#include "pch.h"
#include "Bus.h"
#include "Cpu6502.h"


/*************************************************************************************************/
CBus::CBus() {
	for (int i = 0; i < 0x10000; i++) {
		mDevices[i][0] = nullptr;
		mDevices[i][1] = nullptr;
	}
}

/*************************************************************************************************/
byte CBus::readByte(const word addr, const uint64_t cycles) {
	if (mDevices[addr][0] == nullptr) {
		return 0xFF;
	}
	byte result = mDevices[addr][0]->read(addr, cycles);
	if (mDevices[addr][1] != nullptr) {
		result |= mDevices[addr][1]->read(addr, cycles);
	}
	return result;
}

/*************************************************************************************************/
void CBus::writeByte(const word addr, const byte data, const uint64_t cycles) {
	if (mDevices[addr][0] != nullptr) {
		mDevices[addr][0]->write(addr, data, cycles);
	}
	if (mDevices[addr][1] != nullptr) {
		mDevices[addr][1]->write(addr, data, cycles);
	}
}

/*************************************************************************************************/
word CBus::readWord(const word addr, const uint64_t cycles) {
	return readByte(addr, cycles) | readByte(addr + 1, cycles) << 8;
}

/*************************************************************************************************/
void CBus::writeWord(const word addr, const word data, const uint64_t cycles) {
	writeByte(addr, data & 0xFF, cycles);
	writeByte(addr + 1, data >> 8, cycles);
}

/*************************************************************************************************/
void CBus::addDevice(EDevices t, CDevice* dev) {
	assert(dev != nullptr);
	mMapDevices.emplace(t, dev);
}

/*************************************************************************************************/
CDevice* CBus::getDevice(EDevices dev) {
	return mMapDevices.at(dev);
}

/*************************************************************************************************/
void CBus::registerAddr(EDevices dev, const word addr) {
	if (mDevices[addr][0] == nullptr) {
		mDevices[addr][0] = getDevice(dev);
	} else {
		mDevices[addr][1] = getDevice(dev);
	}
}

/*************************************************************************************************/
void CBus::registerAddr(EDevices dev, const word addrStart, const word addrEnd) {
		assert(addrEnd >= addrStart);
	for (int i = addrStart; i <= addrEnd; i++) {
		if (mDevices[i][0] == nullptr) {
			mDevices[i][0] = getDevice(dev);
		} else {
			mDevices[i][1] = getDevice(dev);
		}
	}
}

/*************************************************************************************************/
void CBus::resetAll() {
	for (auto& [name, dev] : mMapDevices) {
		dev->reset();
	}
}

/*************************************************************************************************/
void CBus::updateAll(const uint64_t cycles) {
	for (auto& [name, dev] : mMapDevices) {
		dev->update(cycles);
	}
}
