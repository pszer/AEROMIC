#include "RocketJump.hpp"

struct RocketJump RocketJump;

const std::string PLAYER_WALK_FRAME[TEXTURE_PLAYER_WALK_FRAME_COUNT] = { "img\\p_walk1.png" , "img\\p_walk2.png" , "img\\p_walk3.png" ,
"img\\p_walk4.png" , "img\\p_walk5.png" };
const std::string PLAYER_AIR_FRAME[TEXTURE_PLAYER_AIR_FRAME_COUNT] = { "img\\p_air1.png" , "img\\p_air2.png" , "img\\p_air3.png" };

void RocketJump::Init() {
	this->FreeEntities();
	this->LoadLevel(this->chosen_level);
	this->Player_Respawn();

	std::string str = SFX_WIND;
	this->wind_channel = Sound.PlayChunk(str, MIX_MAX_VOLUME, -1);
	Mix_Volume(this->wind_channel, 0);

	this->Vars.episode_complete = false;
}

void RocketJump::Quit() {
	this->FreeEntities();
	Sound.StopAllSound();
}

void RocketJump::Update() {
	this->Vars.tick++;

	if (this->Vars.episode_complete) {
		if (this->Vars.episode_complete_timer > 0) {
			--this->Vars.episode_complete_timer;
		} else {
			Menu_Manager.screen = 2;
			Core.ChangeGamestate(CORE_GAMESTATE_MENU);
		}
		return;
	}

	this->UpdateCamera();
	this->Vars.BRUSH_KILL_COLOR.r = (0xff - (4 * this->Vars.tick % 50));

	if (this->Vars.level_transition_timer > 0) {
		--this->Vars.level_transition_timer;
	}

	this->Player_Update();
	this->UpdateSound();

	// key input checks
	KEY_TYPE key = Event_Struct.GetKeyState(SDLK_UP);
	if (key == KEY_DOWN)
		Sound.ChangeSong(1);
	key = Event_Struct.GetKeyState(SDLK_DOWN);
	if (key == KEY_DOWN)
		Sound.ChangeSong(-1);
	key = Event_Struct.GetKeyState(SDLK_ESCAPE);
	if (key == KEY_DOWN) {
		Menu_Manager.ChangeScreen(SCREEN_PAUSE_NAME);
		Sound.PauseAllSound();
		Core.UnsafeChangeGamestate(CORE_GAMESTATE_MENU);
	}
	key = Event_Struct.GetKeyState(SDLK_F5);
	if (key == KEY_DOWN)
		Core.Fullscreen();
	key = Event_Struct.GetKeyState(SDLK_m);
	if (key == KEY_DOWN)
		Sound.ToggleMusic();

	for (std::size_t i = 0; i < this->entities.size();) {
		auto& E = this->entities.at(i);
		E->Update();
		if (!(E->to_delete) && !((E->ent.hitbox.pos.x < -99999.9f) ||
			(E->ent.hitbox.pos.x > 99999.9f) ||
			(E->ent.hitbox.pos.y < -99999.9f) ||
			(E->ent.hitbox.pos.y > 99999.9f)))
		{
			++i;
		} else {
			std::unique_ptr<Entity_Base> ent = std::move(E);
			this->entities.erase(this->entities.begin() + i);
			ent.reset();
		}
	}

	for (std::size_t i = 0; i < this->entities_add_queue.size(); ++i) {
		auto& E = this->entities_add_queue.at(i);
		this->entities.push_back(std::move(E));
	}
	this->entities_add_queue.clear();

	if (Event_Struct.mouse_event.mouse_wheel && (this->Vars.level_transition_timer <= 0)) {
		if (Event_Struct.mouse_event.mouse_wheel < 0) {
			for (int i = Event_Struct.mouse_event.mouse_wheel; i < 0; ++i)
				Camera.zoom /= 1.08f;
			Camera.zoom = std::max(Camera.zoom, this->Vars.min_zoom);
		}
		else {
			for (int i = 0; i < Event_Struct.mouse_event.mouse_wheel; ++i)
				Camera.zoom *= 1.08f;
			Camera.zoom = std::min(Camera.zoom, this->Vars.max_zoom);
		}
	}
}

