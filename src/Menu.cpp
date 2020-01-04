#include "Menu.hpp"

struct Menu_Manager Menu_Manager;

Menu_Screen::Menu_Screen(std::string _name, void (*specific_func) ( ) ) : name(_name),
	ScreenSpecificUpdate(specific_func) { ; }

void Menu_Manager::Init() {
	Menu_Screen title = Menu_Screen(SCREEN_TITLE_NAME);
	title.decals.emplace_back(TITLE_IMAGE, Vec2_F(0.5f, 0.0f), Vec2_F(0.0f, 140.0f), Vec2_F(640.0f, 128.0f));
	title.buttons.emplace_back("PLAY", FONT_EUROSTILE, FONT_64PT, Vec2_F(0.5f, 0.6f), Vec2_F(0.0f, -120.0f), &__BUTTON_Menu_Play);
	title.buttons.emplace_back("CONTROLS", FONT_EUROSTILE, FONT_64PT, Vec2_F(0.5f, 0.6f), Vec2_F(0.0f, 0.0f), &__BUTTON_Menu_Controls);
	title.buttons.emplace_back("QUIT", FONT_EUROSTILE, FONT_64PT, Vec2_F(0.5f, 0.6f), Vec2_F(0.0f, 120.0f), &__BUTTON_Menu_Quit);

	Menu_Screen controls = Menu_Screen(SCREEN_CONTROLS_NAME);
	controls.decals.emplace_back(CONTROLS_IMAGE, Vec2_F(0.5f, 0.4f), Vec2_F(0.0f, 0.0f), Vec2_F(600.0f, 500.0f));
	controls.buttons.emplace_back("img\\arrow_left_black.png", "img\\arrow_left_gray.png",
		Vec2_F(0.5f, 0.9f), Vec2_F(0.0f, 0.0f), Vec2_F(128.0f, 64.0f), &__BUTTON_Controls_GoBack);

	Menu_Screen levels = Menu_Screen(SCREEN_LEVELS_NAME);
	levels.buttons.emplace_back("Tutorial", FONT_EUROSTILE, FONT_64PT, Vec2_F(0.5f, 0.50f), Vec2_F(0.0f, -220.0f), &__BUTTON_Play_Tutorial);
	levels.buttons.emplace_back("Episode 1", FONT_EUROSTILE, FONT_64PT, Vec2_F(0.5f, 0.50f), Vec2_F(0.0f, -100.0f), &__BUTTON_Play_Ep1);
	levels.buttons.emplace_back("Episode 2", FONT_EUROSTILE, FONT_64PT, Vec2_F(0.5f, 0.50f), Vec2_F(0.0f, 20.0f), &__BUTTON_Play_Ep2);
	levels.buttons.emplace_back("Episode 3", FONT_EUROSTILE, FONT_64PT, Vec2_F(0.5f, 0.50f), Vec2_F(0.0f, 140.0f), &__BUTTON_Play_Ep3);
	levels.buttons.emplace_back("img\\arrow_left_black.png", "img\\arrow_left_gray.png", Vec2_F(0.5f, 0.9f), Vec2_F(0.0f, 0.0f),
		Vec2_F(128.0f, 64.0f), &__BUTTON_Controls_GoBack);

	Menu_Screen pause = Menu_Screen(SCREEN_PAUSE_NAME, __SCREEN_Pause_Update);
	pause.decals.emplace_back(PAUSE_IMAGE, Vec2_F(0.5f, 0.0f), Vec2_F(0.0f, 140.0f), Vec2_F(640.0f, 128.0f));
	pause.buttons.emplace_back("Resume", FONT_EUROSTILE, FONT_64PT, Vec2_F(0.5f, 0.50f), Vec2_F(0.0f, -20.0f), &__BUTTON_Pause_Resume);
	pause.buttons.emplace_back("Return to Title Screen", FONT_EUROSTILE, FONT_64PT, Vec2_F(0.5f, 0.50f), Vec2_F(0.0f, 100.0f), &__BUTTON_Pause_ReturnToTitle);

	this->screens.push_back(title);
	this->screens.push_back(controls);
	this->screens.push_back(levels);
	this->screens.push_back(pause);
}

void Menu_Manager::Update() {
	Menu_Screen& scr = this->screens.at(this->screen);
	for (auto B = scr.buttons.begin(); B != scr.buttons.end(); ++B) {
		// true if mouse is over the button
		Rect_F rect = Rect_F((float)B->box.x, (float)B->box.y, (float)B->box.w, (float)B->box.h);
		B->hovered_over = PointRectangleCollision(Vec2_F((float)Event_Struct.mouse_event.x, (float)Event_Struct.mouse_event.y),
			rect);

		if (B->hovered_over) {
			scr.last_button_hovered = B - scr.buttons.begin();
			if (Event_Struct.mouse_event.button_state[0] == BUTTON_DOWN)
				if (B->click_event != nullptr) B->click_event();
		}
	}

	if (scr.ScreenSpecificUpdate != nullptr) {
		scr.ScreenSpecificUpdate();
	}

	KEY_TYPE key = Event_Struct.GetKeyState(SDLK_UP);
	if (key == KEY_DOWN)
		Sound.ChangeSong(1);
	key = Event_Struct.GetKeyState(SDLK_DOWN);
	if (key == KEY_DOWN)
		Sound.ChangeSong(-1);
	key = Event_Struct.GetKeyState(SDLK_m);
	if (key == KEY_DOWN)
		Sound.ToggleMusic();
	key = Event_Struct.GetKeyState(SDLK_F5);
	if (key == KEY_DOWN)
		Core.Fullscreen();
}

