#include "Hitreg.hpp"

bool PointRectangleCollision(const Vec2_F& vec, const Rect_F& rect) {
	if ((vec.x >= rect.pos.x) &&
		(vec.x <= rect.pos.x + rect.size.w) &&
		(vec.y >= rect.pos.y) &&
		(vec.y <= rect.pos.y + rect.size.h))
	{
		return true;
	}
	return false;
}

bool LineLineCollision(const Vec2_F & a, const Vec2_F & b, const Vec2_F & c, const Vec2_F & d) {
	if ((a == c) || (a == d) || (b == c) || (b == d))
		return true;

	float uA, uB;
	uA = ((d.x-c.x)*(a.y-c.y) - (d.y-c.y)*(a.x-c.x)) / ((d.y-c.y)*(b.x-a.x) - (d.x-c.x)*(b.y-a.y));
	uB = ((b.x-a.x)*(a.y-c.y) - (b.y-a.y)*(a.x-c.x)) / ((d.y-c.y)*(b.x-a.x) - (d.x-c.x)*(b.y-a.y)); 

	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
		return true;
	}

	return false;
}

bool LineRectangleCollision(const Vec2_F & a, const Vec2_F & b, const Rect_F & rect) {
	if (LineLineCollision(a, b, Vec2_F(rect.pos.x, rect.pos.y), Vec2_F(rect.pos.x, rect.pos.y + rect.size.h)) ||
		LineLineCollision(a, b, Vec2_F(rect.pos.x, rect.pos.y), Vec2_F(rect.pos.x + rect.size.w, rect.pos.y)) ||
		LineLineCollision(a, b, Vec2_F(rect.pos.x + rect.size.w, rect.pos.y + rect.size.h),
			Vec2_F(rect.pos.x, rect.pos.y + rect.size.h)) ||
		LineLineCollision(a, b, Vec2_F(rect.pos.x + rect.size.w, rect.pos.y + rect.size.h),
			Vec2_F(rect.pos.x + rect.size.w, rect.pos.y)))
	{
		return true;
	}

	return false;
}

BB_COLLISION BoundingBoxCollision(const Rect_F& a, const Rect_F& b) {
	if (!(b.pos.x > a.pos.x + a.size.w
		|| b.pos.x + b.size.w < a.pos.x
		|| b.pos.y > a.pos.y + a.size.h
		|| b.pos.y + b.size.h < a.pos.y))
	{
		if ((a.pos.y + a.size.h == b.pos.y) ||
		    (b.pos.y + b.size.h == a.pos.y) ||
		    (a.pos.x + a.size.w == b.pos.x) ||
		    (b.pos.x + b.size.w == a.pos.x))
		{
			return BB_EDGE_COLLISION;
		}

		return BB_AREA_COLLISION;
	}

	return BB_NO_COLLISION;
}