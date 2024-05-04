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
#include "Audio.h"


/*************************************************************************************************/
void CAudio::makeSamples() {
	unsigned long long cycleDiff = mCpu->getCumulativeCycles() - mLastCycle;
	unsigned int numSamples = (int)((double)cycleDiff / CLOCKSPERSAMPLE);
	unsigned int nCyclesRemaining = (int)((double)cycleDiff - (double)numSamples * CLOCKSPERSAMPLE);

	while (numSamples-- && mPos < NUMSAMPLESPERUPDATE) {
		mBuffer[mPos++] = mMuted ? 0 : mSoundPos;
	}
	mLastCycle = mCpu->getCumulativeCycles() - nCyclesRemaining;
}

/*************************************************************************************************/
CAudio::CAudio(TBus bus, TCpu cpu) : 
	mCpu(cpu),
	mBuffer()
{
	assert(bus != nullptr);
	assert(mCpu != nullptr);

	bus->addDevice("audio", this);
	bus->registerAddr("audio", 0xC030, 0xC03F);
}

/*************************************************************************************************/
CAudio::~CAudio() {
}

/*************************************************************************************************/
byte CAudio::read(const word addr) {
	if (mCpu->getClockRate() != 1.0) {
		return 0xFF;
	}
	mSpeakToggled = true;
	mMuted = false;
	makeSamples();
	mSoundPos = -mSoundPos;
	return 0xFF;
}

/*************************************************************************************************/
void CAudio::write(word addr, byte data) {
	read(addr);
}

/*************************************************************************************************/
void CAudio::update() {
	auto actualCycle = mCpu->getCumulativeCycles();
	if (!mSpeakToggled) {
		if (!mQuietCycle) {
			mQuietCycle = mCpu->getCumulativeCycles();
		} else if (actualCycle - mQuietCycle > CYCLESIN50US) {
			// After 50us, mute audio
			mMuted = true;
		}
	} else {
		mSpeakToggled = false;
		mQuietCycle = 0;
	}

	makeSamples();
	if (mCpu->getClockRate() == 1.0) {
		sAudioMsg m = { mBuffer, mPos };
		notify(&m);
	}
	mPos = 0;
}

/*************************************************************************************************/
void CAudio::reset() {
	mLastCycle = 0;
}

/*************************************************************************************************/
int CAudio::getSampleRate() const {
	return mSampleRate;
}
