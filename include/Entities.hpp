#pragma once

#include "Vec2.hpp"

struct Entity {
	Rect_F hitbox;
	Vec2_F vel;

	bool collided = false;
	bool on_ground = false;
};

class Entity_Base {
public:
	Entity_Base() = default;
	Entity_Base(Rect_F H, Vec2_F V) {
		ent.hitbox = H;
		ent.vel = V;
	}

	virtual void Update() { ; }

	Entity ent;
	bool to_delete = false;
};

struct Rocket : Entity_Base {
	Rocket(Vec2_F source, float angle, float power, float size, float speed);
	void Update();

	float power, size, angle;
};

struct Explosion : Entity_Base {
	Explosion(Vec2_F source, float _size, float _power);
	void Update();

	float power, size;
};

enum PARTICLE_BEHAVIOUR { BEHAVIOUR_NONE, BEHAVIOUR_DEL_ON_COLLIDE, BEHAVIOUR_COLLIDE };

struct Particle : Entity_Base {
	Particle(Vec2_F source, Vec2_F size, float angle, float speed, int lifespan, PARTICLE_BEHAVIOUR behaviour,
		bool gravity, SDL_Color color);
	void Update();

	int lifespan, age = 0;

	PARTICLE_BEHAVIOUR behaviour = BEHAVIOUR_NONE;
	bool gravity = false;

	SDL_Color color;
};

#include "RocketJump.hpp"