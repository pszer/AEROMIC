#pragma once

#ifdef _WIN32
#include "SDL.h"
#include "SDL_image.h"
#elif defined __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif

#include <cstdlib>
#include <map>

#include "Fonts.hpp"
#include "Menu.hpp"
#include "Camera.hpp"
#include "RocketJump.hpp"
#include "Filesystem.hpp"
#include "Log.hpp"

#ifdef _WIN32
#define TEXTURES_FOLDER "img\\"
#elif defined __linux__
#define TEXTURES_FOLDER "img/"
#endif

#define TEXTURE_MUSIC_SYMBOL "img\\music_symbol.png"
#define TEXTURE_EPISODE_COMPLETE "img\\episode_complete.png"
#define TEXTURE_CURSOR "img\\cursor.png"

extern struct Render {
public:
	void Render_Menu(SDL_Renderer* r);
	void Render_Game(SDL_Renderer* r);
	// special function that renders on every gamestate
	void Render_OnAll(SDL_Renderer* r);

	Vec2_F CameraTransform(Vec2_F p);
	Rect_F TransformRect(const Rect_F& rect);

	// Gets texture from std::map<std::string, SDL_Texture* textures>
	SDL_Texture* GetTexture(const std::string& str);

	std::map<std::string, SDL_Texture*> textures;
	/* returns how many textures were loaded
	*/
	int LoadTextures(SDL_Renderer*);
	/* adds texture to std::map<std::string, SDL_Texture*> textures
	 * returns: 0 for success, -1 for error
	 */
	int AddTexture(SDL_Renderer*, const std::string& filename);

	enum RENDER_TEXT_ALIGN { ALIGN_LEFT, ALIGN_CENTRE, ALIGN_RIGHT };
	void RenderText(SDL_Renderer* r, const std::string& text, const std::string& font_name, int x, int y,
		FONT_SIZE size, RENDER_TEXT_ALIGN align, SDL_Color color = { 0,0,0,0xff });
	void RenderTexture(SDL_Renderer* r, const std::string& texture, SDL_Rect* rect, float angle = 0,
		SDL_RendererFlip flip = SDL_FLIP_NONE);

	void Init(SDL_Renderer* r);
	void Quit();

	float RadToDegrees(float angle);

	void GenerateShake();
	float shake_x = 0.0f, shake_y = 0.0f;
private:
	void _Game_RenderBrushes(SDL_Renderer* r);
	void _Game_RenderEntities(SDL_Renderer* r);
	void _Game_RenderPlayer(SDL_Renderer* r);

	void _Game_RenderRocket(SDL_Renderer* r, Rocket* rocket);
	void _Game_RenderParticle(SDL_Renderer* r, Particle* particle);

	void _Menu_RenderDecals(SDL_Renderer* r);
	void _Menu_RenderButtons(SDL_Renderer* r);
	void _Menu_RenderButtonBack(SDL_Renderer* r);

	int texture_alpha_mod = 255;

	int music_change_time = 300, music_change_timer = 0;
} Render;