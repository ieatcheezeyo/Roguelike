#pragma once

#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>
#include <queue>
#include <tuple>

#include <SDL.h>
#include <SDL_image.h>

#include "Tile.h"

class Map {
public:
	Map(int w, int h, SDL_Renderer* renderer);
	~Map();

	void setTile(int x, int y, char symbol);
	char getTileSymbol(int x, int y) const;
	void generateDungeon(Uint64 seed);
	int floodFillAndCount(int startX, int startY);
	void removeSmallAreas();

	std::vector<SDL_Texture*> textures;
	std::vector<Tile> tiles;
	std::vector<std::vector<char>> mapData;
	std::vector<std::tuple<int, int, int, int>> areaRegions;
	int currentAreaID = 1;

private:
	int w, h;
	SDL_Renderer* renderer;
	SDL_Texture* loadTileTexture(const char* file);
};
