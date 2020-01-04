#include "Entities.hpp"

Rocket::Rocket(Vec2_F source, float angle, float power, float size, float speed) {
	this->ent.hitbox.size.w = 4.0f;
	this->ent.hitbox.size.h = 4.0f;

	this->ent.hitbox.pos.x = source.x - this->ent.hitbox.size.w / 2.0f;
	this->ent.hitbox.pos.y = source.y - this->ent.hitbox.size.h / 2.0f;

	this->angle = angle;
	this->ent.vel = Vec2_F(std::cos(angle), std::sin(angle));
	this->ent.vel = this->ent.vel * speed;

	this->power = power;

	this->size = size;
}

void Rocket::Update() {
	Vec2_F smoke_pos = this->ent.hitbox.pos + this->ent.hitbox.size / 2.0f;
	this->ent.hitbox = this->ent.hitbox + this->ent.vel;

	std::unique_ptr<Particle> smoke = std::make_unique<Particle>(
		smoke_pos, Vec2_F(6.0f, 6.0f), (std::rand() / 1000.0f), 1.2f, 50, BEHAVIOUR_NONE,
			false, RocketJump.Vars.SMOKE_COLOR
		);
	RocketJump.AddEntity(std::move(smoke));
	smoke = std::make_unique<Particle>(
		smoke_pos, Vec2_F(6.0f, 6.0f), (std::rand() / 1000.0f), 1.2f, 50, BEHAVIOUR_NONE,
		false, RocketJump.Vars.SMOKE_COLOR
		);
	RocketJump.AddEntity(std::move(smoke));

	Vec2_F temp_vel = this->ent.vel;
	std::vector<Collision> collisions = RocketJump.CollidesWithBrushes(this->ent);
	for (auto c : collisions) {
		if (c.brush_type == BRUSH_SOLID || c.brush_type == BRUSH_KILL) {
			RocketJump.CollideWithBrushes(this->ent, false, false);

			this->to_delete = true;

			std::unique_ptr<Explosion> explosion = std::make_unique<Explosion>(
				this->ent.hitbox.pos, this->size, this->power);
			RocketJump.AddEntity(std::move(explosion));

			for (int i = 0; i < RocketJump.Vars.explosion_debris_count; ++i) {
				SDL_Color color = RocketJump.GetBrushTypeColor(c.brush_type);
				std::unique_ptr<Particle> particle = std::make_unique<Particle>(
					this->ent.hitbox.pos - temp_vel, Vec2_F(6.0f, 6.0f),
					-this->angle + std::fmod((std::rand() / 1000.0f), 90.0f) - 45.0f,
					15.0f * ((std::rand() % 1000) / 2000.0f + 0.5f), 60 + std::rand() % 120, BEHAVIOUR_COLLIDE, true, color);
				RocketJump.AddEntity(std::move(particle));
			}

			for (int i = 0; i < RocketJump.Vars.explosion_particle_count; ++i) {
				SDL_Color c = RocketJump.Vars.EXPLOSION_COLOR;

				bool alt_part = i >= RocketJump.Vars.explosion_particle_count / 2;
				if (alt_part) 
					c = RocketJump.Vars.EXPLOSION_COLOR_ALT;
				else
					c = RocketJump.Vars.EXPLOSION_COLOR;

				std::unique_ptr<Particle> particle = std::make_unique<Particle>(
					this->ent.hitbox.pos, Vec2_F(15.0f, 15.0f), std::rand() / 1000.0f,
					0.05f + ((std::rand() % 1000) / 2000.0f) * (alt_part ? 3.0f : 5.0f),
					90 + std::rand() % 10, BEHAVIOUR_NONE, false, c);
				RocketJump.AddEntity(std::move(particle));
			}

			std::string str = SFX_ROCKET_EXPLODE;
			Sound.PlayChunk(str, RocketJump.GetVolumeByZoom(), 0, true);

			break;
		}
	}
}

Explosion::Explosion(Vec2_F source, float _size, float _power) {
	this->ent.hitbox = Rect_F(source, Vec2_F(0.0f, 0.0f));
	this->power = _power;
	this->size = _size;
}

void Explosion::Update() {
	this->to_delete = true;

	Vec2_F diff = this->ent.hitbox.pos - (RocketJump.Player.ent.hitbox.pos + (RocketJump.Player.ent.hitbox.size / 2.0f));
	float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

	if (distance >= this->size)
		return;

	float distance_over_size = distance / this->size;
	float push_force = this->power * (1 - (distance_over_size * distance_over_size * distance_over_size));

	Vec2_F vel_force = (diff / distance) * push_force;
	RocketJump.Player.ent.vel = RocketJump.Player.ent.vel - vel_force;
	RocketJump.Player.knocked = true;
}

Particle::Particle(Vec2_F source, Vec2_F size, float angle, float speed, int lifespan, PARTICLE_BEHAVIOUR behaviour, bool gravity, SDL_Color color) {
	this->ent.hitbox = Rect_F(source - (size / 2.0f), size);
	this->ent.vel = Vec2_F(std::cos(angle), std::sin(angle));
	this->ent.vel = this->ent.vel * speed;

	this->lifespan = lifespan;
	this->behaviour = behaviour;
	this->gravity = gravity;
	this->color = color;
}

void Particle::Update() {
	++this->age;
	if (this->age > this->lifespan) {
		this->to_delete = true;
		return;
	}

	std::vector<Collision> collisions;
	switch (this->behaviour) {
	case BEHAVIOUR_NONE:
		break;
	case BEHAVIOUR_DEL_ON_COLLIDE:
		collisions = RocketJump.CollidesWithBrushes(this->ent);
		for (auto c : collisions) {
			if (c.brush_type == BRUSH_SOLID || c.brush_type == BRUSH_KILL) {
				this->to_delete = true;
				break;
			}
		}
		break;
	case BEHAVIOUR_COLLIDE:
		RocketJump.CollideWithBrushes(this->ent, true, false);
		break;
	}

	this->ent.hitbox.pos = this->ent.hitbox.pos + this->ent.vel;
	if (this->gravity) {
		this->ent.vel.y += RocketJump.Vars.gravity;
	}
	this->ent.vel = this->ent.vel * RocketJump.Vars.air_resistance;
}
