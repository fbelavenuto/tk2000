
#pragma once

#include <memory>
#include "DataTypes.h"
#include "Device.h"
#include "Bus.h"

class CRam : public CDevice {
public:
	CRam(CBus *bus);
	~CRam();
	byte read(word addr);
	void write(word addr, byte data);
	void reset() {}
	void update() {}
	friend class CVideo;
private:
	byte mRam[0x10000];
};
