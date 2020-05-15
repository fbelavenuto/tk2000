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
#include "Video.h"

/*****************************************************************************/
void CVideo::drawMono() {
	word memAddr = (mSecondPage ? 0xA000 : 0x2000);
	byte *pnt;
	for (int y = 0; y < VIDEOHEIGHT; y++) {
		int offset = ((y & 7) << 10) + ((y & 0x38) << 4) + (y >> 6) * 40;
		pnt = mRamPtr + memAddr + offset;
		for (int x = 0; x < VIDEOWIDTH; x += 7) {
			byte v = *pnt++;
			for (int b = 0; b < 7; b++) {
				const int index = y * 2 * VIDEOWIDTH + x + b;
				const int index2 = index + VIDEOWIDTH;
				if (v & (1 << b)) {
					mFrameBuffer[index].red = 255;
					mFrameBuffer[index].green = 255;
					mFrameBuffer[index].blue = 255;
				} else {
					mFrameBuffer[index].red = 0;
					mFrameBuffer[index].green = 0;
					mFrameBuffer[index].blue = 0;
				}
				if (!mScanLines) {
					mFrameBuffer[index2] = mFrameBuffer[index];
					mFrameBuffer[index2 + 1] = mFrameBuffer[index + 1];
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
		ptr = mRamPtr + memAddr + offset;
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
			const int index = y * 2 * VIDEOWIDTH + x;
			const int index2 = index + VIDEOWIDTH;
			const bool cm = colorMod[x / 7][y];
			//bool prev = (x == 0 ? false : pixels[x - 1][y]);
			byte actual = (pixels[x][y] ? 2 : 0) | (pixels[x + 1][y] ? 1 : 0);
			//bool next = (x == VIDEOWIDTH - 2) ? false : pixels[x - 2][y];
			switch (actual) {
			case 0:	// 00 black
				mFrameBuffer[index].red = 0;
				mFrameBuffer[index].green = 0;
				mFrameBuffer[index].blue = 0;
				mFrameBuffer[index + 1].red = 0;
				mFrameBuffer[index + 1].green = 0;
				mFrameBuffer[index + 1].blue = 0;
				break;

			case 1:	// 01 red / blue
				/*if (next) {
					mFrameBuffer[index + 1].red = 255;
					mFrameBuffer[index + 1].green = 255;
					mFrameBuffer[index + 1].blue = 255;
				} else*/ {
					mFrameBuffer[index].red = cm ? 250 : 0;
					mFrameBuffer[index].green = cm ? 16 : 128;
					mFrameBuffer[index].blue = cm ? 0 : 255;
					mFrameBuffer[index + 1].red = cm ? 250 : 0;
					mFrameBuffer[index + 1].green = cm ? 16 : 128;
					mFrameBuffer[index + 1].blue = cm ? 0 : 255;
				}
				break;

			case 2:	// 10 cyan / green
				/*if (prev) {
					mFrameBuffer[index].red = 255;
					mFrameBuffer[index].green = 255;
					mFrameBuffer[index].blue = 255;
				} else */{
					mFrameBuffer[index].red = cm ? 32 : 32;
					mFrameBuffer[index].green = cm ? 176 : 192;
					mFrameBuffer[index].blue = cm ? 176 : 0;
					mFrameBuffer[index + 1].red = cm ? 32 : 32;
					mFrameBuffer[index + 1].green = cm ? 176 : 192;
					mFrameBuffer[index + 1].blue = cm ? 176 : 0;
				}
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
			if (!mScanLines) {
				mFrameBuffer[index2] = mFrameBuffer[index];
				mFrameBuffer[index2 + 1] = mFrameBuffer[index + 1];
			}
		}
	}
}


/*****************************************************************************/
CVideo::CVideo(CBus *bus, SDL_Renderer *renderer, byte* ramPtr) :
	mRenderer(renderer), mRamPtr(ramPtr) {

	assert(mRenderer != nullptr);
	assert(bus != nullptr);
	assert(ramPtr != nullptr);
	mScreen = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, VIDEOWIDTH, VIDEOHEIGHT * 2);
	if (mScreen == 0) {
		std::string error{ "Error creating screen texture! SDL Error: " };
		error.append(SDL_GetError());
		throw std::runtime_error(error);
	}
	bus->addDevice("video", this);
	bus->registerAddr("video", 0xC050, 0xC051);
	bus->registerAddr("video", 0xC054, 0xC055);
}

/*****************************************************************************/
CVideo::~CVideo() {
	SDL_DestroyTexture(mScreen);
}

/*****************************************************************************/
void CVideo::render() {
	memset(mFrameBuffer, 0, sizeof(mFrameBuffer));
	mVideoMono ? drawMono() : drawColor();
	sRGB *ptr;
	int pitch;
	SDL_LockTexture(mScreen, nullptr, (void **)&ptr, &pitch);
	memcpy(ptr, &mFrameBuffer, VIDEOHEIGHT * 2 * pitch);
	SDL_UnlockTexture(mScreen);
	int w, h;
	SDL_GetRendererOutputSize(mRenderer, &w, &h);
	SDL_Rect r{ 0, 0, w, h };
	SDL_RenderCopy(mRenderer, mScreen, NULL, &r);
}

/*****************************************************************************/
byte CVideo::read(const word addr) {
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
void CVideo::write(const word addr, const byte data) {
	read(addr);
}

/*****************************************************************************/
void CVideo::reset() {
	mVideoMono = false;
	mSecondPage = false;
}

/*****************************************************************************/
void CVideo::setScanline(bool val) noexcept {
	mScanLines = val;
}

/*****************************************************************************/
bool CVideo::getScanline() const noexcept {
	return mScanLines;
}
