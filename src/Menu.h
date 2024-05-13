// Copyright (c) 2024 FBLabs
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

using TMenuLst = std::vector<std::string>;

class CMenu final {
private:
	const int FONT_WIDTH = 9;
	const int FONT_HEIGHT = 16;
	const int FONT_CHARS_PER_LINE = 16;
	const int MAX_ITEMS = 21;
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
