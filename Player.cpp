#include "Player.h"

Player::Player(int x, int y, int scale, SDL_Texture* p_texture, AudioManager* audio)
    : justMoved(false), scale(scale), moveSpeed(10.0f), holdTimer(0.0), holdDelay(0.1), repeatDelay(0.2), isHolding(false), health(rand() % 75 + 10), gold(0), isControllable(true), audio(audio) {
    position = { x, y };
    targetPosition = position;
    texture = p_texture;

    baseDef = rand() % 5;
    baseAtk = rand() % 5;

    def = baseDef;
    atk = baseAtk;

    SDL_QueryTexture(p_texture, NULL, NULL, &w, &h);

	w *= scale;
	h *= scale;

}

Player::~Player() {
    w = 0;
    h = 0;
}

void Player::findSpawnPoint(std::vector<std::vector<char>> mapData) {
    int numRows = mapData.size();
    int numCols = mapData[0].size();

    for (int y = 1; y < numRows - 1; y++) {
        for (int x = 1; x < numCols - 1; x++) {
            if (mapData[y][x] == ' ' &&
                mapData[y - 1][x] == ' ' &&
                mapData[y + 1][x] == ' ' &&
                mapData[y][x - 1] == ' ' &&
                mapData[y][x + 1] == ' ')
            {
                position.x = x * 16 * scale;
                position.y = y * 16 * scale;
                targetPosition.x = x * 16 * scale;
                targetPosition.y = y * 16 * scale;
                return;
            }
        }
    }

    std::cout << "No valid spawn point found, spawning at (0, 0)" << std::endl;
    position.x = 0;
    position.y = 0;
	targetPosition.x = 0;
	targetPosition.y = 0;
}

void Player::update(Map& map, InputManager& pad, TextConsole& console, std::vector<Enemy*>& enemies, double dt) {
    if (isControllable) {
        justMoved = false;
        Vector2 tilePos = { position.x / (16 * scale), position.y / (16 * scale) };

        Vector2 previousPosition = position;

        bool buttonPressed = pad.dpad_up || pad.dpad_down || pad.dpad_left || pad.dpad_right;
        if (buttonPressed) {
            holdTimer += dt;
        } else {
            holdTimer = 0.0;
            isHolding = false;
        }

        bool canMove = false;
        if (pad.dpad_up && !pad.old_dpad_up) {
            canMove = true;
            isHolding = true;
            holdTimer = 0.0;
        } else if (pad.dpad_down && !pad.old_dpad_down) {
            canMove = true;
            isHolding = true;
            holdTimer = 0.0;
        } else if (pad.dpad_left && !pad.old_dpad_left) {
            canMove = true;
            isHolding = true;
            holdTimer = 0.0;
        } else if (pad.dpad_right && !pad.old_dpad_right) {
            canMove = true;
            isHolding = true;
            holdTimer = 0.0;
        } else if (isHolding && holdTimer >= holdDelay + repeatDelay) {
            canMove = true;
            holdTimer -= repeatDelay;
        }

        if (canMove) {
            Vector2 intendedTarget = targetPosition;
            for (auto& tile : walkableTiles) {
                if (pad.dpad_up && map.mapData[tilePos.y - 1][tilePos.x] == tile) {
                    intendedTarget.y -= 16 * scale;
                } else if (pad.dpad_down && map.mapData[tilePos.y + 1][tilePos.x] == tile) {
                    intendedTarget.y += 16 * scale;
                } else if (pad.dpad_left && map.mapData[tilePos.y][tilePos.x - 1] == tile) {
                    intendedTarget.x -= 16 * scale;
                } else if (pad.dpad_right && map.mapData[tilePos.y][tilePos.x + 1] == tile) {
                    intendedTarget.x += 16 * scale;
                }
            }

            bool enemyCollision = false;
            for (auto it = enemies.begin(); it != enemies.end(); ) {
                Enemy* enemy = *it;

                int enemyWorldX = enemy->position.x * (16 * scale);
                int enemyWorldY = enemy->position.y * (16 * scale);

                if (enemyWorldX == intendedTarget.x && enemyWorldY == intendedTarget.y) {
                    int dmg = rand() % (atk * 2) + (atk / 2) - enemy->enemyStats.def;
                    dmg = (dmg < 0) ? 0 : dmg;

                    enemy->health -= dmg;
                    audio->playSFX("hit");

                    if (dmg <= 0) {
                        console.addMessage("You Miss the " + std::string(enemy->enemyDescriptor) + " " + enemy->name + ".");
                    } else {
                        console.addMessage("You hit the " + std::string(enemy->enemyDescriptor) + " " + enemy->name + " for " + std::to_string(dmg) + " damage.");
                    }

                    enemyCollision = true;

                    if (enemy->health <= 0) {
                        console.addMessage("You have slain the " + std::string(enemy->enemyDescriptor) + " " + enemy->name + "!");
                        delete enemy;
                        it = enemies.erase(it);
                    } else {
                        ++it;
                    }
                    break;
                } else {
                    ++it;
                }
            }

            if (!enemyCollision) {
                targetPosition = intendedTarget;
            }
        }

        position.x += (targetPosition.x - position.x) * moveSpeed * dt;
        position.y += (targetPosition.y - position.y) * moveSpeed * dt;

        if (std::abs(position.y - targetPosition.y) < 0.5f * h) {
            position.y = targetPosition.y;
        }

        if (std::abs(position.x - targetPosition.x) < 0.5f * w) {
            position.x = targetPosition.x;
        }

        if (position.x == targetPosition.x && position.y == targetPosition.y) {
            if (previousPosition.x != position.x || previousPosition.y != position.y) {
                justMoved = true;
            }
        }

        if (position.x != targetPosition.x || position.y != targetPosition.y) {
            justMoved = false;
        }

        for (auto it = map.items.items.begin(); it != map.items.items.end();) {
            Item* item = it->get();
            bool collides = (position.x == item->x && position.y == item->y);

            if (collides) {
                if (item->name == "Coin") {
                    gold++;
                    console.addMessage("You pick up 1 %s.", item->name.c_str());
                    audio->playSFX("coin");
                    it = map.items.items.erase(it);
                } else {
                    bool itemAdded = inventory.addItem(item);

                    if (itemAdded) {
                        console.addMessage("You pick up the %s %s.", item->descriptor.c_str(), item->name.c_str());
                        it = map.items.items.erase(it);
                    } else {
                        auto& messageFlag = messageShownForItemThisFrame[item];

                        if (messageFlag == false) {
                            console.addMessage("Your inventory is full, Couldn't pick up the %s.", item->name.c_str());
                            messageFlag = true;
                        }
                        ++it;
                    }
                }
            } else {
                messageShownForItemThisFrame.erase(item);
                ++it;
            }
        }
    }
}


int Player::getHealth() {
    return health;
}

int Player::getGold() {
    return gold;
}

void Player::takeDamage(int damage) {
    health -= damage;
}

void Player::refillHealth(int ammount) {
    health += ammount;
    if (health > 100) {
        health = 100;
    }
}

bool Player::hasMoved() {
    if (justMoved) {
        justMoved = false;
        return true;
    }
    return false;
}

Vector2 Player::getPosition() {
    return { position.x * scale / 16, position.y * scale / 16 };
}
