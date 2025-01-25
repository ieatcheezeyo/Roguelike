#include "RenderWindow.h"

RenderWindow::RenderWindow(const char* title, int w, int h) {
	screen_w = w;
	screen_h = h;
	
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
	if (!window) {
		std::cout << "SDL_CreateWindow() Failed: " << SDL_GetError() << std::endl;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		std::cout << "SDL_CreateRenderer() Failed: " << SDL_GetError() << std::endl;
	}

	font = TTF_OpenFont("Assets/Fonts/default.ttf", fontSize);
	if (!font) {
		std::cout << "TTF_OpenFont() Failed: " << TTF_GetError() << std::endl;
	}

	backgroundColor = { 0, 0, 0, 255 };
	fontColor = { 255, 255, 255, 0 };

	previousTime = 0;
	currentTime = 0;
	deltaTime = 0;
	fps = 0;

}

RenderWindow::~RenderWindow() {
	clean();
}

SDL_Texture* RenderWindow::loadTexture(const char* file) {
	SDL_Texture* tempTexture = IMG_LoadTexture(renderer, file);
	if (!tempTexture) {
		std::cout << "IMG_LoadTexture() Failed: " << IMG_GetError() << std::endl;
		return nullptr;
	}

	textures.push_back(tempTexture);
	return textures.back();
}

Mix_Chunk* RenderWindow::loadSFX(const char* name, const char* file) {
	Mix_Chunk* tempSFX = Mix_LoadWAV(file);
	if (!tempSFX) {
		std::cout << "Mix_LoadWAV() Failed: " << Mix_GetError() << std::endl;
		return nullptr;
	}
	sfx[name] = tempSFX;
	return sfx[name];
}

void RenderWindow::playSFX(const char* name) {
	Mix_PlayChannel(-1, sfx.find(name)->second, 0);
}

void RenderWindow::tick() {
	currentTime = SDL_GetPerformanceCounter();
	Uint64 frequency = SDL_GetPerformanceFrequency();

	if (previousTime == 0) {
		previousTime = currentTime;
	}

	deltaTime = (double)(currentTime - previousTime) / (double)frequency;

	if (deltaTime > 0) {
		fps = 1.0 / deltaTime;
	}
	else {
		fps = 0.0;
	}

	applyCameraShake();
	previousTime = currentTime;
}

void RenderWindow::setCameraPosition(float x, float y) {
	cameraPosition.x = static_cast<float>(std::max(0.0f, std::min(x, sceneBounds.x - viewportSize.x)));
	cameraPosition.y = static_cast<float>(std::max(0.0f, std::min(y, sceneBounds.y - viewportSize.y)));
}

void RenderWindow::moveCamera(float dx, float dy) {
	float newX = cameraPosition.x + dx * cameraSpeed * static_cast<float>(deltaTime);
	float newY = cameraPosition.y + dy * cameraSpeed * static_cast<float>(deltaTime);
	setCameraPosition(newX, newY);
}

void RenderWindow::cameraShake(float amplitude, float duration) {
	cameraShakeAmplitude = amplitude;
	cameraShakeDuration = duration;
	cameraShakeTimeRemaining = duration;
	originalCameraPosition = cameraPosition;
}

void RenderWindow::applyCameraShake() {
	if (cameraShakeTimeRemaining > 0.0f) {
		int offsetX = static_cast<int>((rand() % (int)(2 * cameraShakeAmplitude)) - cameraShakeAmplitude);
		int offsetY = static_cast<int>((rand() % (int)(2 * cameraShakeAmplitude)) - cameraShakeAmplitude);

		cameraPosition.x = originalCameraPosition.x + offsetX;
		cameraPosition.y = originalCameraPosition.y + offsetY;

		cameraShakeTimeRemaining -= static_cast<float>(deltaTime);

		if (cameraShakeTimeRemaining <= 0.0f) {
			cameraPosition = originalCameraPosition;
			setCameraPosition(cameraPosition.x, cameraPosition.y);
		}
	}
}

void RenderWindow::cameraFlash(Uint8 r, Uint8 g, Uint8 b, float duration) {
	cameraFlashColor = { r, g, b, 255 };
	cameraFlashDuration = duration;
	cameraFlashTimeRemaining = duration;
	cameraFlashAlpha = 255.0f;
}

