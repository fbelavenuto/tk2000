
#pragma once

using TMenuLst = std::vector<std::string>;

class CMenu final {
private:
	const int MAX_ITEMS = 10;
	const int FONT_WIDTH = 16;
	const int FONT_HEIGHT = 32;
	std::string mTitle;
	TMenuLst& mMenuLst;
	SDL_Renderer* mRenderer;
	SDL_Texture* mFont{};
	SDL_Color mTitleColor{ 150, 250, 250, SDL_ALPHA_OPAQUE };
	SDL_Color mNotSelColor{ 180, 110, 0, SDL_ALPHA_OPAQUE };
	SDL_Color mSelColor{ 255, 150, 0, SDL_ALPHA_OPAQUE };
	int mSelected = 0;
	int mFirst = 0;
public:
	CMenu(TMenuLst& menuLst, SDL_Renderer* renderer, std::string title);
	~CMenu();
	std::string& getTitle();
	TMenuLst& getMenuLst() const;
	void setMenuLst(TMenuLst& lst);
	int getSelected() const;
	void render();
	void up();
	void down();
};
