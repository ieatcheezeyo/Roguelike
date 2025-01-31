#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <utility>
#include <unordered_map>

#include <SDL.h>

#include "Types.h"
#include "InputManager.h"
#include "Item.h"
#include "Items.h"
#include "Inventory.h"
#include "Map.h"
#include "Enemy.h"
#include "TextConsole.h"
#include "AudioManager.h"

class Map;
class Enemy;

class Player {
public:
	Player(int x, int y, int scale, SDL_Texture* p_texture, AudioManager* audio);
	~Player();

	void update(Map& map, InputManager& pad, TextConsole& console, std::vector<Enemy*>& enemies, double dt);
	Vector2 getPosition();
	void findSpawnPoint(std::vector<std::vector<char>> mapData);

	int w, h;
	Vector2 position;
	Vector2 targetPosition;
	float moveSpeed;
	SDL_Texture* texture;

	int scale;

	bool hasMoved();
	int getHealth();
	int getGold();

	void takeDamage(int damage);
	void refillHealth(int ammount);

	bool isControllable;

	Inventory inventory;

	int atk;
	int def;
	int baseAtk;
	int baseDef;

private:
	std::vector<char> walkableTiles = { ' ', '.' };
	std::unordered_map<Item*, bool> messageShownForItemThisFrame;
	int health;
	int gold;
	double holdTimer;
	double holdDelay;
	double repeatDelay;
	bool isHolding;
	bool justMoved;
	AudioManager* audio;
};

