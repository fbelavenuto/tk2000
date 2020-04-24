
#pragma once

#include "DataTypes.h"
#include "Device.h"

class CBus {
public:
	CBus();
	byte readByte(unsigned long cycles, word addr);
	void writeByte(unsigned long cycles, word addr, byte data);
	word readWord(unsigned long cycles, word addr);
	void writeWord(unsigned long cycles, word addr, word data);
	void addDevice(word addr, CDevice *dev);
	void addDevice(word addrStart, word addrEnd, CDevice *dev);
private:
	CDevice *mDevices[0x10000];
	CDevice *mDevices2[0x10000];
};
