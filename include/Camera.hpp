#pragma once

extern struct Camera {
	// camera centre
	float x = 0.0f, y = 0.0f;

	// 1.0f = xy 1:1 pixels
	// 2.0f = xy 1:2 pixels
	float zoom = 1.0f;

	// amount in pixels to random shake by
	float shake = 0.0f;
} Camera;