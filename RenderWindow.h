#pragma once

#include <iostream>
#include <vector>
#include <cstdarg>
#include <cstdio>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>

#include "Types.h"
#include "Player.h"
#include "Map.h"

//////////////////////////////////////////////////////////////////
// FILE   : RenderWindow Class
// AUTHOR : I_EAT_CHEEZE_YO
// DATE   : 01/07/2025
// 
//////////////////////////////////////////////////////////////////

class RenderWindow {

public:
	RenderWindow(const char* title, int w, int h);
	~RenderWindow();

	SDL_Texture* loadTexture(const char* file);

	void tick();

	double dt() {
		return deltaTime;
	}

	double getFPS() {
		return fps;
	}

	void clear();

	void blit(int x, int y, SDL_Texture* texture);
	void blit(Map& map);
	void blit(Player& player);

	void print(int x, int y, const char* text);
	void printf(int x, int y, const char* format, ...);

	void flip();

	int getScreenWidth();
	int getScreenHeight();
	SDL_Renderer* getRenderer();

	void setBackgroundColor(Uint8 r, Uint8 g, Uint8 b);
	void setFontColor(Uint8 r, Uint8 g, Uint8 b);

private:
	//Window And Renderer
	SDL_Window* window;
	SDL_Renderer* renderer;

	int screen_w, screen_h;

	SDL_Color backgroundColor;

	//Textures
	std::vector<SDL_Texture*> textures;

	//Font
	TTF_Font* font;
	int fontSize = 24;
	SDL_Color fontColor;

	//Timing
	Uint64 previousTime;
	Uint64 currentTime;
	double deltaTime;
	double fps;

	// Private Functions
	void clean();

};

