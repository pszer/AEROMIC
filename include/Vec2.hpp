#pragma once

#include <cmath>

#ifdef _WIN32
#include "SDL_rect.h"
#elif defined __linux__
#include <SDL2/SDL_rect.h>
#endif

struct Vec2_F {
	Vec2_F() = default;
	Vec2_F(float _x, float _y) : x(_x), y(_y) { ; }

	union {
		float x, w;
	};

	union {
		float y, h;
	};
};

Vec2_F operator+(const Vec2_F& a, const Vec2_F& b);
Vec2_F operator-(const Vec2_F& a, const Vec2_F& b);
Vec2_F operator*(const Vec2_F& v, const float s);
Vec2_F operator/(const Vec2_F& v, const float s);

bool operator==(const Vec2_F& a, const Vec2_F& b);
bool operator!=(const Vec2_F& a, const Vec2_F& b);

struct Rect_F {
	Rect_F() = default;
	Rect_F(Vec2_F _pos, Vec2_F _size): pos(_pos), size(_size) { ; }
	Rect_F(float x, float y, float w, float h) : pos(Vec2_F(x, y)), size(Vec2_F(w, h)) { ; }

	Vec2_F pos, size;

	SDL_Rect ToSDLRect();
	Rect_F Absolute();
	Rect_F ScaleFromCenter(const float s);
};

Rect_F operator+(const Rect_F& rect, const Vec2_F& translate);
Rect_F operator-(const Rect_F& rect, const Vec2_F& translate);