void RenderWindow::applyCameraFlash() {
	if (cameraFlashTimeRemaining > 0.0f) {
		cameraFlashTimeRemaining -= static_cast<float>(deltaTime);
		cameraFlashAlpha = std::max(0.0f, 255.0f * (cameraFlashTimeRemaining / cameraFlashDuration));
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, cameraFlashColor.r, cameraFlashColor.g, cameraFlashColor.b, static_cast<Uint8>(cameraFlashAlpha));
		SDL_Rect cameraFlashRect = { 0, 0, static_cast<int>(viewportSize.x), static_cast<int>(viewportSize.y) };
		SDL_RenderFillRect(renderer, &cameraFlashRect);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	}
}

void RenderWindow::setCameraBounds(float x, float y) {
	sceneBounds = { x, y };
}

void RenderWindow::setViewportSize(float x, float y) {
	viewportSize = { x, y };
}

int RenderWindow::getViewportWidth() {
	return static_cast<int>(viewportSize.x);
}

int RenderWindow::getViewPortHeight() {
	return static_cast<int>(viewportSize.y);
}

SDL_Rect RenderWindow::applyCameraOffset(float x, float y, int w, int h) {
	return SDL_Rect{ static_cast<int>(x - cameraPosition.x), static_cast<int>(y - cameraPosition.y), w, h };
}

SDL_Rect RenderWindow::applyCameraOffset(int x, int y, int w, int h) {
	return SDL_Rect{ x - static_cast<int>(cameraPosition.x), y - static_cast<int>(cameraPosition.y), w, h };
}

void RenderWindow::setCameraSpeed(float speed) {
	cameraSpeed = speed;
}

float RenderWindow::getCameraX() {
	return cameraPosition.x;
}

float RenderWindow::getCameraY() {
	return cameraPosition.y;
}

void RenderWindow::cameraFollow(Player& player) {
	float deadZoneW = viewportSize.x / 4;
	float deadZoneH = viewportSize.y / 4;

	SDL_Rect deadZone = {
		static_cast<int>((viewportSize.x - deadZoneW) / 2),
		static_cast<int>((viewportSize.y - deadZoneH) / 2),
		static_cast<int>(deadZoneW),
		static_cast<int>(deadZoneH)
	};

	float targetX = player.position.x - cameraPosition.x;
	float targetY = player.position.y - cameraPosition.y;

	float dx = 0;
	float dy = 0;

	if (targetX < deadZone.x) {
		dx = -(deadZone.x - targetX) * static_cast<float>(deltaTime);
	} else if (targetX > deadZone.x + deadZone.w) {
		dx = (targetX - (deadZone.x + deadZone.w)) * static_cast<float>(deltaTime);	
	}

	if (targetY < deadZone.y) {
		dy = -(deadZone.y - targetY) * static_cast<float>(deltaTime);
	} else if (targetY > deadZone.y + deadZone.h) {
		dy = (targetY - (deadZone.y + deadZone.h)) * static_cast<float>(deltaTime);
	}

	moveCamera(dx, dy);
}



void RenderWindow::clear() {
	tick();
	SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
	SDL_RenderClear(renderer);
}

void RenderWindow::blit(int x, int y, SDL_Texture* texture) {
	SDL_Rect dst = applyCameraOffset(x, y, 0, 0);

	SDL_QueryTexture(texture, NULL, NULL, &dst.w, &dst.h);

	SDL_RenderCopy(renderer, texture, NULL, &dst);
}

void RenderWindow::blit(Map& map) {
	for (size_t y = 0; y < map.mapData.size(); y++) {
		for (size_t x = 0; x < map.mapData[y].size(); x++) {
			char tileID = map.mapData[y][x];

			for (Tile& tile : map.tiles) {
				if (tile.symbol == tileID) {
					SDL_Rect dst = applyCameraOffset(
						static_cast<int>(x * 16 * map.scale), // Scale the x-coordinate
						static_cast<int>(y * 16 * map.scale), // Scale the y-coordinate
						16 * map.scale, // Scale the width
						16 * map.scale  // Scale the height
					);
					SDL_RenderCopy(renderer, tile.texture, NULL, &dst);
				}
			}
		}
	}

	for (auto& item : map.items.items) {
		item->w = 16 * map.scale;
		item->h = 16 * map.scale;

		SDL_Rect dst = applyCameraOffset(item->x, item->y, item->w * item->scale, item->h * item->scale);

		SDL_RenderCopy(renderer, item->texture, NULL, &dst);
	}
}

