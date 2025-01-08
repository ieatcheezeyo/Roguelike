#include "Player.h"

Player::Player(int x, int y, SDL_Texture* p_texture)
    : moveSpeed(15.0f), holdTimer(0.0), holdDelay(0.2), repeatDelay(0.1), isHolding(false) {
    position = { x, y };
    targetPosition = position;
    texture = p_texture;

    SDL_QueryTexture(p_texture, NULL, NULL, &w, &h);
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
                position.x = x * 16;  // Assuming each tile is 16x16 pixels
                position.y = y * 16;
                targetPosition.x = x * 16;
                targetPosition.y = y * 16;
                return;  // Stop searching once a valid spawn point is found
            }
        }
    }

    // If no spawn point was found, you might want to handle it, e.g., by spawning the player at (0, 0)
    std::cout << "No valid spawn point found, spawning at (0, 0)" << std::endl;
    position.x = 0;
    position.y = 0;
}

void Player::update(std::vector<std::vector<char>> mapData, InputManager& pad, double dt) {
    Vector2 tilePos = { position.x / 16, position.y / 16 };

    // Update the hold timer if a button is pressed
    bool buttonPressed = pad.dpad_up || pad.dpad_down || pad.dpad_left || pad.dpad_right;
    if (buttonPressed) {
        holdTimer += dt;
    } else {
        holdTimer = 0.0;
        isHolding = false;
    }

    // Check for initial press or repeated movement
    bool canMove = false;
    if (pad.dpad_up && !pad.old_dpad_up) {
        canMove = true;  // Initial press
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
        canMove = true;  // Holding and enough time has passed
        holdTimer -= repeatDelay;  // Reset timer for next repeat
    }

    // Move if allowed
    if (canMove) {
        if (pad.dpad_up && mapData[tilePos.y - 1][tilePos.x] == ' ') {
            targetPosition.y -= h;
        } else if (pad.dpad_down && mapData[tilePos.y + 1][tilePos.x] == ' ') {
            targetPosition.y += h;
        } else if (pad.dpad_left && mapData[tilePos.y][tilePos.x - 1] == ' ') {
            targetPosition.x -= w;
        } else if (pad.dpad_right && mapData[tilePos.y][tilePos.x + 1] == ' ') {
            targetPosition.x += w;
        }
    }

    // Move the player towards the target position
    position.x += (targetPosition.x - position.x) * moveSpeed * dt;
    position.y += (targetPosition.y - position.y) * moveSpeed * dt;

    // Prevent overshooting the target position
    if (std::abs(position.y - targetPosition.y) < 0.5f * h) {
        position.y = targetPosition.y;
    }

    if (std::abs(position.x - targetPosition.x) < 0.5f * w) {
        position.x = targetPosition.x;
    }
}


Vector2 Player::getPosition() {
	return position;
}
