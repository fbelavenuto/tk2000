
#pragma once

class CMenu final {
private:
	SDL_Renderer* mRenderer;
	std::vector<std::string>& mMenuLst;
	SDL_Texture* mFont{};
	SDL_Color mNotSel{ 128, 100, 0, SDL_ALPHA_OPAQUE };
	SDL_Color mSel{ 255, 150, 0, SDL_ALPHA_OPAQUE };
public:
	CMenu(SDL_Renderer* renderer, std::vector<std::string>& menuLst);
	~CMenu();
	void render(int selected);
};
