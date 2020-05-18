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
#include "AudioSDL.h"

#if 0
// Structs
#pragma pack(push, 1)
typedef struct SWaveCab {
	unsigned char  groupID[4];		// RIFF
	unsigned int   groupLength;
	unsigned char  typeID[4];		// WAVE
	unsigned char  formatID[4];		// fmt
	unsigned int   formatLength;
	unsigned short wFormatTag;
	unsigned short numChannels;
	unsigned int   samplesPerSec;
	unsigned int   bytesPerSec;
	unsigned short nBlockAlign;
	unsigned short bitsPerSample;
	unsigned char  dataID[4];
	unsigned int   dataLength;
} TWaveCab, *PTWaveCab;
#pragma pack(pop)

static FILE          *fileWav = NULL;
static TWaveCab      waveCab;
static unsigned int  dataSize = 0;

/*****************************************************************************/
int createWaveFile(const char *filename) {
	size_t s = 0;

	if (!(fileWav = fopen(filename, "wb"))) {
		return 0;
	}
	memset(&waveCab, 0, sizeof(TWaveCab));

	memcpy((char *)waveCab.groupID, "RIFF", 4);
	waveCab.groupLength = 0;					// Fill after
	memcpy((char *)waveCab.typeID, "WAVE", 4);
	memcpy((char *)waveCab.formatID, "fmt ", 4);
	waveCab.formatLength = 16;
	waveCab.wFormatTag = 1;
	waveCab.numChannels = 1;
	waveCab.samplesPerSec = SAMPLERATE;
	waveCab.bytesPerSec = SAMPLERATE * 1 * (16 / 8);
	waveCab.nBlockAlign = 1 * (16 / 8);
	waveCab.bitsPerSample = 16;
	memcpy((char *)waveCab.dataID, "data", 4);
	waveCab.dataLength = 0;					// Fill after
	s = fwrite(&waveCab, 1, sizeof(TWaveCab), fileWav);
	if (s != sizeof(TWaveCab)) {
		return 0;
	}
	dataSize = 0;
	return 1;
}

/*****************************************************************************/
void writeToWaveFile(unsigned char* ptr, unsigned int size) {
	fwrite(ptr, 1, size, fileWav);
	dataSize += size;
}

/*****************************************************************************/
int finishWaveFile() {
	size_t s = 0;
	// Fornecer dados faltantes do cabeçalho
	waveCab.dataLength = dataSize;
	waveCab.groupLength = dataSize + sizeof(TWaveCab) - 8;
	if (fseek(fileWav, 0, SEEK_SET)) {
		return 1;
	}
	s = fwrite(&waveCab, 1, sizeof(TWaveCab), fileWav);
	fclose(fileWav);
	if (s != sizeof(TWaveCab)) {
		return 1;
	}
	return 0;
}
#endif


/*************************************************************************************************/
CAudioSDL::CAudioSDL() {
	//Default audio spec
	SDL_AudioSpec receivedPlaybackSpec;
	SDL_AudioSpec desiredPlaybackSpec;

	SDL_zero(desiredPlaybackSpec);
	desiredPlaybackSpec.freq = SAMPLERATE;
	desiredPlaybackSpec.format = AUDIO_S16SYS;
	desiredPlaybackSpec.channels = 1;
	desiredPlaybackSpec.samples = (uint16_t)BUFSIZE;
	desiredPlaybackSpec.userdata = this;
	desiredPlaybackSpec.callback = cbFillBuffer;

	//Open playback device
	mPlayDevId = SDL_OpenAudioDevice(NULL, SDL_FALSE, &desiredPlaybackSpec, &receivedPlaybackSpec, 0);
	if (mPlayDevId == 0) {
		std::string error{ "Error opening audio device! SDL Error: " };
		error.append(SDL_GetError());
		throw std::runtime_error(error);
	}
	mIdxWriteBuffer = 0;
	mWritePos = 0;
	mIdxReadBuffer = 0;
	mAllBuffers = new int16_t[(long)BUFSIZE * BUFCOUNT];
	if (!mAllBuffers) {
		throw std::runtime_error("Out of memory");
	}

	mSemaphore = SDL_CreateSemaphore(BUFCOUNT - 1);
	if (!mSemaphore) {
		std::string error{ "Error creating Semaphore! SDL Error: " };
		error.append(SDL_GetError());
		throw std::runtime_error(error);
	}
	SDL_PauseAudioDevice(mPlayDevId, SDL_FALSE);
	//createWaveFile("z.wav");
}

/*************************************************************************************************/
CAudioSDL::~CAudioSDL() {
	SDL_PauseAudioDevice(mPlayDevId, SDL_TRUE);
	SDL_CloseAudioDevice(mPlayDevId);
	SDL_DestroySemaphore(mSemaphore);
	delete[] mAllBuffers;
	//finishWaveFile();
}

/*************************************************************************************************/
inline int16_t* CAudioSDL::getBufferPtr(int index) {
	assert(index < BUFCOUNT);
	return mAllBuffers + (long)index * BUFSIZE;
}

/*************************************************************************************************/
void CAudioSDL::write(const int16_t* in, unsigned long count) {
	while (count) {
		unsigned long n = BUFSIZE - mWritePos;
		if (n > count)
			n = count;

		memcpy(getBufferPtr(mIdxWriteBuffer) + mWritePos, in, n * sizeof(int16_t));
		in += n;
		mWritePos += n;
		count -= n;

		if (mWritePos >= BUFSIZE) {
			mWritePos = 0;
			mIdxWriteBuffer = (mIdxWriteBuffer + 1) % BUFCOUNT;
			SDL_SemWait(mSemaphore);
		}
	}
}

/*************************************************************************************************/
void CAudioSDL::fillBuffer(void* out, int count) {
	if (SDL_SemValue(mSemaphore) < (uint32_t)(BUFCOUNT - 1))	{
		int16_t* p = getBufferPtr(mIdxReadBuffer);
		//writeToWaveFile((unsigned char*)p, count);
		memcpy(out, p, count);
		mIdxReadBuffer = (mIdxReadBuffer + 1) % BUFCOUNT;
		SDL_SemPost(mSemaphore);
	} else {
		memset(out, 0, count);
	}
}

/*************************************************************************************************/
void CAudioSDL::cbFillBuffer(void* userdata, uint8_t* out, int count) {
	auto obj = reinterpret_cast<CAudioSDL *>(userdata);
	obj->fillBuffer(out, count);
}