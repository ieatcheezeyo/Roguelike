#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>

#include <SDL.h>
#include <SDL_image.h>

#include "Items.h"
#include "Types.h"
#include "Map.h"
#include "Player.h"

class Map;
class Player;

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
	void update(std::vector<std::vector<char>>& mapData, Player& player, std::vector<Enemy*>& enemies, double dt);

	EnemyType type;
	const char* enemyDescriptor;
	EnemyStatSet enemyStats;
	SDL_Texture* texture;
	int scale;
	Vector2 position;
	int w, h;
	std::string name;
	bool alerted;
	std::vector<Items*> EnemyLoot;
	int fovRadius;
	int playerStepsWithinFOV;

private:
	Map& map;

	SDL_Texture* ratTexture;
	SDL_Texture* batTexture;
	SDL_Texture* ghostTexture;
	SDL_Texture* gobblinTexture;

	std::unordered_map<EnemyType, SDL_Texture*> textureToEnemyTypeMap;
	SDL_Renderer* renderer;

};

