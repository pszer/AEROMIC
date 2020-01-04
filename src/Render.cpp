#include "Render.hpp"

struct Render Render;

void Render::Render_OnAll(SDL_Renderer * r) {
	if (Sound.music_changed) {
		Sound.music_changed = false;
		this->music_change_timer = this->music_change_time;
	}

	if (this->music_change_timer > 0) {
		--this->music_change_timer;

		std::string str = Sound.GetTrackName(Sound.song_filename);

		int offset = 0;
		if (music_change_timer < 20) {
			offset = -(20 - music_change_timer) * 40;
		} else if (music_change_timer > music_change_time - 20) {
			offset = -(music_change_timer - (music_change_time - 20)) * 40;
		}

		SDL_Rect music_icon_rect = { 5 + offset, 5, 24, 24};

		int w, h;
		TTF_Font* f = Fonts.GetFont(FONT_FRANKLIN, FONT_16PT);
		if (f != nullptr) {
			TTF_SizeText(f, str.c_str(), &w, &h);
			for (int i = 5; i < 37 - 16 + w + 15; i++) {
				SDL_Rect back_rect = { i + offset - 24 , 17 - 24 , 48 , 48};
				this->RenderTexture(r, "img\\back.png", &back_rect);
			}
		}

		this->RenderTexture(r, TEXTURE_MUSIC_SYMBOL, &music_icon_rect);
		this->RenderText(r, str, FONT_FRANKLIN, 37 + offset, 6, FONT_16PT, ALIGN_LEFT);
	}

	static SDL_Rect mouse_rect = {0,0,32,32};
	mouse_rect.x = Event_Struct.mouse_event.x;
	mouse_rect.y = Event_Struct.mouse_event.y;
	this->RenderTexture(r, TEXTURE_CURSOR, &mouse_rect);

	SDL_RenderPresent(r);
}

/*

***	Render_Menu

*/


void Render::Render_Menu(SDL_Renderer* r) {
	SDL_SetRenderDrawColor(r, 230, 230, 230, 0xff);
	SDL_RenderClear(r);

	SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

	if (Menu_Manager.screen == 0) {
		this->_Menu_RenderButtonBack(r);
	}
	this->_Menu_RenderDecals(r);
	this->_Menu_RenderButtons(r);
}

void Render::_Menu_RenderDecals(SDL_Renderer * r) {
	for (auto D : Menu_Manager.screens.at(Menu_Manager.screen).decals) {
		this->RenderTexture(r, D.image, &D.box);
	}
}

void Render::_Menu_RenderButtons(SDL_Renderer * r) {
	for (auto B : Menu_Manager.screens.at(Menu_Manager.screen).buttons) {
		if (B.is_text) {
			this->RenderText(r, B.txt, B.font, B.box.x, B.box.y, B.font_size, ALIGN_LEFT, (B.hovered_over ? B.hover_color : B.color));
		} else {
			this->RenderTexture(r, (B.hovered_over ? B.font : B.txt), &B.box);
		}
	}
}

void Render::_Menu_RenderButtonBack(SDL_Renderer * r) {
	static SDL_Rect rect = { 0 , -100 , Event_Struct.window_event.w, 64 };
	static float y_float = 0;
	const SDL_Color back_color = { 0x0 , 0x0 , 0x0 , 86 };

	rect.w = Event_Struct.window_event.w;
	Menu_Screen& scr = Menu_Manager.screens.at(Menu_Manager.screen);
	int y = scr.buttons.at(scr.last_button_hovered).box.y;
	y_float += ((y - y_float) / 5.0f);
	rect.y = (int)y_float;

	SDL_SetRenderDrawColor(r, back_color.r, back_color.g, back_color.b, back_color.a);
	SDL_RenderFillRect(r, &rect);
}

/*

***

*/

/*

***	Render_Game

*/

void Render::Render_Game(SDL_Renderer* r) {
	SDL_SetRenderDrawColor(r, 230, 230, 230, 0xff);
	SDL_RenderClear(r);

	SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);

	if (RocketJump.Vars.episode_complete) {
		SDL_Rect rect = {0, 0, 1920, 157};
		rect.y = Event_Struct.window_event.h / 2 - rect.h;
		rect.x = -rect.w + (int)((RocketJump.Vars.episode_complete_timer / (float)RocketJump.Vars.episode_complete_time)
			* (rect.w + Event_Struct.window_event.w));

		this->RenderTexture(r, TEXTURE_EPISODE_COMPLETE, &rect);
		return;
	}

	this->GenerateShake();

	this->_Game_RenderBrushes(r);

	if (!RocketJump.Player.dead) {
		this->_Game_RenderPlayer(r);
	}

	std::string level_info = "";
	if (RocketJump.Vars.episode_name != "")
		level_info += RocketJump.Vars.episode_name + " : ";
	level_info += RocketJump.Vars.level_name;
	this->RenderText(r, level_info, FONT_EUROSTILE, Event_Struct.window_event.w - 5, 5, FONT_16PT, ALIGN_RIGHT);

	this->_Game_RenderEntities(r);
}