void RocketJump::UpdateSound() {
	if (this->Player.dead)
		this->WindSound(0);
	else
		this->WindSound(-1);

	for (auto S : Sound.chunks_playing) {
		if (S.affect_by_zoom) {
			Mix_Volume(S.channel, GetVolumeByZoom());
		}
	}
}

void RocketJump::WindSound(int vol) {
	if (this->wind_channel == -1)
		return;
	int volume = 0;
	if (vol == -1) {
		if (this->Player.knocked)
			volume = std::max(0, std::min(MIX_MAX_VOLUME, (int)(5.0f * std::sqrt(this->Player.ent.vel.x * this->Player.ent.vel.x +
			this->Player.ent.vel.y * this->Player.ent.vel.y) - 50.0f)));
	} else  {
		volume = vol;
	}

	Mix_Volume(this->wind_channel, volume);
}

void RocketJump::UpdateCamera() {
	Camera.x = this->Player.ent.hitbox.pos.x + this->Player.ent.hitbox.size.w/2.0f;
	Camera.y = this->Player.ent.hitbox.pos.y + this->Player.ent.hitbox.size.h/2.0f;

	if (this->Player.dead)
		Camera.shake = std::max(0.0f, (float)this->Vars.respawn_timer / 2.0f);
	else if (this->Player.knocked) {
		float speed = std::sqrt(this->Player.ent.vel.x * this->Player.ent.vel.x +
			this->Player.ent.vel.y * this->Player.ent.vel.y);
		Camera.shake = speed / 15.0f;
	}
	else
		Camera.shake = 0.0f;
}

int RocketJump::GetVolumeByZoom() {
	return std::min(MIX_MAX_VOLUME, (int)(MIX_MAX_VOLUME * std::abs(Camera.zoom)));
}

void RocketJump::Player_Update() {
	if (this->Player.dead) {

		if (Event_Struct.mouse_event.button_state[0] == BUTTON_DOWN) {
			this->Player.respawn = true;
		}

		this->Vars.respawn_timer--;

		if (this->Player.respawn && this->Vars.respawn_timer <= 0) {
			this->Player_Respawn();
			this->Player.respawn = false;
		}

		return;
	}

	if (this->Player.rocket_delay > 0) {
		this->Player.rocket_delay--;
	}

	for (std::size_t i = 0; i < this->Player.prev_pos.size() - 1; ++i) {
		this->Player.prev_pos.at(i) = this->Player.prev_pos.at(i + 1);
	}
	this->Player.prev_pos.back() = this->Player.ent.hitbox.pos;

	this->Player_UpdateVel();
	this->Player_CollideWithBrushes();
	this->Player_UpdatePos();
	this->Player_UpdateFrame();

	if (Event_Struct.mouse_event.button_state[0] == BUTTON_DOWN) {
		this->Player_Shoot();
	}

	if (this->Player.ent.on_ground)
		this->Player.knocked = false;
}

void RocketJump::Player_Kill() {
	this->Player.dead = true;
	this->Vars.respawn_timer = 60;

	for (int i = 0; i < this->Vars.death_gore_count; ++i) {
		SDL_Color c = { 0xA0 , 0x0 , 0x0 , 0xff };
		std::unique_ptr<Particle> particle = std::make_unique<Particle>(
			this->Player.ent.hitbox.pos - this->Player.ent.vel, Vec2_F(10.0f, 10.0f), static_cast<float>(std::rand() / 1000.0f),
			30.0f * ((std::rand() % 1000) / 2000.0f + 0.5f), 150 + std::rand() % 60, BEHAVIOUR_COLLIDE, true, c);
		this->AddEntity(std::move(particle));
	}

	std::string str = SFX_DEATH_PREFIX + std::to_string((std::rand() % 5) + 1) + ".wav";
	Sound.PlayChunk(str, this->GetVolumeByZoom(), 0, true);
	str = SFX_GIB;
	Sound.PlayChunk(str, this->GetVolumeByZoom(), 0, true);
}

void RocketJump::Player_Respawn() {
	this->Player.ent.hitbox = Rect_F(00.0f, 00.0f, 20.0f, 40.0f);
	this->Player.ent.hitbox.pos = this->Player.ent.hitbox.pos - (this->Player.ent.hitbox.size / 2.0f);
	this->Player.ent.vel = Vec2_F(0.0f, 0.0f);

	this->Vars.level_transition_timer = this->Vars.level_transition_timer / 2;

	this->Player.dead = false;
	this->Player.rocket_delay = 0;

	this->FreeEntities();

	this->Player.prev_pos.fill(this->Player.ent.hitbox.pos);
}

