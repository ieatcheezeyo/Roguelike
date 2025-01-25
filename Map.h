#pragma once

#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>
#include <queue>
#include <numeric>
#include <functional>
#include <tuple>
#include <unordered_map>
#include <bitset>
#include <fstream>

#include <SDL.h>
#include <SDL_image.h>

#include "BitMaskToTile.h"

#include "Tile.h"
#include "Items.h"

class Map {
public:
	Map(int w, int h, int scale, SDL_Renderer* renderer);
	~Map();

	void setTile(int x, int y, char symbol);
	char getTileSymbol(int x, int y) const;
	void generateDungeon(Uint64 seed);
	int floodFillAndCount(int startX, int startY);
	void removeSmallAreas();
	void fillEdges();
	void drunkardsWalk(int x1, int y1, int x2, int y2);
	void connectAreas();
	void carvePath(int x1, int y1, int x2, int y2);
	void applyBitmasking();
	void ensureConnectivity();
	void cleanIsolatedTiles();
	void floodFillAssignRegion(int x, int y, std::vector<std::vector<int>>& regionMap, int regionId);

	std::vector<SDL_Texture*> textures;
	std::vector<Tile> tiles;
	std::vector<std::vector<char>> mapData;
	std::vector<std::tuple<int, int, int, int>> areaRegions;
	std::vector<std::vector<bool>> visited;
	int currentAreaID;

	int scale;

	Items items;

private:
	int w, h;

	void randomizeGroundTiles();

	void spawnItems();

	SDL_Renderer* renderer;
	SDL_Texture* loadTileTexture(const char* file);
};
