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
#define SCALE 4

int main(int argc, char* argv[]) {

    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    RenderWindow screen("", SCREEN_WIDTH, SCREEN_HEIGHT);
    screen.setBackgroundColor(0, 0, 0);
    screen.setFontColor(0, 0, 0);

    int mapWidth = (1280 * 2) / 16;
    int mapHeight = (720 * 3) / 16;

	screen.setCameraBounds((mapWidth * 16) * SCALE, (mapHeight * 16) * SCALE);
	screen.setViewportSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	screen.setCameraSpeed(300.0f);

    InputManager pad;

    //Load SFX
	Mix_Chunk* sfx = screen.loadSFX("gravel", "Assets/SFX/gravel.wav");

	//Load Textures
    SDL_Texture* playerTexture = screen.loadTexture("Assets/Images/Player.png");
    SDL_Texture* healthBarTexture = screen.loadTexture("Assets/Images/UI_HealthBar.png");
    SDL_Texture* coinCountTexture = screen.loadTexture("Assets/Images/UI_Coin.png");

    TTF_Font* inventoryFont = screen.createFont("Assets/Fonts/default.ttf", 12);

    Player player(0, 0, SCALE, playerTexture);

    Map map(mapWidth, mapHeight, SCALE, screen.getRenderer());
    map.generateDungeon((SDL_GetTicks64() * 64) * SDL_GetPerformanceCounter() * 128);

    player.findSpawnPoint(map.mapData);

    bool running = true;
    bool minimap = false;
    bool inventory = false;
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

            inventory = !inventory;
		}

        if (pad.button_a && !pad.old_button_a) {
            //player.findSpawnPoint(map.mapData);
            player.takeDamage(5);
            screen.cameraShake(10.0f, 0.5f);
			//screen.playSFX("gravel");
        } else if (pad.button_b && !pad.old_button_b) {
            player.refillHealth(5);
            player.inventory.printInventory();
        } else if (pad.button_y && !pad.old_button_y) {
            minimap = !minimap;
        }

        player.update(map, pad, screen.dt());

        if (player.hasMoved()) {
			screen.playSFX("gravel");
        }

        screen.clear();

        screen.blit(map);
        screen.blit(player);

        //screen.blit(itemsManager);

		

        screen.setFontColor(255, 255, 255);
        screen.printf(SCREEN_WIDTH - 130, 25, "FPS: %.2f", screen.getFPS());
        

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

            screen.printf(10, 25, "Position: X(%i),Y(%i)", (player.position.x / SCALE) / 16, (player.position.y / SCALE) / 16);

        } else {
            screen.blitUiElement(5, 5, healthBarTexture);
            screen.blitUiElement(5, 35, coinCountTexture);
            screen.setDrawColor(255, 0, 0);

            // Scale player's health (0-100) to fit the inner width of the health bar
            int healthBarTextureWidth = 128;
            int borderSize = 4;
            int usableWidth = healthBarTextureWidth - 2 * borderSize; // Inner width of the bar
            int scaledHealthWidth = static_cast<int>((player.getHealth() / 100.0) * usableWidth);

            // Draw the scaled health bar inside the borders
            screen.drawRect("fill", 5 + borderSize, 5 + borderSize, scaledHealthWidth, 25 - 2 * borderSize);
            
            screen.printf(30, 10, "HP: %i", player.getHealth());
            screen.printf(40, 45, "x%03i", player.getGold());
        }

        player.isControllable = !inventory;

        if (inventory) {
            if (pad.dpad_down && !pad.old_dpad_down) {
                player.inventory.index = (player.inventory.index + 1) % player.inventory.inventory.size();
            } else if (pad.dpad_up && !pad.old_dpad_up) {
                player.inventory.index = (player.inventory.index - 1) % player.inventory.inventory.size();
            }

            // Set inventory font for rendering
            screen.setFont(inventoryFont);

            // Render inventory
            screen.blit(player.inventory, 5, 128, 128, 32);

            // Reset font to default
            screen.setFont();
        }

        screen.flip();
        pad.lateUpdate();
    }

    std::cout << "Hello World!\n";
    return 0;
}
