#include "Enemy.h"

std::vector<const char*> EnemyDescriptors = {
	"Mongrel",
	"Mangy",
	"Fearless",
	"Withered",
	"Healthy",
	"Sickly",
	"Greedy",
	"Foul"
};

Enemy::Enemy(SDL_Renderer* renderer, Map& map) : renderer(renderer), scale(map.scale), alerted(false), followingPlayer(false), map(map), health(rand() % 100 + 10) {

	ratTexture = IMG_LoadTexture(renderer, "Assets/Images/Enemies/Rat.png");

	if (!ratTexture) {
		std::cout << "Error Loading Texture: " << IMG_GetError() << std::endl;
	}

	batTexture = IMG_LoadTexture(renderer, "Assets/Images/Enemies/Bat.png");

	if (!batTexture) {
		std::cout << "Error Loading Texture: " << IMG_GetError() << std::endl;
	}

	ghostTexture = IMG_LoadTexture(renderer, "Assets/Images/Enemies/Ghost.png");

	if (!ghostTexture) {
		std::cout << "Error Loading Texture: " << IMG_GetError() << std::endl;
	}

	gobblinTexture = IMG_LoadTexture(renderer, "Assets/Images/Enemies/Gobblin.png");

	if (!gobblinTexture) {
		std::cout << "Error Loading Texture: " << IMG_GetError() << std::endl;
	}

	textureToEnemyTypeMap[EnemyType::Rat] = ratTexture;
	textureToEnemyTypeMap[EnemyType::Bat] = batTexture;
	textureToEnemyTypeMap[EnemyType::Ghost] = ghostTexture;
	textureToEnemyTypeMap[EnemyType::Gobblin] = gobblinTexture;

}

Enemy::~Enemy() {
	//Don't run! will remove remaining enemy textures
	//if (texture) {
	//	std::cout << "<Enemy> Destroying Texture: " << texture << std::endl;
	//	SDL_DestroyTexture(texture);
	//}
}

Enemy* Enemy::createEnemy(std::vector<std::vector<char>> mapData) {
	Enemy* newEnemy = new Enemy(renderer, map);

	newEnemy->type = static_cast<EnemyType>(std::rand() % EnemyType::COUNT);
	newEnemy->enemyDescriptor = EnemyDescriptors[std::rand() % EnemyDescriptors.size()];

	newEnemy->enemyStats.def = std::rand() % 10 + 1;
	newEnemy->enemyStats.atk = std::rand() % 10 + 1;
	newEnemy->enemyStats.speed = std::rand() % 5 + 1;

    switch (newEnemy->type) {
        case EnemyType::Rat:
            newEnemy->name = "Rat";
            break;
        case EnemyType::Bat:
            newEnemy->name = "Bat";
            break;
        case EnemyType::Ghost:
            newEnemy->name = "Ghost";
            break;
        case EnemyType::Gobblin:
            newEnemy->name = "Gobblin";
            break;
        default:
            newEnemy->name = "Undefined";
            break;
    }

	Vector2 randomPosition;
	bool validPosition = false;

	while (!validPosition) {
		randomPosition.x = std::rand() % mapData[0].size();
		randomPosition.y = std::rand() % mapData.size();

		char tile = mapData[randomPosition.y][randomPosition.x];
		if (tile == ' ' || tile == '.') {
			validPosition = true;
		}
	}

	newEnemy->position = randomPosition;
	newEnemy->fovRadius = 5;
	newEnemy->playerStepsWithinFOV = 0;

	newEnemy->texture = textureToEnemyTypeMap[newEnemy->type];

	return newEnemy;
}

void Enemy::update(std::vector<std::vector<char>>& mapData, Player& player, std::vector<Enemy*>& enemies, double dt) {
    int playerTileX = player.position.x / (16 * map.scale);
    int playerTileY = player.position.y / (16 * map.scale);

    int dx = abs(playerTileX - position.x);
    int dy = abs(playerTileY - position.y);
    int distance = dx + dy;

    if (distance <= fovRadius) {
        if (!alerted && !followingPlayer) {
            alerted = true;
            playerStepsWithinFOV = 0;
        }
        playerStepsWithinFOV++;
    } else {
        alerted = false;
        playerStepsWithinFOV = 0;
        followingPlayer = false;
    }

    if (alerted && playerStepsWithinFOV >= 3) {
        followingPlayer = true;
        alerted = false;
    }

    int newX = position.x;
    int newY = position.y;

    if (followingPlayer) {
        if (playerTileX > position.x) newX++;
        else if (playerTileX < position.x) newX--;

        if (playerTileY > position.y) newY++;
        else if (playerTileY < position.y) newY--;
    } else {
        std::vector<std::pair<int, int>> directions = { {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {0, 0} };

        int directionIndex = rand() % directions.size();
        newX = position.x + directions[directionIndex].first;
        newY = position.y + directions[directionIndex].second;
    }

    bool isTileEmpty = (mapData[newY][newX] == ' ' || mapData[newY][newX] == '.');
    bool isNotOnPlayer = !(newX == playerTileX && newY == playerTileY);

    bool isNotOnOtherEnemy = true;
    for (Enemy* other : enemies) {
        if (other != this && other->position.x == newX && other->position.y == newY) {
            isNotOnOtherEnemy = false;
            break;
        }
    }

    if (newX >= 0 && newX < mapData[0].size() &&
        newY >= 0 && newY < mapData.size() &&
        isTileEmpty && isNotOnPlayer && isNotOnOtherEnemy) {
        position.x = newX;
        position.y = newY;
    }

    if (type == EnemyType::Ghost) {
        for (auto& item : map.items.items) {
            if (item->type != ItemType::treasure) {
                int tileX = item->x / (16 * map.scale);
                int tileY = item->y / (16 * map.scale);
                if (position.x == tileX && position.y == tileY) {
                    if (std::rand() % 100 < 30) {
                        item->cursed = true;
                        item->descriptor = "Cursed";
                        //std::cout << "Item Has Been Cursed" << std::endl;
                    } else {
                        //std::cout << "Ghost touched an item and didn't curse it." << std::endl;
                    }
                }
            }
        }
    }
}

