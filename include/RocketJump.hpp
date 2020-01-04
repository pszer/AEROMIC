#pragma once

#include <vector>
#include <cmath>
#include <algorithm>
#include <memory>
#include <string>
#include <array>

#include "Levels.hpp"
#include "Event.hpp"
#include "Camera.hpp"
#include "Hitreg.hpp"
#include "Entities.hpp"
#include "Menu.hpp"
#include "Sound.hpp"

#define LEVEL_TUTORIAL "tutorial"
#define LEVEL_EPISODE1 "e1m1"
#define LEVEL_EPISODE2 "e2m1"
#define LEVEL_EPISODE3 "e3m1"

#define SFX_WIND "sfx\\windfly.wav"
#define SFX_ROCKET_SHOOT "sfx\\shoot.wav"
#define SFX_ROCKET_EXPLODE "sfx\\explode.wav"
#define SFX_JUMP "sfx\\jump.wav"
#define SFX_DEATH_PREFIX "sfx\\death"
#define SFX_GIB "sfx\\gib.wav"

#define TEXTURE_PLAYER_IDLE "img\\p_idle.png"
#define TEXTURE_PLAYER_RL "img\\p_rl.png"
#define TEXTURE_PLAYER_RL2 "img\\p_rl2.png"
#define TEXTURE_PLAYER_RL3 "img\\p_rl3.png"
#define TEXTURE_PLAYER_WALK_FRAME_COUNT 5
extern const std::string PLAYER_WALK_FRAME[TEXTURE_PLAYER_WALK_FRAME_COUNT];
#define TEXTURE_PLAYER_AIR_FRAME_COUNT 3
extern const std::string PLAYER_AIR_FRAME[TEXTURE_PLAYER_AIR_FRAME_COUNT];

struct Collision {
	Collision() = default;
	Collision(BB_COLLISION c_type, BRUSH_TYPE b_type);
	BB_COLLISION collision_type;
	BRUSH_TYPE brush_type;
};

extern struct RocketJump {
	struct Player {
		Entity ent;

		bool knocked = false,
			dead = false,
			respawn = false;

		int rocket_delay = 0;

		std::array<Vec2_F, 8> prev_pos;
	} Player;

	void Init();
	void Quit();
	void Update();
	
	void Player_Kill();
	void Player_Respawn();

	void Player_Update();
	void Player_UpdateVel();
	void Player_UpdatePos();

	void Player_CollideWithBrushes();
	void Player_Shoot();
	void Player_Jump();
	void Player_Walk(int dir);

	void Player_UpdateFrame();
	std::string Player_GetFrame();

	struct Vars {
		float	gravity = 0.50f,
			air_resistance = 0.99f,
			ground_resistance = 0.75f,

			min_zoom = 0.1f,
			max_zoom = 10.00f,

			jump_speed = 9.00f,
			walk_max_speed = 8.00f,
			walk_acceleration = 0.25f,
			strafing_max_speed = 2.50f,
			strafing_acceleration = 0.12f,
			rocket_speed = 25.00f,
			rocket_power = 15.00f,
			explosion_size = 150.00f,

			bounce = 0.8f;

		int	tick = 0,
			respawn_timer = 0,

			level_transition_timer = 0,
			level_transition_time = 40,

			episode_complete_timer = 0,
			episode_complete_time = 150,

			rocket_firerate = 20,

			explosion_debris_count = 5,
			explosion_particle_count = 50,
			death_gore_count = 50;

		bool episode_complete = false;

		Rect_F rl_box = Rect_F(-9.7f, -6.1f, 40.60f, 40.60f);
		float player_walk_cycle = 0.0f,
			player_air_cycle = 0.0f;

		std::string episode_name = "", level_name = "";

		SDL_Color BRUSH_SOLID_COLOR =		{ 0x00, 0x00, 0x00, 0xff },
			BRUSH_NON_SOLID_COLOR =		{ 0x40, 0x40, 0x40, 0xff },
			BRUSH_KILL_COLOR =		{ 0xff, 0x00, 0x00, 0xff },
			BRUSH_NEXT_LEVEL_COLOR =	{ 0x00, 0xff, 0x00, 0xff },

			SMOKE_COLOR = { 32 , 32 , 32 , 64} ,
			EXPLOSION_COLOR = { 245, 204, 0, 128 },
			EXPLOSION_COLOR_ALT = { 255, 50, 0, 128 };
	} Vars;

	std::string chosen_level = LEVEL_TUTORIAL;
	void LoadLevel(const std::string& level_name);
	Level loaded_level;
	void UpdateEpisodeName();

	// These functions clip entity vectors and positions
	void CollideWithBrushes(Entity&, bool bounce = false, bool is_player = false);
	void CollideWithHitbox(Entity&, const Rect_F&, bool bounce = false);

	std::vector<std::unique_ptr<Entity_Base>> entities;
	std::vector<std::unique_ptr<Entity_Base>> entities_add_queue;
	void FreeEntities();
	void AddEntity(std::unique_ptr<Entity_Base>);

	// Returns whether or not the entity collides with a brush
	// doesnt clip any movement
	std::vector<Collision> CollidesWithBrushes(const Entity&);
	BB_COLLISION _HitboxCollideWithRect(const Rect_F& hitbox, const Rect_F& rect);

	SDL_Color GetBrushTypeColor(BRUSH_TYPE);
	
	void UpdateSound();
	void WindSound(int vol = -1);

	void UpdateCamera();
	int GetVolumeByZoom();
	int wind_channel = 0;
} RocketJump;