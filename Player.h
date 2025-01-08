#pragma once

#include <iostream>
#include <vector>
#include <cmath>

#include <SDL.h>

#include "Types.h"
#include "InputManager.h"

class Player {
public:
	Player(int x, int y, SDL_Texture* p_texture);
	~Player();

	void update(std::vector<std::vector<char>> mapData, InputManager& pad, double dt);
	Vector2 getPosition();
	void findSpawnPoint(std::vector<std::vector<char>> mapData);

	int w, h;
	Vector2 position;
	Vector2 targetPosition;
	float moveSpeed;
	SDL_Texture* texture;

private:
	double holdTimer;
	double holdDelay;
	double repeatDelay;
	bool isHolding;
};

