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

#pragma once

#include "DataTypes.h"
#include "Device.h"
#include "Bus.h"
#include "Ram.h"

class CVideo : public CDevice {
public:
	CVideo(SDL_Renderer *renderer, CBus *bus, CRam *ram);
	~CVideo() override;
	byte read(const word addr) override;
	void write(const word addr, const byte data) override;
	void update() override {}
	void reset() override;
	//
	void render();
	void setScanline(const bool val) noexcept;
	bool getScanline() const noexcept;
private:
	struct sRGB {
		byte red;
		byte green;
		byte blue;
	};

	SDL_Renderer *mRenderer{};
	SDL_Texture *mScreen{};
	CRam *mRam{};
	bool mVideoMono{ false };
	bool mSecondPage{ false };
	bool mScanLines{ false };
	sRGB mFrameBuffer[VIDEOWIDTH * VIDEOHEIGHT * 2];
	void drawMono();
	void drawColor();
};
