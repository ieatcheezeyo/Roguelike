#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "RenderWindow.h"
#include "InputManager.h"
#include "Map.h"
#include "Player.h"

int main(int argc, char* argv[]) {

    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    RenderWindow screen("", 1280, 720);
    screen.setBackgroundColor(255, 255, 255);
    screen.setFontColor(0, 0, 0);

    InputManager pad;

    SDL_Texture* testTexture = screen.loadTexture("Assets/Images/test.png");
    SDL_Texture* emptyTileTexture = screen.loadTexture("Assets/Images/Empty_tile.png");

    Player player(16, 16, testTexture);

    int mapWidth = 1280 / 16;
    int mapHeight = 720 / 16;

    Map map(mapWidth, mapHeight, screen.getRenderer());
    map.generateDungeon();

    player.findSpawnPoint(map.mapData);

    bool running = true;

    SDL_Event event;

    while (running) {
        pad.update();

        if (pad.quit) {
            running = false;
        } else if (pad.button_select && !pad.old_button_select) {
            running = false;
        }

        if (pad.button_a && !pad.old_button_a) {
        
        }

        player.update(map.mapData, pad, screen.dt());

        screen.clear();

        screen.blit(map);

        //screen.printf(10, 10, "DT: %f", screen.dt());
        //screen.printf(10, 25, "FPS: %f", screen.getFPS());
        //screen.printf(10, 100, "Player Position: X(%i),Y(%i)", player.position.x / 16, player.position.y / 16);
        screen.blit(player);

        screen.flip();
        pad.lateUpdate();
    }

    std::cout << "Hello World!\n";
    return 0;
}
