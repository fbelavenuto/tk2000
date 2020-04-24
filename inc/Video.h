
#pragma once

#include <SDL2/SDL.h>
#include "DataTypes.h"
#include "Device.h"
#include "Bus.h"
#include "Ram.h"

class CVideo : public CDevice {
public:
	CVideo(SDL_Renderer *renderer, CBus *bus, CRam *ram);
	~CVideo();
	byte read(word addr);
	void write(word addr, byte data);
	void update() {}
	void reset();
	void render();
private:
	struct sRGB {
		byte red;
		byte green;
		byte blue;

		bool isEqual(sRGB other) const {
			return red == other.red && green == other.green && blue == other.blue;
		}
	};

	SDL_Renderer *mRenderer = nullptr;
	SDL_Texture *mScreen = nullptr;
	CRam *mRam = nullptr;
	bool mVideoMono = false;
	bool mSecondPage = false;
	sRGB mFrameBuffer[VIDEOWIDTH * VIDEOHEIGHT];
	void drawMono();
	void drawColor();
};
