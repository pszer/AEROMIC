#pragma once

#include <iostream>
#include <vector>

#include "Log.hpp"
#include "Menu.hpp"
#include "Render.hpp"
#include "Sound.hpp"
#include "Event.hpp"
#include "Fonts.hpp"

#define WINDOW_TITLE "AEROMIC"
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_MIN_WIDTH 800
#define WINDOW_MIN_HEIGHT 600

enum CORE_GAMESTATE { CORE_GAMESTATE_MENU, CORE_GAMESTATE_GAME };

extern struct Core {
	// return -1 for error, 0 for success
	int Init(int argc = 0, char** argv = nullptr);
	void Quit();

	// returns -1 for error 0 for success
	int CreateWindow(std::string title, int w, int h, Uint32 flags);
	int CreateRenderer(Uint32 flags);

	void MainLoop();
	bool quit = false;

	// returns 1 if event is parsed, 0 if no event is parsed
	int ParseEvent();
	void UpdateEvent();

	void Fullscreen();

	SDL_Renderer * renderer = nullptr;
	SDL_Window * window = nullptr;

	std::vector<std::string> args = std::vector<std::string>();

	SDL_Event event;

	// Changes gamestate and calls init as necessary
	void ChangeGamestate(CORE_GAMESTATE state);
	// Changes gamestate without calling init
	void UnsafeChangeGamestate(CORE_GAMESTATE state);
	CORE_GAMESTATE gamestate;

	void _MenuLoop();
	void _GameLoop();
} Core;