void Render::_Game_RenderBrushes(SDL_Renderer* r) {
	SDL_Color c;
	int count = 0;
	for (auto R : RocketJump.loaded_level.brushes) {
		c = RocketJump.GetBrushTypeColor(R.type);

		Rect_F rect;
		if (RocketJump.Vars.level_transition_timer > 0) {
			float time = RocketJump.Vars.level_transition_timer / (float)RocketJump.Vars.level_transition_time;

			rect = R.brush +
				// level transition transformation
				Vec2_F((150.0f + 60.0f * (count % 5)) * (time * time) * (count % 2 ? -1.0f : 1.0f), 0.0f);

			this->texture_alpha_mod = int(255.0f - time * 255.0f);
			SDL_SetRenderDrawColor(r, c.r, c.g, c.b, (int)(c.a * (1.0f - time * time)));
		}
		else {
			rect = R.brush;
			this->texture_alpha_mod = 255;
			SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
		}

		SDL_Rect brush_rect = this->TransformRect(rect).ToSDLRect();

		if (R.type != BRUSH_DECAL) {
			SDL_RenderFillRect(r, &brush_rect);
		}
		else {
			SDL_Texture* text = this->GetTexture(R.key);
			if (text != nullptr) {
				SDL_SetTextureAlphaMod(text, this->texture_alpha_mod);
				SDL_RenderCopy(r, text, nullptr, &brush_rect);
			}
		}

		++count;
	}
}

void Render::_Game_RenderEntities(SDL_Renderer * r) {
	for (auto E = RocketJump.entities.begin(); E != RocketJump.entities.end(); ++E) {
		if (dynamic_cast<Rocket*>((*E).get()) != nullptr) {
			this->_Game_RenderRocket(r, (Rocket*)(*E).get());
		} 
		else if (dynamic_cast<Particle*>((*E).get()) != nullptr) {
			_Game_RenderParticle(r, (Particle*)(*E).get());
		}
	}
}

void Render::_Game_RenderPlayer(SDL_Renderer* r) {
	SDL_Texture* p_texture = this->GetTexture(RocketJump.Player_GetFrame());
	if (p_texture == nullptr)
		return;
	SDL_SetTextureBlendMode(p_texture, SDL_BLENDMODE_BLEND);

	float dx, dy;
	dx = Event_Struct.mouse_event.x - (Event_Struct.window_event.w / 2.0f);
	dy = Event_Struct.mouse_event.y - (Event_Struct.window_event.h / 2.0f);
	float angle = this->RadToDegrees(std::atan2(dy, dx));

	SDL_RendererFlip flip;
	if (angle > 90.0f || angle < -90.0f)
		flip = SDL_FLIP_VERTICAL;
	else
		flip = SDL_FLIP_NONE;

	std::string rl_texture_str;
	if (angle > 35.0f && angle < 145.0f)
		rl_texture_str = TEXTURE_PLAYER_RL3;
	else if (angle < -35.0f && angle > -145.0f)
		rl_texture_str = TEXTURE_PLAYER_RL2;
	else
		rl_texture_str = TEXTURE_PLAYER_RL;
	SDL_Texture* rl_texture = this->GetTexture(rl_texture_str);
	if (rl_texture == nullptr)
		return;
	SDL_SetTextureBlendMode(rl_texture, SDL_BLENDMODE_BLEND);

	for (std::size_t i = 0; i < RocketJump.Player.prev_pos.size(); ++i) {
		int j = RocketJump.Player.prev_pos.size() - i - 1;
		if (i == RocketJump.Player.prev_pos.size() - 1) {
			SDL_SetTextureAlphaMod(p_texture, 0xff);
			SDL_SetTextureAlphaMod(rl_texture, 0xff);
		}
		else {
			SDL_SetTextureAlphaMod(p_texture, 31 + (Uint8)i * 24);
			SDL_SetTextureAlphaMod(rl_texture, 31 + (Uint8)i * 24);
		}

		SDL_Rect player_rect = this->TransformRect(Rect_F(RocketJump.Player.prev_pos.at(i),
			RocketJump.Player.ent.hitbox.size)).ToSDLRect();
		this->RenderTexture(r, RocketJump.Player_GetFrame(), &player_rect, 0.0f,
			(RocketJump.Player.ent.vel.x > 0.0f ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL));

		SDL_Rect rl_rect = this->TransformRect(RocketJump.Vars.rl_box + RocketJump.Player.prev_pos.at(i)).ToSDLRect();
		this->RenderTexture(r, rl_texture_str, &rl_rect, angle, flip);
	}
}

