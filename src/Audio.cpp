
#include <cstdio>
#include <cstring>
#include <cassert>
#include "Audio.h"

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
static unsigned int  sampleRate = 44100;
static TWaveCab      waveCab;
static size_t size = 0;


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
	waveCab.wFormatTag = 0x0001;
	waveCab.numChannels = 1;
	waveCab.samplesPerSec = sampleRate;
	waveCab.bytesPerSec = sampleRate * 1 * (16 / 8);
	waveCab.nBlockAlign = 1 * (16 / 8);
	waveCab.bitsPerSample = 16;
	memcpy((char *)waveCab.dataID, "data", 4);
	waveCab.dataLength = 0;					// Fill after
	s = fwrite(&waveCab, 1, sizeof(TWaveCab), fileWav);
	if (s != sizeof(TWaveCab)) {
		return 0;
	}
	return 1;
}

/*****************************************************************************/
int finishWaveFile(size_t dataSize) {
	size_t s = 0;
	// Fornecer dados faltantes do cabeçalho
	waveCab.dataLength = (unsigned int)dataSize;
	waveCab.groupLength = (unsigned int)dataSize + sizeof(TWaveCab) - 8;
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
void CAudio::playCallback(void *userdata, uint8_t * stream, int len) {
	static short soundPos = -32767;
	auto obj = reinterpret_cast<CAudio *>(userdata);
	int bufferLen = len / sizeof(uint16_t);
	int16_t *buffer = (int16_t *)stream;
	memset(stream, 0, len);
	const std::lock_guard<std::mutex> lock(obj->myMutex);
	int distance = (obj->mWritePos >= obj->mReadPos) ? obj->mWritePos - obj->mReadPos : BUFFERSIZE - obj->mReadPos + obj->mWritePos;
	if (distance > bufferLen) {
		memcpy(stream, &obj->mBuffer[obj->mReadPos], len);
		obj->mReadPos += bufferLen;
		if (obj->mReadPos >= BUFFERSIZE) { 
			obj->mReadPos -= BUFFERSIZE;
		}
	}
}

/*************************************************************************************************/
void CAudio::updateAudio() {
	unsigned long long duration = (mCumulativeCycles + mCycles) - mLastCycle;
	mLastCycle = mCumulativeCycles + mCycles;
	int amplitude = (mSoundFlag ? 32767 : -32767);
	int samples = (dword)((double)duration / mClocksPerSample);
	const std::lock_guard<std::mutex> lock(myMutex);
	for (int i = 0; i < samples; i++) {
		mBuffer[mWritePos++] = amplitude;
		if (mWritePos == BUFFERSIZE) {
			mWritePos = 0;
		}
		//assert(mWritePos != mReadPos);
	}
}

/*************************************************************************************************/
CAudio::CAudio(CBus *bus) {
	assert(bus != nullptr);

	//Default audio spec
	SDL_AudioSpec receivedPlaybackSpec;
	SDL_AudioSpec desiredPlaybackSpec;

	SDL_zero(desiredPlaybackSpec);
	desiredPlaybackSpec.freq = SAMPLERATE;
	desiredPlaybackSpec.format = AUDIO_S16SYS;
	desiredPlaybackSpec.channels = 1;
	desiredPlaybackSpec.samples = 735;
	desiredPlaybackSpec.userdata = this;
	desiredPlaybackSpec.callback = playCallback;

	//Open playback device
	mPlayDevId = SDL_OpenAudioDevice(NULL, SDL_FALSE, &desiredPlaybackSpec, &receivedPlaybackSpec, 0);
	if (mPlayDevId == 0) {
		throw "Error initializing audio";
	}

	bus->addDevice(0xC030, 0xC03F, this);
}

/*************************************************************************************************/
byte CAudio::read(word addr) {
	if (mLastCycle == 0) {
		mLastCycle = mCycles;
	}
	updateAudio();
	mSoundFlag = !mSoundFlag;
	return 0xFF;
}

/*************************************************************************************************/
void CAudio::write(word addr, byte data) {
	read(addr);
}

/*************************************************************************************************/
void CAudio::update(unsigned long cycles) {
	mCycles = 0;
	mCumulativeCycles += cycles;
	updateAudio();
}

/*************************************************************************************************/
void CAudio::reset() {
	mCumulativeCycles = 0;
	//SDL_PauseAudioDevice(mPlayDevId, SDL_TRUE);
	SDL_PauseAudioDevice(mPlayDevId, SDL_FALSE);
}

