#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>

#include <SDL.h>
#include <SDL_image.h>

#include "Items.h"
#include "Types.h"
#include "Map.h"

class Map;

enum EnemyType {
	Rat,
	Bat,
	Ghost,
	Gobblin,
	COUNT
};

typedef struct {
	int def;
	int atk;
	int speed;
} EnemyStatSet;

class Enemy {

public:
	Enemy(SDL_Renderer* renderer, Map& map);
	~Enemy();

	Enemy* createEnemy(std::vector<std::vector<char>> mapData);
	void update(std::vector<std::vector<char>>& mapData, double dt);

	EnemyType type;
	const char* enemyDescriptor;
	EnemyStatSet enemyStats;
	SDL_Texture* texture;
	int scale;
	Vector2 position;
	int w, h;
	bool alerted;
	std::vector<Items*> EnemyLoot;

private:
	Map& map;

	SDL_Texture* ratTexture;
	SDL_Texture* batTexture;
	SDL_Texture* ghostTexture;
	SDL_Texture* gobblinTexture;

	std::unordered_map<EnemyType, SDL_Texture*> textureToEnemyTypeMap;
	SDL_Renderer* renderer;

};

