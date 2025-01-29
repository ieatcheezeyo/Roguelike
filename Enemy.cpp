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

Enemy::Enemy(SDL_Renderer* renderer) : renderer(renderer), scale(3), alerted(false) {

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
	if (texture) {
		SDL_DestroyTexture(texture);
	}
}

Enemy* Enemy::createEnemy(std::vector<std::vector<char>> mapData) {
	Enemy* newEnemy = new Enemy(renderer);

	newEnemy->type = static_cast<EnemyType>(std::rand() % EnemyType::COUNT);
	newEnemy->enemyDescriptor = EnemyDescriptors[std::rand() % EnemyDescriptors.size()];

	newEnemy->enemyStats.def = std::rand() % 10 + 1;
	newEnemy->enemyStats.atk = std::rand() % 10 + 1;
	newEnemy->enemyStats.speed = std::rand() % 5 + 1;

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

	newEnemy->texture = textureToEnemyTypeMap[newEnemy->type];

	return newEnemy;
}

void Enemy::update(std::vector<std::vector<char>>& mapData, double dt) {
	static bool seeded = false;
	if (!seeded) {
		srand(static_cast<unsigned int>(time(0)));
		seeded = true;
	}

	std::vector<std::pair<int, int>> directions = {
		{-1, 0},	// up
		{1, 0},		// down
		{0, -1},	// left
		{0, 1},		// right
		{0, 0}		//none
	};

	int directionIndex = rand() % 5;
	int dx = directions[directionIndex].first;
	int dy = directions[directionIndex].second;

	int newX = position.x + dx;
	int newY = position.y + dy;

	if (newX >= 0 && newX < mapData[0].size() && newY >= 0 && newY < mapData.size()) {
		if (mapData[newY][newX] == ' ' || mapData[newY][newX] == '.') {
			position.x = newX;
			position.y = newY;
		}
	}
}