void Menu_Manager::WindowUpdate() {
	for (auto S = this->screens.begin(); S != this->screens.end(); ++S) {
		for (auto D = S->decals.begin(); D != S->decals.end(); ++D) {
			D->box = this->RectFromSizePos(D->pos, D->offset, D->size);
		}

		for (auto B = S->buttons.begin(); B != S->buttons.end(); ++B) {
			B->box = this->RectFromSizePos(B->pos, B->offset, B->size);
		}
	}
}

void Menu_Manager::ChangeScreen(std::string str) {
	for (int i = 0; i < (int)this->screens.size(); ++i) {
		if (this->screens.at(i).name == str) {
			this->last_screen = this->screen;
			this->screen = i;
			return;
		}
	}

	Log::PrintLogError("Could not find Menu_Screen with the name \"" + str + "\"");
}

SDL_Rect Menu_Manager::RectFromSizePos(Vec2_F pos,  Vec2_F offset, Vec2_F size) {
	return SDL_Rect {(int)((pos.x * Event_Struct.window_event.w) - size.w / 2.0f + offset.x),
		(int)((pos.y * Event_Struct.window_event.h) - size.h / 2.0f + offset.y),
		(int)(size.w),
		(int)(size.h) };
}

Menu_Items::Decal::Decal(std::string image, Vec2_F pos, Vec2_F offset, Vec2_F size): image(image), pos(pos), offset(offset), size(size) {
	this->box = Menu_Manager.RectFromSizePos(pos, offset, size);
}

Menu_Items::Button::Button(std::string txt, std::string font, FONT_SIZE font_size, Vec2_F pos, Vec2_F offset, void (*func)(void), SDL_Color c, SDL_Color hover_c) :
	color(c), font_size(font_size), pos(pos), offset(offset), txt(txt), font(font), hover_color(hover_c), click_event(func)
{
	this->is_text = true;
	TTF_Font* f = Fonts.GetFont(font, font_size);
	if (f != nullptr) {
		int w, h;
		TTF_SizeText(f, txt.c_str(), &w, &h);
		this->size = Vec2_F((float)w, (float)h);
		this->box = Menu_Manager.RectFromSizePos(pos, offset, this->size);
	}
	else {
		this->box = { 0,0,0,0 };
	}
}

Menu_Items::Button::Button(std::string texture, std::string hover_texture, Vec2_F pos, Vec2_F offset, Vec2_F size, void (*func)(void)) :
	txt(texture), font(hover_texture), pos(pos), offset(offset), size(size), click_event(func)
{
	this->is_text = false;
	this->box = Menu_Manager.RectFromSizePos(pos, offset, size);
}

void __BUTTON_Menu_Play() {
	Menu_Manager.ChangeScreen(SCREEN_LEVELS_NAME);
}

void __BUTTON_Menu_Controls() {
	Menu_Manager.ChangeScreen(SCREEN_CONTROLS_NAME);
}

void __BUTTON_Menu_Quit() {
	Core.quit = true;
}

void __BUTTON_Play_Tutorial() {
	RocketJump.chosen_level = LEVEL_TUTORIAL;
	Core.ChangeGamestate(CORE_GAMESTATE_GAME);
}

void __BUTTON_Play_Ep1() {
	RocketJump.chosen_level = LEVEL_EPISODE1;
	Core.ChangeGamestate(CORE_GAMESTATE_GAME);
}

void __BUTTON_Play_Ep2() {
	RocketJump.chosen_level = LEVEL_EPISODE2;
	Core.ChangeGamestate(CORE_GAMESTATE_GAME);
}

void __BUTTON_Play_Ep3() {
	RocketJump.chosen_level = LEVEL_EPISODE3;
	Core.ChangeGamestate(CORE_GAMESTATE_GAME);
}

void __BUTTON_Controls_GoBack() {
	Menu_Manager.ChangeScreen(SCREEN_TITLE_NAME);
}

void __SCREEN_Pause_Update() {
	KEY_TYPE key = Event_Struct.GetKeyState(SDLK_ESCAPE);

	// Unpause
	if (key == KEY_DOWN) {
		Sound.ResumeAllSound();
		Core.UnsafeChangeGamestate(CORE_GAMESTATE_GAME);
	}
}

void __BUTTON_Pause_Resume() {
	Sound.ResumeAllSound();
	Core.UnsafeChangeGamestate(CORE_GAMESTATE_GAME);
}

void __BUTTON_Pause_ReturnToTitle() {
	RocketJump.Quit();
	Menu_Manager.ChangeScreen(SCREEN_TITLE_NAME);
}
