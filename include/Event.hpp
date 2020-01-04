#pragma once

#ifdef _WIN32
#include "SDL_events.h"
#include "SDL_keycode.h"
#elif defined __linux__
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#endif

#include <vector>

enum KEY_TYPE { KEY_NULL , KEY_DOWN , KEY_HOLD , KEY_UP };

struct Keyboard_Event {
	SDL_Keycode key;
	KEY_TYPE type;
};

enum BUTTON_TYPE { BUTTON_NULL , BUTTON_DOWN, BUTTON_HOLD, BUTTON_UP };

struct Mouse_Event {
	int x = 0, y = 0;
	int dx = 0, dy = 0;
	BUTTON_TYPE button_state[5] = { BUTTON_NULL , BUTTON_NULL , BUTTON_NULL ,
		BUTTON_NULL , BUTTON_NULL };

	int mouse_wheel = 0;
};

struct Window_Event {
	int w = 0, h = 0;
};

extern struct Event_Struct {
	std::vector<Keyboard_Event> key_events = std::vector<Keyboard_Event>();
	Mouse_Event mouse_event;
	Window_Event window_event;

	void AddKey(const SDL_KeyboardEvent& event);
	KEY_TYPE GetKeyState(SDL_Keycode key);
} Event_Struct;