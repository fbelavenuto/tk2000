

#include <cassert>
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
byte CBus::readByte(unsigned long cycles, word addr) {
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
void CBus::writeByte(unsigned long cycles, word addr, byte data) {
	if (mDevices[addr] != nullptr) {
		mDevices[addr]->write(addr, data);
	}
}

/*************************************************************************************************/
word CBus::readWord(unsigned long cycles, word addr) {
	return readByte(cycles, addr) | readByte(cycles, addr + 1) << 8;
}

/*************************************************************************************************/
void CBus::writeWord(unsigned long cycles, word addr, word data) {
	writeByte(cycles, addr, data & 0xFF);
	writeByte(cycles, addr + 1, data >> 8);
}

/*************************************************************************************************/
void CBus::addDevice(word addr, CDevice *dev) {
	if (mDevices[addr] == nullptr) {
		mDevices[addr] = dev;
	} else {
		mDevices2[addr] = dev;
	}
}

/*************************************************************************************************/
void CBus::addDevice(word addrStart, word addrEnd, CDevice *dev) {
	assert(addrEnd >= addrStart);
	for (int i = addrStart; i <= addrEnd; i++) {
		if (mDevices[i] == nullptr) {
			mDevices[i] = dev;
		} else {
			mDevices2[i] = dev;
		}
	}
}
