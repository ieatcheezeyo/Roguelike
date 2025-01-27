#include "Player.h"

Player::Player(int x, int y, int scale, SDL_Texture* p_texture)
    : justMoved(false), scale(scale), moveSpeed(10.0f), holdTimer(0.0), holdDelay(0.1), repeatDelay(0.2), isHolding(false), health(100), gold(0), isControllable(true) {
    position = { x, y };
    targetPosition = position;
    texture = p_texture;

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

void Player::update(Map& map, InputManager& pad, double dt) {
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
            for (auto& tile : walkableTiles) {

                if (pad.dpad_up && map.mapData[tilePos.y - 1][tilePos.x] == tile) {
                    targetPosition.y -= 16 * scale;
                } else if (pad.dpad_down && map.mapData[tilePos.y + 1][tilePos.x] == tile) {
                    targetPosition.y += 16 * scale;
                } else if (pad.dpad_left && map.mapData[tilePos.y][tilePos.x - 1] == tile) {
                    targetPosition.x -= 16 * scale;
                } else if (pad.dpad_right && map.mapData[tilePos.y][tilePos.x + 1] == tile) {
                    targetPosition.x += 16 * scale;
                }
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
                    it = map.items.items.erase(it);
                } else {
                    if (inventory.addItem(item)) {
                        std::cout << "Item " << item->name << " added to inventory.\n";
                        it = map.items.items.erase(it);
                    } else {
                        std::cerr << "Failed to add item: " << item->name << " to inventory.\n";
                        ++it;
                    }
                }
            } else {
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
