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

        if (pad.button_y && !pad.old_button_y) {
            minimap = !minimap;
        }

        player.update(map, pad, screen.dt());

        if (player.hasMoved()) {
			screen.playSFX("gravel");
            for (auto& enemy : map.enemies) {
                enemy->update(map.mapData, screen.dt());
            }
        }

        screen.clear();

        screen.blit(map);
        screen.blit(player);

        screen.setFontColor(255, 255, 255);
        screen.printf(SCREEN_WIDTH - 130, 25, "FPS: %.2f", screen.getFPS());
        

        if (minimap) {
            int minimapX = SCREEN_WIDTH / 2 - (mapWidth * 4) / 2;
            int minimapY = 20;
            screen.blit(minimapX, minimapY, map);

            int playerMinimapX = minimapX + (player.position.x / SCALE / 16) * 4;
            int playerMinimapY = minimapY + (player.position.y / SCALE / 16) * 4;

            playerMinimapX = std::max(minimapX, std::min(playerMinimapX, minimapX + mapWidth * 4 - 1));
            playerMinimapY = std::max(minimapY, std::min(playerMinimapY, minimapY + mapHeight * 4 - 1));

            playerIndicatorFlashTimer += screen.dt();
            if (playerIndicatorFlashTimer >= 0.5f) {
                showPlayerIndicator = !showPlayerIndicator;
                playerIndicatorFlashTimer = 0.0f;
            }

            if (showPlayerIndicator) {
                screen.setDrawColor(255, 0, 0);
                screen.drawRect("fill", playerMinimapX, playerMinimapY, 4, 4);
            }

            screen.printf(10, 25, "Position: X(%i),Y(%i)", (player.position.x / SCALE) / 16, (player.position.y / SCALE) / 16);

        } else {
            screen.blitUiElement(5, 5, healthBarTexture);
            screen.blitUiElement(5, 35, coinCountTexture);
            screen.setDrawColor(255, 0, 0);

            int healthBarTextureWidth = 128;
            int borderSize = 4;
            int usableWidth = healthBarTextureWidth - 2 * borderSize;
            int scaledHealthWidth = static_cast<int>((player.getHealth() / 100.0) * usableWidth);

            screen.drawRect("fill", 5 + borderSize, 5 + borderSize, scaledHealthWidth, 25 - 2 * borderSize);
            
            screen.printf(30, 10, "HP: %i", player.getHealth());
            screen.printf(40, 45, "x%03i", player.getGold());
            screen.printf(30, 80, "ATK: %i, DEF: %i", player.atk, player.def);
        }

        player.isControllable = !inventory;

        if (inventory) {

            if (pad.button_b && !pad.old_button_b) {
                inventory = !inventory;
            }

            if (pad.dpad_down && !pad.old_dpad_down) {
                player.inventory.index = (player.inventory.index + 1) % player.inventory.inventory.size();
            } else if (pad.dpad_up && !pad.old_dpad_up) {
                player.inventory.index = (player.inventory.index == 0)
                    ? player.inventory.inventory.size() - 1
                    : player.inventory.index - 1;
            }

            if (pad.button_a && !pad.old_button_a) {
                auto& currentItem = player.inventory.inventory[player.inventory.index];
                if (currentItem != nullptr) {
                    switch (currentItem->type) {
                    case recover_hp:
                    {
                        if (player.getHealth() < 100) {
                            player.refillHealth(rand() % 18);
                            delete currentItem;
                            currentItem = nullptr;
                            player.inventory.compact();
                        }
                        break;
                    }
                    case food:
                    {
                        if (player.getHealth() < 100) {
                            player.refillHealth(rand() % 18);
                            delete currentItem;
                            currentItem = nullptr;
                            player.inventory.compact();
                        }
                        break;
                    }
                    case weapon:
                    {
                        if (currentItem->equipable) {
                            if (currentItem->equiped) {
                                // Unequip the currently selected weapon
                                currentItem->equiped = false;
                                player.atk -= currentItem->atk; // Revert stats
                            } else {
                                // Unequip any other equipped weapon and revert stats
                                for (auto& item : player.inventory.inventory) {
                                    if (item != nullptr && item->type == weapon && item->equiped) {
                                        item->equiped = false;
                                        player.atk -= item->atk; // Revert stats
                                    }
                                }

                                // Equip the selected weapon
                                currentItem->equiped = true;
                                player.atk += currentItem->atk; // Add stats
                            }
                        }
                        break;
                    }
                    case shield:
                    {
                        if (currentItem->equipable) {
                            if (currentItem->equiped) {
                                // Unequip the currently selected shield
                                currentItem->equiped = false;
                                player.def -= currentItem->def; // Revert stats
                            } else {
                                // Unequip any other equipped shield and revert stats
                                for (auto& item : player.inventory.inventory) {
                                    if (item != nullptr && item->type == shield && item->equiped) {
                                        item->equiped = false;
                                        player.def -= item->def; // Revert stats
                                    }
                                }

                                // Equip the selected shield
                                currentItem->equiped = true;
                                player.def += currentItem->def; // Add stats
                            }
                        }
                        break;
                    }
                    default:
                        break;
                    }
                }
            }

            if (pad.button_x && !pad.old_button_x) {
                auto& currentItem = player.inventory.inventory[player.inventory.index];
                if (currentItem != nullptr) {
                    if (currentItem->dropable) {
                        int playerTileX = player.position.x / (16 * SCALE);
                        int playerTileY = player.position.y / (16 * SCALE);

                        // Directions for cardinal movement: {dx, dy}
                        std::vector<std::pair<int, int>> directions = {
                            {0, -1}, // North
                            {0, 1},  // South
                            {-1, 0}, // West
                            {1, 0}   // East
                        };

                        std::shuffle(directions.begin(), directions.end(), std::mt19937(std::random_device()()));

                        bool itemDropped = false;

                        for (const auto& dir : directions) {
                            int dropTileX = playerTileX + dir.first;
                            int dropTileY = playerTileY + dir.second;

                            // Check if the target tile is valid (' ' or '.')
                            if (map.mapData[dropTileY][dropTileX] == ' ' || map.mapData[dropTileY][dropTileX] == '.') {
                                // Convert tile coordinates back to world coordinates
                                int dropWorldX = dropTileX * 16 * SCALE;
                                int dropWorldY = dropTileY * 16 * SCALE;

                                std::cout << "Dropping item at: Tile(" << dropTileX << ", " << dropTileY << ") "
                                    << "World(" << dropWorldX << ", " << dropWorldY << ")" << std::endl;

                                // Create the item in the world
                                Item* newItem = map.items.createItem(dropWorldX, dropWorldY, currentItem->scale, currentItem->name);
                                if (newItem) {
                                    newItem->atk = currentItem->atk;
                                    newItem->def = currentItem->def;
                                    newItem->value = currentItem->value;
                                    newItem->description = currentItem->description;
                                    newItem->equipable = currentItem->equipable;
                                    newItem->dropable = currentItem->dropable;
                                    newItem->type = currentItem->type;
                                }

                                // Remove the item from the player's inventory
                                delete currentItem;
                                currentItem = nullptr;
                                player.inventory.compact();

                                itemDropped = true;
                                break; // Exit the loop once the item is dropped
                            }
                        }

                        if (!itemDropped) {
                            // No valid position found
                            std::cout << "There isn't enough room to drop the item.\n";
                        }
                    } else {
                        // Not dropable
                        std::cout << "This item cannot be dropped.\n";
                    }
                }
            }

            screen.setFont(inventoryFont);
            screen.blit(player.inventory, 5, 128, 128, 32);
            screen.setFont();
        }

        screen.flip();
        pad.lateUpdate();
    }

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    std::cout << "Goodbye!\n";
    return 0;
}
