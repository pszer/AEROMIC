#pragma once

#include <iostream>
#include <string>

#ifdef _WIN32
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#elif defined __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#endif

namespace Log {
	void PrintSDLError();
	void PrintIMGError();
	void PrintMIXError();
	void PrintTTFError();
	void PrintLogError(const std::string&);
}