void RocketJump::Player_UpdateVel() {
	this->Player.ent.vel.x = this->Player.ent.vel.x * (this->Player.ent.on_ground ? this->Vars.ground_resistance :
		this->Vars.air_resistance);
	this->Player.ent.vel.y += this->Vars.gravity;

	KEY_TYPE key_state = Event_Struct.GetKeyState(SDLK_SPACE);
	if (key_state == KEY_DOWN || key_state == KEY_HOLD) {
		this->Player_Jump();
	}

	key_state = Event_Struct.GetKeyState(SDLK_a);
	if (key_state == KEY_DOWN || key_state == KEY_HOLD) {
		this->Player_Walk(-1);
	}

	key_state = Event_Struct.GetKeyState(SDLK_d);
	if (key_state == KEY_DOWN || key_state == KEY_HOLD) {
		this->Player_Walk(1);
	}
}

void RocketJump::Player_UpdatePos() {
	this->Player.ent.hitbox.pos = this->Player.ent.hitbox.pos + this->Player.ent.vel;
}

void RocketJump::Player_Jump() {
	if (this->Player.ent.on_ground) {
		this->Player.ent.vel.y -= this->Vars.jump_speed;
		std::string str = SFX_JUMP;
		Sound.PlayChunk(str, this->GetVolumeByZoom());
	}
}

void RocketJump::Player_Walk(int dir) {
	if (this->Player.ent.on_ground) {
		if (dir == 1) {
			this->Player.ent.vel.x += (this->Vars.walk_max_speed * this->Vars.walk_acceleration);
			std::min(this->Player.ent.vel.x, this->Vars.walk_max_speed);
		} else if (dir == -1) {
			this->Player.ent.vel.x -= (this->Vars.walk_max_speed * this->Vars.walk_acceleration);
			std::max(this->Player.ent.vel.x, -this->Vars.walk_max_speed);
		}
	} else {
		if (dir == 1) {
			this->Player.ent.vel.x += (this->Vars.strafing_max_speed * this->Vars.strafing_acceleration);
			std::min(this->Player.ent.vel.x, this->Vars.strafing_max_speed);
		} else if (dir == -1) {
			this->Player.ent.vel.x -= (this->Vars.strafing_max_speed * this->Vars.strafing_acceleration);
			std::max(this->Player.ent.vel.x, -this->Vars.strafing_max_speed);
		}
	}
}

void RocketJump::Player_UpdateFrame() {
	if (this->Player.ent.on_ground && (std::abs(this->Player.ent.vel.x) > 0.5)) {
		this->Vars.player_walk_cycle += std::min(1.0f, std::abs(this->Player.ent.vel.x) / 32.0f);
		this->Vars.player_walk_cycle = std::fmod(this->Vars.player_walk_cycle, (float)TEXTURE_PLAYER_WALK_FRAME_COUNT);
	}
	else this->Vars.player_walk_cycle = 0.0f;

	if (!this->Player.ent.on_ground) {
		this->Vars.player_air_cycle += 0.2f;
		this->Vars.player_air_cycle = std::min(std::abs(this->Vars.player_air_cycle),
			                      (float)TEXTURE_PLAYER_AIR_FRAME_COUNT - 1.0f);
	} else this->Vars.player_air_cycle = 0.0f;
}

std::string RocketJump::Player_GetFrame() {
	// on ground
	if (this->Player.ent.on_ground) {
		if (std::abs(this->Player.ent.vel.x) <= 0.5)
			return TEXTURE_PLAYER_IDLE;
		else {
			return PLAYER_WALK_FRAME[(int)this->Vars.player_walk_cycle];
		}
	}
	// in air
	else {
		return PLAYER_AIR_FRAME[(int)this->Vars.player_air_cycle];
	}
}

void RocketJump::Player_CollideWithBrushes() {
	this->CollideWithBrushes(this->Player.ent, false, true);
}