void RenderWindow::blit(int startX, int startY, Map& minimap) {
	// Loop over the minimap's mapData
	for (size_t row = 0; row < minimap.mapData.size(); ++row) {
		for (size_t col = 0; col < minimap.mapData[row].size(); ++col) {
			char tileID = minimap.mapData[row][col];

			// Set color based on the tileID
			if (tileID == ' ' || tileID == '.') {
				setDrawColor(55, 55, 55); // Empty space color
			} else {
				setDrawColor(0, 0, 0); // Occupied space color
			}

			// Calculate the destination rectangle with scaling
			SDL_Rect dst = {
				startX + static_cast<int>(col * 4), // Scale x-coordinate
				startY + static_cast<int>(row * 4), // Scale y-coordinate
				4, // Width of the tile
				4  // Height of the tile
			};

			// Draw the rectangle
			drawRect("fill", dst.x, dst.y, dst.w, dst.h);
		}
	}
}

void RenderWindow::blit(Player& player) {
	SDL_Rect dst = applyCameraOffset(player.position.x, player.position.y, player.w, player.h);
	SDL_RenderCopy(renderer, player.texture, NULL, &dst);
}

void RenderWindow::blit(Items& items) {
	for (auto& item : items.items) {
		SDL_Rect dst = applyCameraOffset(item->x, item->y, item->w, item->h);
		SDL_RenderCopy(renderer, item->texture, NULL, &dst);
	}
}

void RenderWindow::print(int x, int y, const char* text) {
	SDL_Surface* tempSurface = TTF_RenderText_Solid(font, text, fontColor);

	if (!tempSurface) {
		std::cout << "TTF_RenderText_Solid() Failed: " << TTF_GetError() << std::endl;
	}

	SDL_Rect src;
	src.x = 0;
	src.y = 0;

	SDL_Texture* tempTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);

	if (!tempTexture) {
		std::cout << "SDL_CreateTextureFromSurface() Failed: " << SDL_GetError() << std::endl;
	}

	SDL_QueryTexture(tempTexture, NULL, NULL, &src.w, &src.h);

	SDL_FreeSurface(tempSurface);

	SDL_Rect dst = { x, y, src.w, src.h };

	SDL_RenderCopy(renderer, tempTexture, &src, &dst);

	SDL_DestroyTexture(tempTexture);
}

void RenderWindow::printf(int x, int y, const char* format, ...) {
	char buffer[1024];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);
	print(x, y, buffer);
}

void RenderWindow::drawRect(const char* mode, int x, int y, int w, int h) {
	SDL_SetRenderDrawColor(renderer, drawColor.r, drawColor.g, drawColor.b, drawColor.a);
	SDL_Rect dst = { x, y, w, h };
	if (strcmp(mode, "fill") == 0) {
		SDL_RenderFillRect(renderer, &dst);
	} else if (strcmp(mode, "line") == 0) {
		SDL_RenderDrawRect(renderer, &dst);
	} else {
		std::cout << "Invalid mode: " << mode << std::endl;
	}
	SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
}

void RenderWindow::flip() {
	applyCameraFlash();
	SDL_RenderPresent(renderer);
}

int RenderWindow::getScreenWidth() {
	return screen_w;
}

int RenderWindow::getScreenHeight() {
	return screen_h;
}

SDL_Renderer* RenderWindow::getRenderer() {
	return renderer;
}

void RenderWindow::setBackgroundColor(Uint8 r, Uint8 g, Uint8 b) {
	backgroundColor = { r, g, b, 255 };
}

void RenderWindow::setDrawColor(Uint8 r, Uint8 g, Uint8 b) {
	drawColor = { r, g, b, 255 };
}

void RenderWindow::setFontColor(Uint8 r, Uint8 g, Uint8 b) {
	fontColor = { r, g, b, 255 };
}

void RenderWindow::clean() {
	for (SDL_Texture* texture : textures) {
		std::cout << "Destrtoying Texture: " << texture << std::endl;
		SDL_DestroyTexture(texture);
	}
	textures.clear();

	for (auto& sfxPair : sfx) {
		std::cout << "Destroying SFX: " << sfxPair.first << std::endl;
		Mix_FreeChunk(sfxPair.second);
	}
	sfx.clear();
}
