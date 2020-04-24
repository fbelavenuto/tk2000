
#include <cstring>
#include <cassert>
#include "Video.h"


/*****************************************************************************/
void CVideo::drawMono() {
	word memAddr = (mSecondPage ? 0xA000 : 0x2000);
	byte *pnt;
	for (int y = 0; y < VIDEOHEIGHT; y++) {
		int offset = ((y & 7) << 10) + ((y & 0x38) << 4) + (y >> 6) * 40;
		pnt = mRam->mRam + memAddr + offset;
		for (int x = 0; x < VIDEOWIDTH; x += 7) {
			byte v = *pnt++;
			for (int b = 0; b < 7; b++) {
				const int index = y * VIDEOWIDTH + x + b;
				if (v & (1 << b)) {
					mFrameBuffer[index].red = 255;
					mFrameBuffer[index].green = 255;
					mFrameBuffer[index].blue = 255;
				} else {
					mFrameBuffer[index].red = 0;
					mFrameBuffer[index].green = 0;
					mFrameBuffer[index].blue = 0;
				}
			}
		}
	}

}

/*****************************************************************************/
void CVideo::drawColor() {
	word memAddr = (mSecondPage ? 0xA000 : 0x2000);
	byte *ptr;
	bool pixels[VIDEOWIDTH][VIDEOHEIGHT];
	bool colorMod[VIDEOWIDTH / 7][VIDEOHEIGHT];
	for (int y = 0; y < VIDEOHEIGHT; y++) {
		int offset = ((y & 7) << 10) + ((y & 0x38) << 4) + (y >> 6) * 40;
		ptr = mRam->mRam + memAddr + offset;
		for (int x = 0; x < VIDEOWIDTH; x += 7) {
			byte v = *ptr++;
			colorMod[x/7][y] = (v & 0x80) != 0;
			for (int b = 0; b < 7; b++) {
				pixels[x + b][y] = (v & (1 << b)) != 0;
			}
		}
	}

	for (int y = 0; y < VIDEOHEIGHT; y++) {
		for (int x = 0; x < VIDEOWIDTH; x += 2) {
			const int index = y * VIDEOWIDTH + x;
			const bool cm = colorMod[x / 7][y];
			byte actual = pixels[x][y];
			byte next = (x == VIDEOWIDTH - 1) ? 0 : pixels[x+1][y];
			switch (actual << 1 | next) {
			case 0:	// 00 black
				mFrameBuffer[index].red = 0;
				mFrameBuffer[index].green = 0;
				mFrameBuffer[index].blue = 0;
				mFrameBuffer[index + 1].red = 0;
				mFrameBuffer[index + 1].green = 0;
				mFrameBuffer[index + 1].blue = 0;
				break;

			case 1:	// 01 red / blue
				mFrameBuffer[index].red = cm ? 250 : 0;
				mFrameBuffer[index].green = cm ? 16 : 128;
				mFrameBuffer[index].blue = cm ? 0 : 255;
				mFrameBuffer[index + 1].red = cm ? 250 : 0;
				mFrameBuffer[index + 1].green = cm ? 16 : 128;
				mFrameBuffer[index + 1].blue = cm ? 0 : 255;
				break;

			case 2:	// 10 cyan / green
				mFrameBuffer[index].red = cm ? 32 : 32;
				mFrameBuffer[index].green = cm ? 176 : 192;
				mFrameBuffer[index].blue = cm ? 176 : 0;
				mFrameBuffer[index + 1].red = cm ? 32 : 32;
				mFrameBuffer[index + 1].green = cm ? 176 : 192;
				mFrameBuffer[index + 1].blue = cm ? 176 : 0;
				break;

			case 3:	// 11
				mFrameBuffer[index].red = 255;
				mFrameBuffer[index].green = 255;
				mFrameBuffer[index].blue = 255;
				mFrameBuffer[index + 1].red = 255;
				mFrameBuffer[index + 1].green = 255;
				mFrameBuffer[index + 1].blue = 255;
				break;

			}
		}
	}
}


/*****************************************************************************/
CVideo::CVideo(SDL_Renderer *renderer, CBus *bus, CRam *ram) :
	mRenderer(renderer), mRam(ram) {

	assert(mRenderer != nullptr);
	assert(bus != nullptr);
	assert(mRam != nullptr);
	mScreen = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, VIDEOWIDTH, VIDEOHEIGHT);
	if (mScreen == 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error creating screen renderer! SDL Error: %s\n", SDL_GetError());
		throw "Error creating screen renderer";
	}
	bus->addDevice(0xC050, 0xC051, this);
	bus->addDevice(0xC054, 0xC055, this);

}

/*****************************************************************************/
CVideo::~CVideo() {

}

/*****************************************************************************/
void CVideo::render() {
	mVideoMono ? drawMono() : drawColor();
	sRGB *ptr;
	int pitch;
	SDL_LockTexture(mScreen, nullptr, (void **)&ptr, &pitch);
	memcpy(ptr, &mFrameBuffer, VIDEOHEIGHT * pitch);
	SDL_UnlockTexture(mScreen);
	SDL_Rect r{ 0, 0, VIDEOWIDTH * 2, VIDEOHEIGHT * 2 };
	SDL_RenderCopy(mRenderer, mScreen, NULL, &r);
}

/*****************************************************************************/
byte CVideo::read(word addr) {
	switch (addr & 0x00FF) {
	case 0x50:
		mVideoMono = false;
		break;

	case 0x51:
		mVideoMono = true;
		break;

	case 0x54:
		mSecondPage = false;
		break;

	case 0x55:
		mSecondPage = true;
		break;

	}
	return 0xFF;
}

/*****************************************************************************/
void CVideo::write(word addr, byte data) {
	read(addr);
}

/*****************************************************************************/
void CVideo::reset() {
	mVideoMono = false;
	mSecondPage = false;
}