void RocketJump::Player_Shoot() {
	if (this->Player.rocket_delay > 0) {
		return;
	}

	float dx, dy;
	dx = Event_Struct.mouse_event.x - (Event_Struct.window_event.w / 2.0f);
	dy = Event_Struct.mouse_event.y - (Event_Struct.window_event.h / 2.0f);

	float angle = std::atan2(dy, dx);

	std::unique_ptr<Rocket> rocket = std::make_unique<Rocket>(
		Vec2_F(this->Player.ent.hitbox.pos.x + this->Player.ent.hitbox.size.w / 2.0f,
		this->Player.ent.hitbox.pos.y + this->Player.ent.hitbox.size.h / 2.0f),
		angle, this->Vars.rocket_power, this->Vars.explosion_size, this->Vars.rocket_speed);
	this->AddEntity(std::move(rocket));

	this->Player.rocket_delay = this->Vars.rocket_firerate;

	std::string str = SFX_ROCKET_SHOOT;
	Sound.PlayChunk(str, this->GetVolumeByZoom(), 0, true);
}

void RocketJump::LoadLevel(const std::string & level_name) {
	Level level;
	switch (OpenLevel(level_name, level)) {
	case LEVEL_OPEN_SUCCESS:
		this->loaded_level = std::move(level);
		this->Player_Respawn();
		this->Vars.level_transition_timer = this->Vars.level_transition_time;
		this->Vars.level_name = GetLevelName(level_name);
		this->UpdateEpisodeName();
		break;
	default:
		return;
	}
}

void RocketJump::UpdateEpisodeName() {
	if (this->chosen_level == LEVEL_TUTORIAL)
		this->Vars.episode_name = "Tutorial";
	else if (this->chosen_level == LEVEL_EPISODE1)
		this->Vars.episode_name = "Episode 1";
	else if (this->chosen_level == LEVEL_EPISODE2)
		this->Vars.episode_name = "Episode 2";
	else if (this->chosen_level == LEVEL_EPISODE3)
		this->Vars.episode_name = "Episode 3";
}

void RocketJump::CollideWithBrushes(Entity& e, bool bounce, bool is_player) {
	Rect_F& hitbox = e.hitbox;
	Vec2_F& vel = e.vel;

	e.collided = false;
	e.on_ground = false;

	for (auto R : this->loaded_level.brushes) {
		switch (R.type) {
		case BRUSH_SOLID:
			this->CollideWithHitbox(e, R.brush, bounce);
			break;
		case BRUSH_NON_SOLID:
			if (is_player) {
				this->CollideWithHitbox(e, R.brush, bounce);
			}
			break;
		case BRUSH_KILL:
			if (!is_player) {
				this->CollideWithHitbox(e, R.brush, bounce);
				break;
			}
			if (_HitboxCollideWithRect(e.hitbox, R.brush))
				this->Player_Kill();
			break;
		case BRUSH_NEXT_LEVEL:
			if (_HitboxCollideWithRect(e.hitbox, R.brush) && is_player) {
				if (R.key == "") {
					Menu_Manager.screen = 2;
					this->Quit();
					this->Vars.episode_complete = true;
					this->Vars.episode_complete_timer = this->Vars.episode_complete_time;
					break;
				}
				this->LoadLevel(R.key);
			}
			break;
		}
	}
}

