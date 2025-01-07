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

	fontColor = { 255, 255, 255, 0 };

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

void RenderWindow::clear() {
	SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
	SDL_RenderClear(renderer);
}

void RenderWindow::blit(int x, int y, SDL_Texture* texture) {
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;

	SDL_QueryTexture(texture, NULL, NULL, &dst.w, &dst.h);

	SDL_RenderCopy(renderer, texture, NULL, &dst);
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

void RenderWindow::flip() {
	SDL_RenderPresent(renderer);
}

int RenderWindow::getScreenWidth() {
	return screen_w;
}

int RenderWindow::getScreenHeight() {
	return screen_h;
}

void RenderWindow::setBackgroundColor(Uint8 r, Uint8 g, Uint8 b) {
	backgroundColor = { r, g, b, 255 };
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
}
