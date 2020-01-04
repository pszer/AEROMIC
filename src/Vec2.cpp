#include "Vec2.hpp"

/*
	Vec2_F
*/

Vec2_F operator+(const Vec2_F& a, const Vec2_F& b) {
	return Vec2_F(a.x + b.x, a.y + b.y);
}

Vec2_F operator-(const Vec2_F& a, const Vec2_F& b) {
	return Vec2_F(a.x - b.x, a.y - b.y);
}

Vec2_F operator*(const Vec2_F& v, const float s) {
	return Vec2_F(v.x * s, v.y * s);
}

Vec2_F operator/(const Vec2_F& v, const float s) {
	return Vec2_F(v.x / s, v.y / s);
}

bool operator==(const Vec2_F & a, const Vec2_F & b) {
	return (a.x == b.x) && (a.y == b.y);
}

bool operator!=(const Vec2_F & a, const Vec2_F & b) {
	return !(a == b);
}

/*

	Rect_F

*/

Rect_F Rect_F::ScaleFromCenter(const float s) {
	const float half_w = this->size.w / 2.0f,
		half_h = this->size.h / 2.0f;
	Rect_F result = Rect_F(this->pos.x - half_w * (s - 1.0f),
		this->pos.y - half_h * (s - 1.0f),
		this->size.w * s, this->size.h * s);
	return result;
}

SDL_Rect Rect_F::ToSDLRect() {
	SDL_Rect result = {
		static_cast<int>(std::floor(this->pos.x)),
		static_cast<int>(std::floor(this->pos.y)),
		static_cast<int>(std::ceil(this->size.w)),
		static_cast<int>(std::ceil(this->size.h))
	};
	return result;
}

Rect_F Rect_F::Absolute() {
	Rect_F r = *this;
	if (r.size.w < 0.0f) {
		r.pos.x = r.pos.x + r.size.w;
		r.size.w *= -1.0f;
	}

	if (r.size.h < 0.0f) {
		r.pos.y = r.pos.y + r.size.h;
		r.size.h *= -1.0f;
	}

	return r;
}

Rect_F operator+(const Rect_F& rect, const Vec2_F& translate) {
	return Rect_F(Vec2_F(rect.pos.x + translate.x, rect.pos.y + translate.y), rect.size);
}

Rect_F operator-(const Rect_F& rect, const Vec2_F& translate) {
	return Rect_F(Vec2_F(rect.pos.x - translate.x, rect.pos.y - translate.y), rect.size);
}