
#include "pch.h"
#include "Menu.h"

/*****************************************************************************/
CMenu::CMenu(SDL_Renderer* renderer, std::vector<std::string>& menuLst) :
	mRenderer(renderer),
	mMenuLst(menuLst)
{
	SDL_Surface* font = SDL_LoadBMP("..\\data\\16x32.bmp");
	if (!font) {
		std::string error{ "Error loading font file! SDL Error: " };
		error.append(SDL_GetError());
		throw std::runtime_error(error);
	}
	SDL_SetColorKey(font, SDL_TRUE, SDL_MapRGB(font->format, 255, 0, 255));
	mFont = SDL_CreateTextureFromSurface(mRenderer, font);
	if (!mFont) {
		std::string error{ "Error creating font texture! SDL Error: " };
		error.append(SDL_GetError());
		throw std::runtime_error(error);
	}
}

/*****************************************************************************/
CMenu::~CMenu() {
	SDL_DestroyTexture(mFont);
}

/*****************************************************************************/
void CMenu::render(int selected) {
	int w, h;
	SDL_GetRendererOutputSize(mRenderer, &w, &h);
	int menuLen = (int)mMenuLst.size();
	int yPos = (h - menuLen * 32) / 2;
	int y = 0;
	for (auto& item : mMenuLst) {
		int len = (int)item.size();
		int xPos = (w - len * 16) / 2;
		for (int x = 0; x < len; x++) {
			const char c = item[x];
			SDL_Rect textRect{};
			textRect.w = 16;
			textRect.h = 32;
			textRect.x = (c % 16) * textRect.w;
			textRect.y = (c / 16) * textRect.h;
			SDL_Rect destRect{};
			destRect.w = 16;
			destRect.h = 32;
			destRect.x = xPos + x * destRect.w;
			destRect.y = yPos + y * destRect.h;
			SDL_RenderFillRect(mRenderer, &destRect);
			SDL_SetTextureBlendMode(mFont, SDL_BLENDMODE_BLEND);
			SDL_Color* color = (y == selected) ? &mSel : &mNotSel;
			SDL_SetTextureColorMod(mFont, color->r, color->g, color->b);
			SDL_RenderCopy(mRenderer, mFont, &textRect, &destRect);
		}
		++y;
	}
}
