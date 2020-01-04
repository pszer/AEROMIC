#pragma once

#include <vector>
#include <list>
#include <algorithm>

#include "Fonts.hpp"
#include "Event.hpp"
#include "Vec2.hpp"
#include "Hitreg.hpp"
#include "RocketJump.hpp"
#include "Core.hpp"

#define TITLE_IMAGE "img\\title.png"
#define CONTROLS_IMAGE "img\\controls.png"
#define PAUSE_IMAGE "img\\paused.png"

#define SCREEN_TITLE_NAME "title"
#define SCREEN_CONTROLS_NAME "controls"
#define SCREEN_LEVELS_NAME "levels"
#define SCREEN_PAUSE_NAME "pause"

struct Menu_Screen;
extern struct Menu_Manager Menu_Manager;

/* POSITIONS - positions are not x,y values but ratios 0.0-1.0 of the screen,
* for example the middle of the screen is (0.5,0.5)
*/

namespace Menu_Items {
	struct Decal {
		Decal(std::string image, Vec2_F pos, Vec2_F offset, Vec2_F size);
		std::string image;
		Vec2_F pos, offset;
		Vec2_F size;

		SDL_Rect box;
	};

	struct Button {
		Button(std::string txt, std::string font, FONT_SIZE font_size, Vec2_F pos, Vec2_F offset,
			void (*func)(void), SDL_Color c = { 0,0,0,0xff }, SDL_Color hover_c = { 0x30,0x30,0x30,0xff });
		Button(std::string texture, std::string hover_texture, Vec2_F pos, Vec2_F offset, Vec2_F size, void (*func)(void));

		SDL_Color color, hover_color;
		bool is_text, hovered_over = false;

		// if is_text, txt is buttons text and font is buttons font,
		// otherwise txt is the texture name and font is the hovered over image
		std::string txt, font;
		FONT_SIZE font_size;

		Vec2_F pos, offset;
		Vec2_F size;

		SDL_Rect box;

		void (*click_event)(void);
	};
};

extern struct Menu_Manager {
	void Init();

	void Update();
	void WindowUpdate();

	void ChangeScreen(std::string str);

	SDL_Rect RectFromSizePos(Vec2_F pos,  Vec2_F offset, Vec2_F size);

	int screen = 0, last_screen = 0;
	std::vector<Menu_Screen> screens;
} Menu_Manager;

struct Menu_Screen {
	Menu_Screen(std::string name, void (*specific_func) ( ) = nullptr);
	std::string name = "";

	std::vector<Menu_Items::Decal> decals;
	std::vector<Menu_Items::Button> buttons;

	void (*ScreenSpecificUpdate)( void ) = nullptr;

	int last_button_hovered = 0;
};

void __BUTTON_Menu_Play();
void __BUTTON_Menu_Controls();
void __BUTTON_Menu_Quit();

void __BUTTON_Play_Tutorial();
void __BUTTON_Play_Ep1();
void __BUTTON_Play_Ep2();
void __BUTTON_Play_Ep3();

void __BUTTON_Controls_GoBack();

void __SCREEN_Pause_Update();
void __BUTTON_Pause_Resume();
void __BUTTON_Pause_ReturnToTitle();