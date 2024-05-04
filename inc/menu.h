#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include <vector>
#include <limits.h>

SDL_Window* gWindow;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
SDL_Event event;

class Menu {
	SDL_Surface* screen;
	TTF_Font* font;
	int x, y; 					// Mouse position
	int numMenus = 0;
	std::vector<std::string> labels{};
	std::vector<SDL_Surface*> menus{};
	std::vector<SDL_Rect> pos{};
	SDL_Surface* ekraan;
	int suurim = 0;
	SDL_Rect ajutine;

public:
	Menu(TTF_Font* SisendFont, std::vector<std::string> iLabels) {
		ekraan = SDL_GetWindowSurface(gWindow);
		font = SisendFont;
		labels = iLabels;
		int suurus = 0;
		int ajutine_suurus = 0;
		numMenus = iLabels.size();
		for (int i = 0; i < numMenus; i++) {
			ajutine_suurus = labels.at(i).size();
			if (suurus < ajutine_suurus) {
				suurim = i;
				suurus = ajutine_suurus;
			};
		};

		for (int i = 0; i < numMenus; i++)
			menus.push_back(TTF_RenderText_Solid(font, labels.at(i).c_str(), { 255,255,255 }));
	};

	// Event loop
	int valja() {
		ekraan = SDL_GetWindowSurface(gWindow);

		ajutine.w = menus.at(suurim)->w;
		ajutine.h = menus.at(suurim)->h;
		pos = {};
		for (int i = 0; i < numMenus; i++) {
			ajutine.x = ekraan->w / 2 - menus.at(suurim)->w / 2;
			ajutine.y = ekraan->h / 2 + (menus.at(suurim)->h) * (i - 1);
			pos.push_back(ajutine);
		}

		//		SDL_Event event;
		SDL_Rect raam;
		raam.x = ekraan->w / 2 - menus.at(suurim)->w / 2;
		raam.y = ekraan->h / 2 - menus.at(suurim)->h;
		raam.w = menus.at(suurim)->w;
		raam.h = (menus.at(suurim)->h) * numMenus;

		SDL_FillRect(ekraan, /*&screen->clip_rect*/ &raam, SDL_MapRGB(ekraan->format, 128, 128, 128));
		SDL_UpdateWindowSurface(gWindow);

		// Väljastamine
		for (int i = 0; i < numMenus; i++)
			if ((SDL_BlitSurface(menus.at(i), NULL, ekraan, &pos.at(i))) < 0)
				printf("Blit error %s\n", SDL_GetError());
		SDL_UpdateWindowSurface(gWindow);


		while ((SDL_PollEvent(&event) != 0)) {
			if (/*event.type== SDL_MOUSEBUTTONDOWN && */event.button.button == SDL_BUTTON_LEFT) {
				x = event.button.x;
				y = event.button.y;
				for (int i = 0; i < numMenus; i++)
					if (x >= pos.at(i).x && x <= pos.at(i).x + pos.at(i).w &&
						y >= pos.at(i).y && y <= pos.at(i).y + pos.at(i).h)
						return i;
					else
						return -1;
			};
		};
	};
};

// Initialize SDL
bool init() {
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	} else {
		//Create window
		gWindow = SDL_CreateWindow("Create Triangle", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
			SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
		if (gWindow == NULL) {
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
	}
	if (TTF_Init() == -1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		success = false;
	};
	return success;
};

void Exit() {
	TTF_Quit();
	SDL_DestroyWindow(gWindow);
	SDL_Quit();
}


int main(int argc, char* argv[]) {
	if (!init()) {
		printf("Failed to initialize!\n");
	} else {
		// Fondi avamine
		TTF_Font* Font;
		Font = TTF_OpenFont("Lohit-Devanagari.ttf", 20);
		if (!Font)
			printf("TTF_OpenFont: %s\n", TTF_GetError());

		// Menüü loomine
		SDL_Surface* screen;
		screen = SDL_GetWindowSurface(gWindow);
		if (!screen) {
			printf("SDL_GetWindowSurface error: %s\n", SDL_GetError());
		}

		vector<string> nimed{};
		nimed.push_back("Esimene");
		nimed.push_back("Teine");
		nimed.push_back("Seitsmes");
		nimed.push_back("Kolmas");
		Menu menu(Font, nimed);

		bool quit = false;
		//      SDL_Event e;

		while (!quit) {
			while (SDL_PollEvent(&event) != 0) {
				switch (event.type) {
				case SDL_QUIT:
					quit = true;
					Exit();
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == SDL_BUTTON_RIGHT) {
						int j = menu.valja();
						switch (j) {
						case 0:
							cout << "Esimene\n";
							break;
						case 1:
							cout << "Teine\n";
							break;
						case 2:
							cout << "Kolmas\n";
							break;
						case -1:
							cout << "Mujale\n";
							break;
						}
					}
					break;
				}
			}
		}
	}
	return 0;
}