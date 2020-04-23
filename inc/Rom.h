
#pragma once

#include <memory>
#include "DataTypes.h"
#include "Device.h"
#include "Bus.h"

class CRom : public CDevice {
public:
	CRom(CBus *bus);
	~CRom();
	byte read(word addr);
	void write(word addr, byte data);
	void update(unsigned long cycles) {}
	void reset() {}
private:
};
