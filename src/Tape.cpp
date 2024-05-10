// Copyright (c) 2020 FBLabs
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
CTape::CTape(CBus& bus, CCpu6502& cpu) :
	mCpu(cpu),
	mCyclesNeeded(0)
{
	bus.addDevice(EDevices::TAPE, this);
	bus.registerAddr(EDevices::TAPE, 0xC010, 0xC02F);
	bus.registerAddr(EDevices::TAPE, 0xC052, 0xC053);
	bus.registerAddr(EDevices::TAPE, 0xC056, 0xC057);
	mQueueIt = mQueueCycles.end();
}

/*************************************************************************************************/
byte CTape::read(const word addr, const uint64_t cycles) {
	if ((addr & 0xF0) == 0x10) {
		if (!mPlay) {
			return 0x00;
		}
		const unsigned long long actualCycle = mCpu.getCumulativeCycles();
		if (mStartCycle == 0 && mQueueIt != mQueueCycles.end()) {
			mStartCycle = actualCycle;
			mCyclesNeeded = (*mQueueIt);
		}
		if (actualCycle - mStartCycle > mCyclesNeeded) {
			mStartCycle = actualCycle;
			mCasOut ^= 0x80;
			if (mQueueIt != mQueueCycles.end()) {
				mCyclesNeeded = *(mQueueIt++);
			} else {
				mStartCycle = 0;
				mPlay = false;
				mCpu.setFullSpeed(false);
			}
		}
	}

	return mCasOut;
}

/*************************************************************************************************/
void CTape::reset() {
	mPlay = false;
	mStartCycle = 0;
}

/*************************************************************************************************/
void CTape::play() noexcept {
	if (mQueueIt != mQueueCycles.end()) {
		mPlay = true;
		mCpu.setFullSpeed(true);
	}
}

/*************************************************************************************************/
void CTape::stop() noexcept {
	if (mPlay) {
		mPlay = false;
		mQueueIt = mQueueCycles.end();
	}
}

/*************************************************************************************************/
void CTape::rewind() noexcept {
	mQueueIt = mQueueCycles.begin();
}

/*************************************************************************************************/
bool CTape::getPlayState() const noexcept {
	return mPlay;
}

/*************************************************************************************************/
bool CTape::insertCt2(const char *fileName) {
	FILE *fileTape = fopen(fileName, "rb");
	if (!fileTape) {
		return false;
	}
	dword magic;
	size_t rlen = fread(&magic, 1, sizeof(dword), fileTape);
	if (memcmp(&magic, CT2_MAGIC, sizeof(dword)) != 0) {
		return false;
	}
	mQueueCycles.clear();
	SCh ch;
	while (true) {
		rlen = fread(&ch, 1, sizeof(SCh), fileTape);
		if (feof(fileTape)) {
			break;
		}
		if (memcmp(ch.ID, CT2_CAB_A, sizeof(word)) == 0) {
			for (int i = 0; i < 500; i++) {
				mQueueCycles.emplace_back(500);
				mQueueCycles.emplace_back(500);
			}
		} else if (memcmp(ch.ID, CT2_CAB_B, sizeof(word)) == 0) {
			mQueueCycles.emplace_back(464);
			mQueueCycles.emplace_back(679);
			for (int i = 0; i < 32; i++) {
				mQueueCycles.emplace_back(679);
				mQueueCycles.emplace_back(679);
			}
			mQueueCycles.emplace_back(199);
			mQueueCycles.emplace_back(250);
		} else if (memcmp(ch.ID, CT2_DATA, sizeof(word)) == 0) {
			byte b;
			for (int i = 0; i < ch.size; i++) {
				rlen = fread(&b, 1, 1, fileTape);
				for (int j = 0; j < 8; j++) {
					int mask = 1 << (7 - j);
					if ((mask & b) == mask) {
						// 1
						mQueueCycles.emplace_back(500);
						mQueueCycles.emplace_back(500);
					} else {
						// 0
						mQueueCycles.emplace_back(250);
						mQueueCycles.emplace_back(250);
					}
				}
			}
		}
	}
	mQueueIt = mQueueCycles.begin();	// Update iterator
	return true;
}
