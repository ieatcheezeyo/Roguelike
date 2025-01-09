#pragma once

typedef struct {
	int x;
	int y;
} Vector2;

struct Vector2f {
	float x;
	float y;
	Vector2f(float p_x = 0.0f, float p_y = 0.0f) : x(p_x), y(p_y) {}
};