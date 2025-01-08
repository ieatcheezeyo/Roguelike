#pragma once

#include <iostream>

#include <SDL.h>

class Tile {

public:
	Tile(char symbol, SDL_Texture* texture);
	~Tile();

	SDL_Texture* texture;
	char symbol;

};

