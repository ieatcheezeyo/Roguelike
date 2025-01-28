#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>

#include <SDL.h>
#include <SDL_image.h>

#include "Items.h"
#include "Types.h"

enum EnemyType {
	Rat,
	Bat,
	Ghost
};

typedef struct {
	int def;
	int atk;
	int speed;
} EnemyStatSet;

class Enemy {

public:
	Enemy(SDL_Renderer* renderer);
	~Enemy();

	Enemy* createEnemy(std::vector<std::vector<char>> mapData);
	void update(std::vector<std::vector<char>>& mapData, float dt);

	EnemyType type;
	const char* enemyDescriptor;
	EnemyStatSet enemyStats;
	SDL_Texture* texture;
	int scale;
	Vector2 position;
	int w, h;
	std::vector<Items*> EnemyLoot;

private:

	SDL_Texture* ratTexture;
	SDL_Texture* batTexture;
	SDL_Texture* ghostTexture;

	std::unordered_map<EnemyType, SDL_Texture*> textureToEnemyTypeMap;
	SDL_Renderer* renderer;


};

