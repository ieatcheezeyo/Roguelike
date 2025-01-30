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
    screen.audio = new AudioManager();
    screen.setBackgroundColor(0, 0, 0);
    screen.setFontColor(0, 0, 0);

    int mapWidth = (1280 * 2) / 16;
    int mapHeight = (720 * 3) / 16;

	screen.setCameraBounds(static_cast<float>((mapWidth * 16) * SCALE), static_cast<float>((mapHeight * 16) * SCALE));
	screen.setViewportSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	screen.setCameraSpeed(300.0f);

    InputManager pad;

    //Load SFX
    screen.audio->loadSFX("gravel", "Assets/SFX/gravel.wav");
    screen.audio->loadSFX("coin", "Assets/SFX/pickupCoin.wav");
    screen.audio->loadSFX("usePotion", "Assets/SFX/usePotion.wav");
    screen.audio->loadSFX("eatFood", "Assets/SFX/eatFood.wav");

	//Load Textures
    SDL_Texture* playerTexture = screen.loadTexture("Assets/Images/Player.png");
    SDL_Texture* healthBarTexture = screen.loadTexture("Assets/Images/UI_HealthBar.png");
    SDL_Texture* coinCountTexture = screen.loadTexture("Assets/Images/UI_Coin.png");

    TTF_Font* inventoryFont = screen.createFont("Assets/Fonts/default.ttf", 12);

    Player player(0, 0, SCALE, playerTexture, screen.getAudioManager());

    Map map(mapWidth, mapHeight, SCALE, screen.getRenderer());
    map.generateDungeon((SDL_GetTicks64() * 64) * SDL_GetPerformanceCounter() * 128);

    player.findSpawnPoint(map.mapData);

    bool running = true;
    bool minimap = false;
    bool inventory = false;
    bool showPlayerIndicator = true;
    float playerIndicatorFlashTimer = 0.0f;

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

        if (pad.button_y && !pad.old_button_y) {
            inventory = !inventory;
        } else if (pad.button_b && !pad.old_button_b) {
            
        }

        player.update(map, pad, *screen.console, screen.dt());

        if (player.hasMoved()) {
			screen.audio->playSFX("gravel");
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

            playerIndicatorFlashTimer += static_cast<float>(screen.dt());
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
                player.inventory.index = (player.inventory.index == 0) ? player.inventory.inventory.size() - 1 : player.inventory.index - 1;
            }

            if (pad.button_a && !pad.old_button_a) {
                auto& currentItem = player.inventory.inventory[player.inventory.index];
                if (currentItem != nullptr) {
                    switch (currentItem->type) {
                    case recover_hp:
                    {
                        if (player.getHealth() < 100) {
                            int amount = rand() % 18 + 5;
                            player.refillHealth(amount);
                            screen.audio->playSFX("usePotion");
                            screen.console->addMessage("You drank the %s and recovered %i HP.", currentItem->name.c_str(), amount);
                            delete currentItem;
                            currentItem = nullptr;
                            player.inventory.compact();
                        } else {
                            screen.console->addMessage("Your HP is already full.");
                        }
                        break;
                    }
                    case food:
                    {
                        if (player.getHealth() < 100) {
                            int amount = rand() % 18 + 5;
                            player.refillHealth(amount);
                            screen.audio->playSFX("eatFood");
                            screen.console->addMessage("You ate the %s and recovered %i HP.", currentItem->name.c_str(), amount);
                            delete currentItem;
                            currentItem = nullptr;
                            player.inventory.compact();
                        } else {
                            screen.console->addMessage("Your HP is already full.");
                        }
                        break;
                    }
                    case weapon:
                    {
                        if (currentItem->equipable) {
                            if (currentItem->equiped) {
                                currentItem->equiped = false;
                                player.atk -= currentItem->atk;
                                screen.console->addMessage("You Unequip the %s.", currentItem->name.c_str());
                            } else {
                                for (auto& item : player.inventory.inventory) {
                                    if (item != nullptr && item->type == weapon && item->equiped) {
                                        item->equiped = false;
                                        player.atk -= item->atk;
                                    }
                                }
                                currentItem->equiped = true;
                                player.atk += currentItem->atk;
                                screen.console->addMessage("You equiped the %s.", currentItem->name.c_str());
                            }
                        }
                        break;
                    }
                    case shield:
                    {
                        if (currentItem->equipable) {
                            if (currentItem->equiped) {
                                currentItem->equiped = false;
                                player.def -= currentItem->def;
                                screen.console->addMessage("You Unequip the %s.", currentItem->name.c_str());
                            } else {
                                for (auto& item : player.inventory.inventory) {
                                    if (item != nullptr && item->type == shield && item->equiped) {
                                        item->equiped = false;
                                        player.def -= item->def;
                                    }
                                }
                                currentItem->equiped = true;
                                player.def += currentItem->def;
                                screen.console->addMessage("You equiped the %s.", currentItem->name.c_str());
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

                            if (map.mapData[dropTileY][dropTileX] == ' ' || map.mapData[dropTileY][dropTileX] == '.') {
                                int dropWorldX = dropTileX * 16 * SCALE;
                                int dropWorldY = dropTileY * 16 * SCALE;

                                screen.console->addMessage("You drop the %s.", currentItem->name.c_str());

                                Item* newItem = map.items.createItem(dropWorldX, dropWorldY, currentItem->scale, currentItem->name);
                                if (newItem) {
                                    newItem->atk = currentItem->atk;
                                    newItem->def = currentItem->def;
                                    newItem->value = currentItem->value;
                                    newItem->description = currentItem->description;
                                    newItem->equipable = currentItem->equipable;
                                    newItem->dropable = currentItem->dropable;
                                    newItem->type = currentItem->type;
                                    newItem->cursed = currentItem->cursed;
                                }

                                delete currentItem;
                                currentItem = nullptr;
                                player.inventory.compact();

                                itemDropped = true;
                                break;
                            }
                        }

                        if (!itemDropped) {
                            //std::cout << "There isn't enough room to drop the item.\n";
                        }
                    } else {
                        // Not dropable
                        //std::cout << "This item cannot be dropped.\n";
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
