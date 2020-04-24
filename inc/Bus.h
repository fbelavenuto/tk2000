
#pragma once

#include "DataTypes.h"
#include "Device.h"

class CBus {
public:
	CBus();
	byte readByte(word addr);
	void writeByte(word addr, byte data);
	word readWord(word addr);
	void writeWord(word addr, word data);
	void addDevice(word addr, CDevice *dev);
	void addDevice(word addrStart, word addrEnd, CDevice *dev);
private:
	CDevice *mDevices[0x10000];
	CDevice *mDevices2[0x10000];
};
