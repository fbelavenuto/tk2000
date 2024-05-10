
#include "pch.h"
#include "Menu.h"
#include "media.h"

/*****************************************************************************/
CMenu::CMenu(TMenuLst& menuLst, SDL_Renderer* renderer, std::string title) :
	mMenuLst(menuLst),
	mRenderer(renderer),
	mTitle(title)
{
	SDL_Surface* font = SDL_LoadBMP_RW(SDL_RWFromConstMem(fontBMP, sizeof(fontBMP)), true);
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
std::string& CMenu::getTitle() {
	return mTitle;
}

/*****************************************************************************/
TMenuLst& CMenu::getMenuLst() const {
	return mMenuLst;
}

/*****************************************************************************/
void CMenu::setMenuLst(TMenuLst& lst) {
	mMenuLst = lst;
	mSelected = 0;
	mFirst = 0;
}

/*****************************************************************************/
int CMenu::getSelected() const {
	return mSelected;
}

/*****************************************************************************/
void CMenu::render() {
	int w, h;
	SDL_GetRendererOutputSize(mRenderer, &w, &h);
	int max = std::min((int)mMenuLst.size(), MAX_ITEMS);
	int yPos = (h - (max-1) * FONT_HEIGHT) / 2;		// -1 to skip title
	if (mSelected > mFirst + max) {
		mSelected = mFirst;
	}
	auto renderText = [&](std::string& str, int top, SDL_Color& color) {
		int len = (int)str.size();
		int xPos = (w - len * FONT_WIDTH) / 2;
		if (xPos < 0) {
			xPos = w - len * FONT_WIDTH;		// Align to right
		}
		for (int x = 0; x < len; x++) {
			const char c = str[x];
			SDL_Rect fontRect{};
			fontRect.w = FONT_WIDTH;
			fontRect.h = FONT_HEIGHT;
			fontRect.x = (c % FONT_CHARS_PER_LINE) * fontRect.w;
			fontRect.y = (c / FONT_CHARS_PER_LINE) * fontRect.h;
			SDL_Rect destRect{};
			destRect.w = FONT_WIDTH;
			destRect.h = FONT_HEIGHT;
			destRect.x = xPos + x * destRect.w;
			destRect.y = top;
			SDL_RenderFillRect(mRenderer, &destRect);
			SDL_SetTextureBlendMode(mFont, SDL_BLENDMODE_BLEND);
			SDL_SetTextureColorMod(mFont, color.r, color.g, color.b);
			SDL_RenderCopy(mRenderer, mFont, &fontRect, &destRect);
		}

	};
	renderText(mTitle, 10, mTitleColor);
	for (int y = 0; y < max; y++) {
		std::string& str = mMenuLst[mFirst + y];
		SDL_Color& color = (mFirst + y == mSelected) ? mSelColor : mNotSelColor;
		renderText(str, yPos + y * FONT_HEIGHT, color);
	}
}

/*****************************************************************************/
void CMenu::up() {
	if (mSelected > 0) {
		--mSelected;
		if (mSelected < mFirst) {
			--mFirst;
		}
	}
}

/*****************************************************************************/
void CMenu::down() {
	int max = std::min((int)mMenuLst.size(), MAX_ITEMS);
	if (mSelected < mMenuLst.size() - 1) {
		++mSelected;
		if (mSelected >= mFirst + max) {
			++mFirst;
		}
	}
}
