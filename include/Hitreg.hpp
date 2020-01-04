#pragma once

#include <algorithm>

#include "Vec2.hpp"

// returns true if collision
bool PointRectangleCollision(const Vec2_F& vec, const Rect_F& rect);

bool LineLineCollision(const Vec2_F& a, const Vec2_F& b, const Vec2_F& c, const Vec2_F& d);
bool LineRectangleCollision(const Vec2_F& a, const Vec2_F& b, const Rect_F& rect);

enum BB_COLLISION { BB_NO_COLLISION , BB_EDGE_COLLISION , BB_AREA_COLLISION };
BB_COLLISION BoundingBoxCollision(const Rect_F& a, const Rect_F& b);