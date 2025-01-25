#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "RenderWindow.h"
#include "InputManager.h"
#include "Map.h"
#include "Player.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define SCALE 3

int main(int argc, char* argv[]) {

    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    RenderWindow screen("", SCREEN_WIDTH, SCREEN_HEIGHT);
    screen.setBackgroundColor(0, 0, 0);
    screen.setFontColor(0, 0, 0);

    int mapWidth = (1280 * 2) / 16;
    int mapHeight = (720 * 2) / 16;

	screen.setCameraBounds((mapWidth * 16) * SCALE, (mapHeight * 16) * SCALE);
	screen.setViewportSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	screen.setCameraSpeed(300.0f);

    InputManager pad;

    //Load SFX
	Mix_Chunk* sfx = screen.loadSFX("gravel", "Assets/SFX/gravel.wav");

	//Load Textures
    SDL_Texture* playerTexture = screen.loadTexture("Assets/Images/Player.png");

    Player player(0, 0, SCALE, playerTexture);

    Map map(mapWidth, mapHeight, SCALE, screen.getRenderer());
    map.generateDungeon(SDL_GetTicks64());

    player.findSpawnPoint(map.mapData);

    bool running = true;
    bool minimap = false;
    bool showPlayerIndicator = true;
    float playerIndicatorFlashTimer = 0.0f;

    SDL_Event event;

    while (running) {
        pad.update();

        screen.cameraFollow(player);

        if (pad.quit) {
            running = false;
        } else if (pad.button_select && !pad.old_button_select) {
            running = false;
		} else if (pad.button_start && !pad.old_button_start) {
			minimap = !minimap;
		}

        if (pad.button_a && !pad.old_button_a) {
            player.findSpawnPoint(map.mapData);

            screen.cameraShake(10.0f, 0.5f);
			screen.playSFX("gravel");
        }

        player.update(map.mapData, pad, screen.dt());

        if (player.hasMoved()) {
			screen.playSFX("gravel");
        }

        screen.clear();

        screen.blit(map);
        screen.blit(player);

        //screen.blit(itemsManager);

		

        screen.setFontColor(255, 255, 255);
        //screen.printf(10, 5, "%s, %i, %i", itemsManager.items[0]->name.c_str(), itemsManager.items[0]->x / 16, itemsManager.items[0]->y / 16);
        //screen.printf(10, 5, "DT: %.3f", screen.dt());
        screen.printf(10, 25, "FPS: %.2f", screen.getFPS());
        screen.printf(10, 55, "Player Position: X(%i),Y(%i)", (player.position.x / SCALE) / 16, (player.position.y / SCALE) / 16);

		//screen.setDrawColor(255, 255, 255);
		//screen.drawRect("line", player.position.x, player.position.y, 32, 32);

        if (minimap) {
            // Center the minimap horizontally and place it 20 pixels from the top
            int minimapX = SCREEN_WIDTH / 2 - (mapWidth * 4) / 2;
            int minimapY = 20;
            screen.blit(minimapX, minimapY, map);

            // Calculate scaled player position for the minimap
            int playerMinimapX = minimapX + (player.position.x / SCALE / 16) * 4;
            int playerMinimapY = minimapY + (player.position.y / SCALE / 16) * 4; // Use same scale for consistency

            // Ensure player position stays within minimap bounds
            playerMinimapX = std::max(minimapX, std::min(playerMinimapX, minimapX + mapWidth * 4 - 1));
            playerMinimapY = std::max(minimapY, std::min(playerMinimapY, minimapY + mapHeight * 4 - 1));

            // Update flashing timer
            playerIndicatorFlashTimer += screen.dt();
            if (playerIndicatorFlashTimer >= 0.5f) {
                showPlayerIndicator = !showPlayerIndicator;
                playerIndicatorFlashTimer = 0.0f;
            }

            // Draw the flashing player indicator
            if (showPlayerIndicator) {
                screen.setDrawColor(255, 0, 0); // Red color for player
                screen.drawRect("fill", playerMinimapX, playerMinimapY, 4, 4);
            }
        }

        screen.flip();
        pad.lateUpdate();
    }

    std::cout << "Hello World!\n";
    return 0;
}
