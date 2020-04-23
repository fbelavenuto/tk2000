
#include "Ram.h"

/*************************************************************************************************/
CRam::CRam(CBus *bus) {
	memset(mRam, 0, 65536);
	memset(mRam + 0x2000, 0xFF, 0x1000); // Simula sujeira na tela
	memset(mRam + 0xA000, 0xFF, 0x1000); // Simula sujeira na tela
	bus->addDevice(0x0000, 0xBFFF, this);
}

/*************************************************************************************************/
CRam::~CRam() {
}

/*************************************************************************************************/
byte CRam::read(word addr) {
	return mRam[addr];
}

/*************************************************************************************************/
void CRam::write(word addr, byte data) {
	mRam[addr] = data;
}