void RocketJump::CollideWithHitbox(Entity & e, const Rect_F & R, bool bounce) {
	Rect_F& hitbox = e.hitbox;
	Vec2_F& vel = e.vel;

	BB_COLLISION collision = BoundingBoxCollision(hitbox + vel, R);

	Rect_F guaranteed_rect = Rect_F();
	if (e.vel.x >= 0.0f) {
		guaranteed_rect.pos.x = e.hitbox.pos.x;
		guaranteed_rect.size.w = e.hitbox.size.w + e.vel.x;
	} 
	else {
		guaranteed_rect.pos.x = e.hitbox.pos.x + e.vel.x;
		guaranteed_rect.size.w = e.hitbox.size.w - e.vel.x;
	}

	if (e.vel.y >= 0.0f) {
		guaranteed_rect.pos.y = e.hitbox.pos.y;
		guaranteed_rect.size.h = e.hitbox.size.h + e.vel.y;
	}
	else {
		guaranteed_rect.pos.y = e.hitbox.pos.y + e.vel.y;
		guaranteed_rect.size.h = e.hitbox.size.h - e.vel.y;
	}

	if (_HitboxCollideWithRect(guaranteed_rect, R) == BB_NO_COLLISION)
		return;

	// collision with top of brush
	if ( ((LineLineCollision( Vec2_F(e.hitbox.pos.x, e.hitbox.pos.y + e.hitbox.size.h),
		               Vec2_F(e.hitbox.pos.x, e.hitbox.pos.y + e.hitbox.size.h) + e.vel,
		               Vec2_F(R.pos.x  - e.hitbox.size.w, R.pos.y), Vec2_F(R.pos.x + R.size.w, R.pos.y))
		||
	    LineLineCollision( Vec2_F(e.hitbox.pos.x + e.hitbox.size.w, e.hitbox.pos.y + e.hitbox.size.h),
		               Vec2_F(e.hitbox.pos.x + e.hitbox.size.w, e.hitbox.pos.y + e.hitbox.size.h) + e.vel,
		               Vec2_F(R.pos.x, R.pos.y), Vec2_F(R.pos.x + R.size.w + e.hitbox.size.w, R.pos.y)))
		) &&
		(e.hitbox.pos.y < R.pos.y + R.size.h))
	{
		if ((e.hitbox.pos.x + e.hitbox.size.w != R.pos.x) && (e.hitbox.pos.x != R.pos.x + R.size.w)) {
		float collision_depth = hitbox.pos.y + hitbox.size.h + vel.y - R.pos.y;
		if (collision_depth >= 0.0f) {
			hitbox.pos.y = R.pos.y - hitbox.size.h;
			if (!bounce)
				vel.y = 0.0f;
			else
				vel.y = (-vel.y) * this->Vars.bounce;
				e.collided = true;
				e.on_ground = true;
			}
		}
	}
		
		// collision with bottom of the brush
	else
	if ( ((LineLineCollision( Vec2_F(e.hitbox.pos.x, e.hitbox.pos.y),
		               Vec2_F(e.hitbox.pos.x, e.hitbox.pos.y) + e.vel,
		               Vec2_F(R.pos.x - e.hitbox.size.w, R.pos.y + R.size.h), Vec2_F(R.pos.x + R.size.w, R.pos.y + R.size.h))
		||
	    LineLineCollision( Vec2_F(e.hitbox.pos.x + e.hitbox.size.w, e.hitbox.pos.y),
		               Vec2_F(e.hitbox.pos.x + e.hitbox.size.w, e.hitbox.pos.y ) + e.vel,
		               Vec2_F(R.pos.x, R.pos.y + R.size.h), Vec2_F(R.pos.x + R.size.w + e.hitbox.size.w, R.pos.y + R.size.h)))
		) &&
		    (e.hitbox.pos.y + e.hitbox.size.h > R.pos.y))
	{
		if ((e.hitbox.pos.x + e.hitbox.size.w != R.pos.x) && (e.hitbox.pos.x != R.pos.x + R.size.w)) {
			float collision_depth = R.pos.y + R.size.h - (hitbox.pos.y + vel.y);
			if (collision_depth >= 0.0f) {
				hitbox.pos.y = R.pos.y + R.size.h;
				if (!bounce)
					vel.y = 0.0f;
				else
					vel.y = (-vel.y) * this->Vars.bounce;
				e.collided = true;
			}
		}
	}

	// collision with left size of brush
	if ( ((LineLineCollision(Vec2_F(e.hitbox.pos.x + e.hitbox.size.w, e.hitbox.pos.y),
		               Vec2_F(e.hitbox.pos.x + e.hitbox.size.w, e.hitbox.pos.y) + e.vel,
		               Vec2_F(R.pos.x, R.pos.y - e.hitbox.size.h), Vec2_F(R.pos.x, R.pos.y + R.size.h))
		||
	    LineLineCollision( Vec2_F(e.hitbox.pos.x + e.hitbox.size.w, e.hitbox.pos.y + e.hitbox.size.h),
			       Vec2_F(e.hitbox.pos.x + e.hitbox.size.w, e.hitbox.pos.y + e.hitbox.size.h) + e.vel,
			       Vec2_F(R.pos.x, R.pos.y), Vec2_F(R.pos.x, R.pos.y + R.size.h + e.hitbox.size.h)))
		) &&
		(e.hitbox.pos.x < R.pos.x + R.size.w))
	{
		if ((e.hitbox.pos.y + e.hitbox.size.h != R.pos.y) && (e.hitbox.pos.y != R.pos.y + R.size.h)) {
			float collision_depth = hitbox.pos.x + hitbox.size.w + vel.x - R.pos.x;
			if (collision_depth >= 0.0f) {
				hitbox.pos.x = R.pos.x - hitbox.size.w;
				if (!bounce)
					vel.x = 0.0f;
				else
					vel.x = (-vel.x) * this->Vars.bounce;
				e.collided = true;
			}
		}
	}
	// collision with right size of the brush
	else
	if ( ((LineLineCollision( Vec2_F(e.hitbox.pos.x, e.hitbox.pos.y),
	                       Vec2_F(e.hitbox.pos.x, e.hitbox.pos.y) + e.vel,
		               Vec2_F(R.pos.x + R.size.w, R.pos.y  - e.hitbox.size.h), Vec2_F(R.pos.x + R.size.w, R.pos.y + R.size.h))
		||
	    LineLineCollision( Vec2_F(e.hitbox.pos.x, e.hitbox.pos.y + e.hitbox.size.h),
			       Vec2_F(e.hitbox.pos.x, e.hitbox.pos.y + e.hitbox.size.h) + e.vel,
			       Vec2_F(R.pos.x + R.size.w, R.pos.y), Vec2_F(R.pos.x + R.size.w, R.pos.y + R.size.h + e.hitbox.size.h)))
		) &&
		    (e.hitbox.pos.x + e.hitbox.size.w > R.pos.x))
	{
		if ((e.hitbox.pos.y + e.hitbox.size.h != R.pos.y) && (e.hitbox.pos.y != R.pos.y + R.size.h)) {
			float collision_depth = R.pos.x + R.size.w - (hitbox.pos.x + vel.x);
			if (collision_depth >= 0.0f) {
				hitbox.pos.x = R.pos.x + R.size.w;
				if (!bounce)
					vel.x = 0.0f;
				else
					vel.x = (-vel.x) * this->Vars.bounce;
				e.collided = true;
			}
		}
	}
}

