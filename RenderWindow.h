#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstdarg>
#include <cstdio>

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>

#include "Types.h"
#include "Player.h"
#include "Map.h"
#include "Items.h"
#include "Inventory.h"
#include "Enemy.h"

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

	Mix_Chunk* loadSFX(const char* name, const char* file);
	void playSFX(const char* name);

	TTF_Font* createFont(const char* file, int size);

	void setCameraPosition(float x, float y);
	void moveCamera(float dx, float dy);
	void cameraShake(float amplitude, float duration);
	void cameraFlash(Uint8 r, Uint8 g, Uint8 b, float duration);
	void setCameraBounds(float w, float h);
	void setViewportSize(float w, float h);
	void setCameraSpeed(float speed);
	void cameraFollow(Player& player);

	float getCameraX();
	float getCameraY();
	int getViewportWidth();
	int getViewPortHeight();

	void clear();

	void blit(int x, int y, SDL_Texture* texture);
	void blitUiElement(int x, int y, SDL_Texture* texture);
	void blitUiElement(int x, int y, int w, int h, SDL_Texture* texture);
	void blit(Map& map);
	void blit(int x, int y, Map& minimap);
	void blit(Player& player);
	void blit(Items& items);
	void blit(Inventory& inventory, int startX, int startY, int slotWidth, int slotHeight);

	void print(int x, int y, const char* text);
	void printf(int x, int y, const char* format, ...);

	void drawRect(const char* mode, int x, int y, int w, int h);

	void flip();

	int getScreenWidth();
	int getScreenHeight();
	SDL_Renderer* getRenderer();

	void setBackgroundColor(Uint8 r, Uint8 g, Uint8 b);
	void setDrawColor(Uint8 r, Uint8 g, Uint8 b);
	void setFontColor(Uint8 r, Uint8 g, Uint8 b);
	void setFont(TTF_Font* newFont);
	void setFont();

private:
	//Window And Renderer
	SDL_Window* window;
	SDL_Renderer* renderer;

	int screen_w, screen_h;

	SDL_Color backgroundColor;
	SDL_Color drawColor;

	//Textures
	std::vector<SDL_Texture*> textures;

	//Font
	TTF_Font* defaultFont;
	TTF_Font* font;
	int fontSize = 24;
	SDL_Color fontColor;
	std::vector<TTF_Font*> userFonts;

	//SFX
	std::unordered_map<const char*, Mix_Chunk*> sfx;

	//Camera
	Vector2f cameraPosition;
	Vector2f viewportSize;
	Vector2f sceneBounds = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
	Vector2f originalCameraPosition;
	SDL_Color cameraFlashColor = { 255, 255, 255, 255 };
	float cameraSpeed;
	float cameraShakeAmplitude = 0.0f;
	float cameraShakeDuration = 0.0f;
	float cameraShakeTimeRemaining = 0.0f;
	float cameraFlashAlpha = 255.0f;
	float cameraFlashDuration = 0.0f;
	float cameraFlashTimeRemaining = 0.0f;
	
	SDL_Rect applyCameraOffset(float x, float y, int w, int h);
	SDL_Rect applyCameraOffset(int x, int y, int w, int h);
	void applyCameraShake();
	void applyCameraFlash();

	//Timing
	Uint64 previousTime;
	Uint64 currentTime;
	double deltaTime;
	double fps;

	// Private Functions
	void clean();

	//UI
	SDL_Texture* InventoryContainerTexture;
	SDL_Texture* InventoryContainerSelectedTexture;
	SDL_Texture* InventoryContainerDescriptionTexture;
	SDL_Texture* cursedItemIndecator;

	//Button Prompts
	std::unordered_map<std::string, SDL_Texture*> buttonPromptTextures;

};

