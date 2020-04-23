
#include <cstdio>
#include "Audio.h"

/*************************************************************************************************/
CAudio::CAudio(CBus *bus) {
	bus->addDevice(0xC030, 0xC03F, this);
}

/*************************************************************************************************/
byte CAudio::read(word addr) {
	fprintf(stderr, "Audio access at %d\n", mCycles);
	return 0xFF;
}

/*************************************************************************************************/
void CAudio::write(word addr, byte data) {
	read(addr);
}

/*************************************************************************************************/
void CAudio::update(unsigned long cycles) {
	fprintf(stderr, "Audio update at %d\n", cycles);
}

/*************************************************************************************************/
void CAudio::reset() {

}
