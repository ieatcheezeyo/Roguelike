#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "RenderWindow.h"
#include "InputManager.h"

int main(int argc, char* argv[]) {

    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    RenderWindow screen("", 1280, 720);
    screen.setBackgroundColor(255, 255, 255);
    screen.setFontColor(0, 0, 0);

    InputManager pad;

    SDL_Texture* testTexture = screen.loadTexture("Assets/Images/test.png");

    bool running = true;

    SDL_Event event;

    int x = 0;

    while (running) {
        pad.update();

        if (pad.quit) {
            running = false;
        }

        x++;

        screen.clear();

        screen.printf(10, 10, "Hello, World! %i", x);
        screen.blit(100, 100, testTexture);

        screen.flip();
        pad.lateUpdate();
    }

    std::cout << "Hello World!\n";
    return 0;
}
