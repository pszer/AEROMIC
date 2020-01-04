#pragma once

#ifdef _WIN32
#include "SDL.h"
#include "SDL_ttf.h"
#elif defined __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#endif

#include "Log.hpp"
#include "Filesystem.hpp"

#include <map>

#define TTF_FOLDER "ttf"

#define FONT_EUROSTILE "ttf\\Eurostile.ttf"
#define FONT_FRANKLIN "ttf\\framd.ttf"

struct Font {
	TTF_Font* _8pt = nullptr,
		*_12pt = nullptr,
		*_16pt = nullptr,
		*_24pt = nullptr,
		*_32pt = nullptr,
		*_64pt = nullptr;
};

enum FONT_SIZE { FONT_8PT, FONT_12PT, FONT_16PT, FONT_24PT, FONT_32PT , FONT_64PT };

extern struct Fonts {
	void Init();
	void Quit();

	// returns -1 if error, 0 for success
	int OpenFont(const std::string& filename, Font& font);
	void FreeFont(Font&);

	std::map<std::string, Font> fonts;
	/* returns how many fonts were loaded
	*/
	int LoadFonts();
	/* adds font to std::map<std::string, Font> fonts
	* returns: 0 for success, -1 for error
	*/
	int AddFont(const std::string& filename);

	TTF_Font* GetFont(const std::string& str, FONT_SIZE size);
} Fonts;