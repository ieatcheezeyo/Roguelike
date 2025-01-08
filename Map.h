#pragma once

#include <vector>
#include <cstdlib>
#include <ctime>
#include <queue>

#include <SDL.h>
#include <SDL_image.h>

#include "Tile.h"

class Map {
public:
	Map(int w, int h, SDL_Renderer* renderer);
	~Map();

	void setTile(int x, int y, char symbol);
	char getTileSymbol(int x, int y) const;
	void generateDungeon();

	std::vector<SDL_Texture*> textures;
	std::vector<Tile> tiles;
	std::vector<std::vector<char>> mapData;

private:
	int w, h;
	SDL_Renderer* renderer;
	SDL_Texture* loadTileTexture(const char* file);
	
	std::vector<std::pair<int, int>> roomCenters;

	struct BSPNode {
		int x, y, w, h;
		BSPNode* left;
		BSPNode* right;

		BSPNode(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h), left(nullptr), right(nullptr) {}
	};

	void splitBSP(BSPNode* node, int minRoomSize, int maxRoomSize);
	void createRoom(BSPNode* node, int minRoomSize, int maxRoomSize);
	void connectRooms();

};
