#include "Core.hpp"

struct Core Core;

int Core::Init(int argc, char** argv) {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		Log::PrintSDLError();
		return -1;
	}
	
	int flags = IMG_INIT_JPG | IMG_INIT_PNG;
	if ((IMG_Init(flags) & flags) != flags) {
		Log::PrintIMGError();
		return -1;
	}

	flags = MIX_INIT_MP3 | MIX_INIT_OGG;
	if ((Mix_Init(flags) & flags) != flags) {
		Log::PrintMIXError();
		return -1;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
		Log::PrintMIXError();
		return -1;
	}

	if (TTF_Init() == -1) {
		Log::PrintTTFError();
		return -1;
	}

	Mix_AllocateChannels(32);

	int result = this->CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
	if (result == -1)
		return -1;
	SDL_SetWindowMinimumSize(this->window, WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT);
	SDL_ShowCursor(SDL_DISABLE);

	SDL_GetWindowSize(this->window, &Event_Struct.window_event.w, &Event_Struct.window_event.h);

	result = this->CreateRenderer(SDL_RENDERER_ACCELERATED);
	if (result == -1)
		return -1;

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	if (argc >= 2) {
		for (int i = 1; i < argc; ++i) {
			this->args.push_back(std::string(argv[i]));
		}
	}

	Render.Init(this->renderer);
	Sound.Init();
	Fonts.Init();

	Sound.ChangeSong(0);

	this->ChangeGamestate(CORE_GAMESTATE_MENU);
	if (this->args.size() > 0) {
		RocketJump.chosen_level = this->args.at(0);
		this->ChangeGamestate(CORE_GAMESTATE_GAME);
	}

	return 0;
}

int Core::CreateWindow(std::string title, int w, int h, Uint32 flags) {
	if (this->window != nullptr)
		SDL_DestroyWindow(this->window);
	this->window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		w, h, flags);
	if (this->window == nullptr) {
		Log::PrintSDLError();
		return -1;
	}

	return 0;
}

int Core::CreateRenderer(Uint32 flags) {
	if (this->renderer != nullptr)
		SDL_DestroyRenderer(this->renderer);
	this->renderer = SDL_CreateRenderer(this->window, -1, flags);
	if (this->renderer == nullptr) {
		std::cerr << "Falling back to software renderer" << std::endl;
		this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_SOFTWARE);
		if (this->renderer == nullptr) {
			Log::PrintSDLError();
			return -1;
		}
	}

	return 0;
}

void Core::Quit() {
	Render.Quit();
	RocketJump.Quit();
	Sound.Quit();
	Fonts.Quit();

	SDL_DestroyWindow(this->window);
	SDL_DestroyRenderer(this->renderer);

	Mix_CloseAudio();
	Mix_Quit();
	SDL_Quit();
	IMG_Quit();
}

int Core::ParseEvent() {
	if (!SDL_PollEvent(&this->event)) {
		return 0;
	}

	switch (this->event.type) {
	case SDL_QUIT:
		this->quit = true;
		break;
	case SDL_MOUSEMOTION:
		SDL_GetMouseState(&Event_Struct.mouse_event.x, &Event_Struct.mouse_event.y);
		break;
	case SDL_MOUSEBUTTONDOWN:
		Event_Struct.mouse_event.button_state[this->event.button.button - 1] = BUTTON_DOWN;
		break;
	case SDL_MOUSEBUTTONUP:
		Event_Struct.mouse_event.button_state[this->event.button.button - 1] = BUTTON_UP;
		break;
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		Event_Struct.AddKey(this->event.key);
		break;
	case SDL_MOUSEWHEEL:
		Event_Struct.mouse_event.mouse_wheel = this->event.wheel.y;
		break;
	case SDL_WINDOWEVENT:
		switch (this->event.window.event) {
		case SDL_WINDOWEVENT_RESIZED:
			SDL_GetWindowSize(this->window, &Event_Struct.window_event.w, &Event_Struct.window_event.h);
			Menu_Manager.WindowUpdate();
			break;
		}
	}

	return 1;
}

void Core::UpdateEvent() {
	Event_Struct.mouse_event.mouse_wheel = 0;

	for (BUTTON_TYPE* k = Event_Struct.mouse_event.button_state;
		k < Event_Struct.mouse_event.button_state + 5; ++k)
	{
		if (*k == BUTTON_DOWN)
			*k = BUTTON_HOLD;
		else if (*k == BUTTON_UP)
			*k = BUTTON_NULL;
	}

	for (int k = Event_Struct.key_events.size() - 1; k >= 0; --k) {
		if (Event_Struct.key_events.at(k).type == KEY_DOWN)
			Event_Struct.key_events.at(k).type = KEY_HOLD;
		else if (Event_Struct.key_events.at(k).type == KEY_UP)
			Event_Struct.key_events.erase(Event_Struct.key_events.begin() + k);
	}
}

void Core::Fullscreen() {
	int flags = SDL_GetWindowFlags(this->window);
	if ((flags & SDL_WINDOW_FULLSCREEN) == SDL_WINDOW_FULLSCREEN) {
		SDL_SetWindowFullscreen(this->window, 0);
		SDL_SetWindowSize(this->window, WINDOW_WIDTH, WINDOW_HEIGHT);
		Event_Struct.window_event.w = WINDOW_WIDTH;
		Event_Struct.window_event.h = WINDOW_HEIGHT;
	}
	else {
		SDL_DisplayMode mode;
		if (SDL_GetDesktopDisplayMode(0, &mode) < 0) {
			Log::PrintSDLError();
		}
		else {
			SDL_SetWindowSize(this->window, mode.w, mode.h);
			Event_Struct.window_event.w = mode.w;
			Event_Struct.window_event.h = mode.h;
			SDL_SetWindowFullscreen(this->window, SDL_WINDOW_FULLSCREEN);
		}
	}

	Menu_Manager.WindowUpdate();
}

void Core::MainLoop() {
	while (!this->quit) {
		this->UpdateEvent();
		while (this->ParseEvent()) {
			/* do something */
		}

		switch (this->gamestate) {
		case CORE_GAMESTATE_MENU:
			this->_MenuLoop();
			break;
		case CORE_GAMESTATE_GAME:
			this->_GameLoop();
			break;
		}

		Render.Render_OnAll(this->renderer);

		SDL_Delay(16);
	}
}

void Core::ChangeGamestate(CORE_GAMESTATE state) {
	this->gamestate = state;
	switch (state) {
	case CORE_GAMESTATE_MENU:
		Menu_Manager.Init();
		break;
	case CORE_GAMESTATE_GAME:
		RocketJump.Init();
		break;
	}
}

void Core::UnsafeChangeGamestate(CORE_GAMESTATE state) {
	this->gamestate = state;
}

void Core::_MenuLoop() {
	Menu_Manager.Update();
	Render.Render_Menu(this->renderer);
}

void Core::_GameLoop() {
	RocketJump.Update();
	Render.Render_Game(this->renderer);
}