void RocketJump::FreeEntities() {
	this->entities.clear();
}

void RocketJump::AddEntity(std::unique_ptr<Entity_Base> e) {
	this->entities_add_queue.push_back(std::move(e));
}

std::vector<Collision> RocketJump::CollidesWithBrushes(const Entity& e) {
	std::vector<Collision> result = std::vector<Collision>();
	for (auto r : this->loaded_level.brushes) {
		auto R = r.brush;

		Rect_F guaranteed_rect = Rect_F();
		if (e.vel.x >= 0.0f) {
			guaranteed_rect.pos.x = e.hitbox.pos.x;
			guaranteed_rect.size.w = e.hitbox.size.w + e.vel.x;
		} 
		else {
			guaranteed_rect.pos.x = e.hitbox.pos.x + e.vel.x;
			guaranteed_rect.size.w = e.hitbox.size.w - e.vel.x;
		}

		if (e.vel.y >= 0.0f) {
			guaranteed_rect.pos.y = e.hitbox.pos.y;
			guaranteed_rect.size.h = e.hitbox.size.h + e.vel.y;
		}
		else {
			guaranteed_rect.pos.y = e.hitbox.pos.y + e.vel.y;
			guaranteed_rect.size.h = e.hitbox.size.h - e.vel.y;
		}

		if (_HitboxCollideWithRect(guaranteed_rect, R) == BB_NO_COLLISION)
			continue;

		if (	_HitboxCollideWithRect(e.hitbox, R) ||
			(LineLineCollision( Vec2_F(e.hitbox.pos.x, e.hitbox.pos.y + e.hitbox.size.h),
				Vec2_F(e.hitbox.pos.x, e.hitbox.pos.y + e.hitbox.size.h) + e.vel,
				Vec2_F(R.pos.x  - e.hitbox.size.w, R.pos.y), Vec2_F(R.pos.x + R.size.w, R.pos.y)) ||
			LineLineCollision( Vec2_F(e.hitbox.pos.x + e.hitbox.size.w, e.hitbox.pos.y + e.hitbox.size.h),
				Vec2_F(e.hitbox.pos.x + e.hitbox.size.w, e.hitbox.pos.y + e.hitbox.size.h) + e.vel,
				Vec2_F(R.pos.x, R.pos.y), Vec2_F(R.pos.x + R.size.w + e.hitbox.size.w, R.pos.y))) &&
				(e.hitbox.pos.y < R.pos.y + R.size.h) ||
			(LineLineCollision( Vec2_F(e.hitbox.pos.x, e.hitbox.pos.y),
				Vec2_F(e.hitbox.pos.x, e.hitbox.pos.y) + e.vel,
				Vec2_F(R.pos.x - e.hitbox.size.w, R.pos.y + R.size.h), Vec2_F(R.pos.x + R.size.w, R.pos.y + R.size.h)) ||
				LineLineCollision( Vec2_F(e.hitbox.pos.x + e.hitbox.size.w, e.hitbox.pos.y),
					Vec2_F(e.hitbox.pos.x + e.hitbox.size.w, e.hitbox.pos.y ) + e.vel,
					Vec2_F(R.pos.x, R.pos.y + R.size.h), Vec2_F(R.pos.x + R.size.w + e.hitbox.size.w, R.pos.y + R.size.h))) &&
					(e.hitbox.pos.y + e.hitbox.size.h > R.pos.y) ||
			(LineLineCollision(Vec2_F(e.hitbox.pos.x + e.hitbox.size.w, e.hitbox.pos.y),
				Vec2_F(e.hitbox.pos.x + e.hitbox.size.w, e.hitbox.pos.y) + e.vel,
				Vec2_F(R.pos.x, R.pos.y - e.hitbox.size.h), Vec2_F(R.pos.x, R.pos.y + R.size.h)) ||
			 LineLineCollision( Vec2_F(e.hitbox.pos.x + e.hitbox.size.w, e.hitbox.pos.y + e.hitbox.size.h),
					Vec2_F(e.hitbox.pos.x + e.hitbox.size.w, e.hitbox.pos.y + e.hitbox.size.h) + e.vel,
					Vec2_F(R.pos.x, R.pos.y), Vec2_F(R.pos.x, R.pos.y + R.size.h + e.hitbox.size.h)) ||
			(LineLineCollision( Vec2_F(e.hitbox.pos.x, e.hitbox.pos.y),
				Vec2_F(e.hitbox.pos.x, e.hitbox.pos.y) + e.vel,
				Vec2_F(R.pos.x + R.size.w, R.pos.y  - e.hitbox.size.h), Vec2_F(R.pos.x + R.size.w, R.pos.y + R.size.h)) ||
			 LineLineCollision( Vec2_F(e.hitbox.pos.x, e.hitbox.pos.y + e.hitbox.size.h),
						Vec2_F(e.hitbox.pos.x, e.hitbox.pos.y + e.hitbox.size.h) + e.vel,
						Vec2_F(R.pos.x + R.size.w, R.pos.y), Vec2_F(R.pos.x + R.size.w, R.pos.y + R.size.h + e.hitbox.size.h))) &&
							(e.hitbox.pos.x + e.hitbox.size.w > R.pos.x)))
		{
			result.emplace_back(BB_AREA_COLLISION, r.type);
		}
	}

	return result;
}

BB_COLLISION RocketJump::_HitboxCollideWithRect(const Rect_F & hitbox, const Rect_F & rect) {
	return BoundingBoxCollision(hitbox, rect);
}

SDL_Color RocketJump::GetBrushTypeColor(BRUSH_TYPE type) {
	switch (type) {
	case BRUSH_SOLID:
		return this->Vars.BRUSH_SOLID_COLOR;
		break;
	case BRUSH_NON_SOLID:
		return this->Vars.BRUSH_NON_SOLID_COLOR;
		break;
	case BRUSH_KILL:
		return this->Vars.BRUSH_KILL_COLOR;
		break;
	case BRUSH_NEXT_LEVEL:
		return this->Vars.BRUSH_NEXT_LEVEL_COLOR;
		break;
	case BRUSH_DECAL:
		break;
	}

	return { 0xff , 0xff , 0xff , 0xff};
}

Collision::Collision(BB_COLLISION c_type, BRUSH_TYPE b_type): collision_type(c_type), brush_type(b_type) { ; }
