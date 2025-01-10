#include "Player.h"

Player::Player(int x, int y, int scale, SDL_Texture* p_texture)
    : justMoved(false), scale(scale), moveSpeed(10.0f), holdTimer(0.0), holdDelay(0.1), repeatDelay(0.2), isHolding(false) {
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

    // Try to find a spawn point by scanning the map.
    for (int y = 1; y < numRows - 1; y++) {  // Avoid edges to prevent out-of-bounds
        for (int x = 1; x < numCols - 1; x++) {
            // Check if the current tile is an empty space and surrounded by walls
            if (mapData[y][x] == ' ' &&
                mapData[y - 1][x] == ' ' && // Check top
                mapData[y + 1][x] == ' ' && // Check bottom
                mapData[y][x - 1] == ' ' && // Check left
                mapData[y][x + 1] == ' ')   // Check right
            {
                // Set the player's spawn position to this valid point
                position.x = x * 16 * scale;  // Assuming each tile is 16x16 pixels
                position.y = y * 16 * scale;
                targetPosition.x = x * 16 * scale;
                targetPosition.y = y * 16 * scale;
                return;  // Stop searching once a valid spawn point is found
            }
        }
    }

    // If no spawn point was found, you might want to handle it, e.g., by spawning the player at (0, 0)
    std::cout << "No valid spawn point found, spawning at (0, 0)" << std::endl;
    position.x = 0;
    position.y = 0;
	targetPosition.x = 0;
	targetPosition.y = 0;
}

void Player::update(std::vector<std::vector<char>> mapData, InputManager& pad, double dt) {
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
		
            if (pad.dpad_up && mapData[tilePos.y - 1][tilePos.x] == tile) {
                targetPosition.y -= 16 * scale;
            } else if (pad.dpad_down && mapData[tilePos.y + 1][tilePos.x] == tile) {
                targetPosition.y += 16 * scale;
            } else if (pad.dpad_left && mapData[tilePos.y][tilePos.x - 1] == tile) {
                targetPosition.x -= 16 * scale;
            } else if (pad.dpad_right && mapData[tilePos.y][tilePos.x + 1] == tile) {
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