void Render::_Game_RenderRocket(SDL_Renderer * r, Rocket * rocket) {
	SDL_Rect ent_rect = this->TransformRect(
		Rect_F(rocket->ent.hitbox.pos + (rocket->ent.hitbox.size/2.0f) - Vec2_F(16.0f, 8.0f), Vec2_F(32.0f, 16.0f))
		).ToSDLRect();
	SDL_RenderCopyEx(r, this->GetTexture("img\\rocket.png"), nullptr, &ent_rect, this->RadToDegrees(rocket->angle), nullptr, SDL_FLIP_NONE);
}

void Render::_Game_RenderParticle(SDL_Renderer * r, Particle * particle) {
	SDL_Rect ent_rect = this->TransformRect(particle->ent.hitbox).ToSDLRect();
	SDL_SetRenderDrawColor(r, particle->color.r, particle->color.g, particle->color.b, particle->color.a);
	SDL_RenderFillRect(r, &ent_rect);
}

/*

***

*/

Vec2_F Render::CameraTransform(Vec2_F p) {
	return Vec2_F(
		(p.x - Camera.x) * Camera.zoom + (Event_Struct.window_event.w / 2.0f),
		(p.y - Camera.y) * Camera.zoom + (Event_Struct.window_event.h / 2.0f)
	) + Vec2_F(this->shake_x, this->shake_y);
}

Rect_F Render::TransformRect(const Rect_F& rect) {
	return Rect_F(this->CameraTransform(rect.pos), rect.size * Camera.zoom);
}

SDL_Texture * Render::GetTexture(const std::string & str) {
	auto t = this->textures.find(str);

	// if not found
	if (t == this->textures.end())
		return nullptr;
	
	return t->second;
}

int Render::LoadTextures(SDL_Renderer* r) {
	std::vector<std::string> files = GetAllFilesInDir(TEXTURES_FOLDER);

	int count = 0;
	for (auto f : files) {
		if (!this->AddTexture(r, f)) {
			++count;
			std::cout << "Loaded texture \"" << f << "\"" << std::endl;
		}
	}

	return count;
}

int Render::AddTexture(SDL_Renderer* r, const std::string & filename) {
	SDL_Surface* surface = IMG_Load(filename.c_str());
	if (surface == nullptr) {
		Log::PrintIMGError();
		return -1;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(r, surface);
	if (texture == nullptr) {
		Log::PrintSDLError();
		return -1;
	}

	std::string str = filename;
	for (auto c = str.begin(); c != str.end(); ++c)
		if (*c == '/')
			*c = '\\';
	this->textures[str] = texture;
	return 0;
}

void Render::RenderText(SDL_Renderer* r, const std::string& text, const std::string& font_name, int x, int y,
	FONT_SIZE size, RENDER_TEXT_ALIGN align, SDL_Color color)
{
	TTF_Font* font = Fonts.GetFont(font_name, size);
	if (font == nullptr)
		return;

	int w, h;
	TTF_SizeText(font, text.c_str(), &w, &h);

	SDL_Rect rect = { 0, y , w, h };
	switch (align) {
	case ALIGN_LEFT:
		rect.x = x;
		break;
	case ALIGN_CENTRE:
		rect.x = x - w / 2;
		break;
	case ALIGN_RIGHT:
		rect.x = x - w;
		break;
	}

	SDL_Surface* s = TTF_RenderText_Blended(font, text.c_str(), color);
	if (s == nullptr) {
		Log::PrintTTFError();
		return;
	}

	SDL_Texture* t = SDL_CreateTextureFromSurface(r, s);
	SDL_FreeSurface(s);
	if (t == nullptr) {
		Log::PrintSDLError();
		return;
	}

	SDL_RenderCopy(r, t, nullptr, &rect);
	SDL_DestroyTexture(t);
}

void Render::RenderTexture(SDL_Renderer * r, const std::string & texture, SDL_Rect* rect, float angle,
	SDL_RendererFlip flip)
{
	SDL_Texture* t= this->GetTexture(texture);
	if (t == nullptr)
		return;

	SDL_RenderCopyEx(r, t, nullptr, rect, angle, nullptr, flip);
}

void Render::Init(SDL_Renderer* r) {
	std::cout << "Loaded " << this->LoadTextures(r) << " textures" << std::endl;
}

void Render::Quit() {
	for (auto T : this->textures) {
		SDL_DestroyTexture( std::get<1>(T) );
	}

	this->textures.clear();
}

float Render::RadToDegrees(float angle) {
	return angle * 57.2957795131f;
}

void Render::GenerateShake() {
	if (Camera.shake == 0.0f) {
		this->shake_x = 0.0f;
		this->shake_y = 0.0f;
		return;
	}

	this->shake_x = std::fmod(static_cast<float>(std::rand()), 2 * Camera.shake) - Camera.shake;
	this->shake_y = std::fmod(static_cast<float>(std::rand()), 2 * Camera.shake) - Camera.shake;
}