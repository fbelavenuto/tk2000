
#include <cassert>
#include <cstdio>
#include <cstring>
#include "Tape.h"

#define CT2_MAGIC "CTK2"
#define CT2_CAB_A "CA"
#define CT2_CAB_B "CB"
#define CT2_DATA  "DA"

#pragma pack(push, 1)
struct SCh {
	byte ID[2];
	word size;
};
#pragma pack(pop)

/*************************************************************************************************/
void CTape::loadCt2(const char *fileName) {
	FILE *fileTape = fopen(fileName, "rb");
	if (!fileTape) {
		return;
	}
	size_t pos = 0;
	dword magic;
	fread(&magic, 1, sizeof(dword), fileTape);
	if (memcmp(&magic, CT2_MAGIC, sizeof(CT2_MAGIC) != 0)) {
		return;
	}
	SCh ch;
	while (true) {
		fread(&ch, 1, sizeof(SCh), fileTape);
		if (feof(fileTape)) {
			break;
		}
		if (memcmp(ch.ID, CT2_CAB_A, sizeof(word)) == 0) {
			for (int i = 0; i < 500; i++) {
				mQueueCycles.emplace(500);
				mQueueCycles.emplace(500);
			}
		} else if (memcmp(ch.ID, CT2_CAB_B, sizeof(word)) == 0) {
			mQueueCycles.emplace(464);
			mQueueCycles.emplace(679);
			for (int i = 0; i < 32; i++) {
				mQueueCycles.emplace(679);
				mQueueCycles.emplace(679);
			}
			mQueueCycles.emplace(199);
			mQueueCycles.emplace(250);
		} else if (memcmp(ch.ID, CT2_DATA, sizeof(word)) == 0) {
			byte b;
			for (int i = 0; i < ch.size; i++) {
				fread(&b, 1, 1, fileTape);
				for (int j = 0; j < 8; j++) {
					int mask = 1 << (7 - j);
					if ((mask & b) == mask) {
						// 1
						mQueueCycles.emplace(500);
						mQueueCycles.emplace(500);
					} else {
						// 0
						mQueueCycles.emplace(250);
						mQueueCycles.emplace(250);
					}
				}
			}
		}
	}
}

/*************************************************************************************************/
CTape::CTape(CBus *bus, CCpu6502 *cpu) : mCpu(cpu) {
	assert(bus != nullptr);
	assert(mCpu != nullptr);

	bus->addDevice(0xC010, 0xC01F, this);
	bus->addDevice(0xC020, 0xC02F, this);
	bus->addDevice(0xC052, 0xC053, this);
	bus->addDevice(0xC056, 0xC057, this);
}

/*************************************************************************************************/
byte CTape::read(word addr) {
	if ((addr & 0xF0) == 0x10) {
		if (!mPlay) {
			return 0x00;
		}
		unsigned long long actualCycle = mCpu->getCumulativeCycles();
		if (mStartCycle == 0 && mQueueCycles.size() > 0) {
			mStartCycle = actualCycle;
			mCyclesNeeded = mQueueCycles.front();
			mQueueCycles.pop();
		}
		if (actualCycle - mStartCycle > mCyclesNeeded) {
			mStartCycle = actualCycle;
			mCasOut ^= 0x80;
			if (mQueueCycles.size() > 0) {
				mCyclesNeeded = mQueueCycles.front();
				mQueueCycles.pop();
			} else {
				mStartCycle = 0;
				mPlay = false;
				mCpu->setFullSpeed(false);
			}
		}
	}

	return mCasOut;
}

/*************************************************************************************************/
void CTape::write(word addr, byte data) {
}

/*************************************************************************************************/
void CTape::update() {

}

/*************************************************************************************************/
void CTape::reset() {
	mPlay = false;
	mStartCycle = 0;
}

/*************************************************************************************************/
void CTape::play() {
	loadCt2("../data/Corrida.ct2");
	mPlay = true;
	mCpu->setFullSpeed(true);
}

/*************************************************************************************************/
bool CTape::getPlayState() {
	return mPlay;
}
