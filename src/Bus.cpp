

#include <cassert>
#include "Bus.h"


/*************************************************************************************************/
CBus::CBus() {
	for (int i = 0; i < 0x10000; i++) {
		mDevices[i] = nullptr;
	}
}

/*************************************************************************************************/
byte CBus::readByte(unsigned long cycles, word addr) {
	if (mDevices[addr] == nullptr) {
		return 0xFF;
	}
	mDevices[addr]->setCycles(cycles);
	return mDevices[addr]->read(addr);
}

/*************************************************************************************************/
void CBus::writeByte(unsigned long cycles, word addr, byte data) {
	if (mDevices[addr] != nullptr) {
		mDevices[addr]->setCycles(cycles);
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
	mDevices[addr] = dev;
}

/*************************************************************************************************/
void CBus::addDevice(word addrStart, word addrEnd, CDevice *dev) {
	assert(addrEnd >= addrStart);
	for (int i = addrStart; i <= addrEnd; i++) {
		mDevices[i] = dev;
	}
}
