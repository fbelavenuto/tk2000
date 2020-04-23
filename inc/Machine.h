
#pragma once

#include <memory>
#include <SDL2/SDL.h>
#include "DataTypes.h"
#include "Bus.h"
#include "Cpu6502.h"
#include "Video.h"
#include "Ram.h"
#include "Rom.h"
#include "Audio.h"

/*************************************************************************************************/
class CMachine {
public:
	CMachine();
	~CMachine();
	bool init();
	bool loop();
private:
	SDL_Window *mWindow = nullptr;
	SDL_Renderer *mRenderer = nullptr;
	std::unique_ptr<CCpu6502> mCpu = nullptr;
	std::unique_ptr<CVideo> mVideo = nullptr;
	std::unique_ptr<CBus> mBus = nullptr;
	std::unique_ptr<CRam> mRam = nullptr;
	std::unique_ptr<CRom> mRom = nullptr;
	std::unique_ptr<CAudio> mAudio = nullptr;
};
