#include "Map.h"

Map::Map(int w, int h, int scale, SDL_Renderer* renderer) : items(renderer),  scale(scale), renderer(renderer), w(w), h(h) {
    mapData.resize(h, std::vector<char>(w, 'W'));

    SDL_Texture* emptyTileTexture_01 = loadTileTexture("Assets/Images/Empty_tile_01.png");
	SDL_Texture* emptyTileTexture_02 = loadTileTexture("Assets/Images/Empty_tile_02.png");
    SDL_Texture* wallTileTexture = loadTileTexture("Assets/Images/Wall_tile.png");

    tiles.push_back(Tile(' ', emptyTileTexture_01));
	tiles.push_back(Tile('.', emptyTileTexture_02));
    tiles.push_back(Tile('W', wallTileTexture));

    std::srand(static_cast<unsigned int>(std::time(0)));
}

Map::~Map() {
    for (SDL_Texture* texture : textures) {
        std::cout << "<Map> Destroying Texture: " << texture << std::endl;
        SDL_DestroyTexture(texture);
    }
    textures.clear();
    mapData.clear();
}

SDL_Texture* Map::loadTileTexture(const char* file) {
    SDL_Texture* tempTexture = IMG_LoadTexture(renderer, file);

    if (!tempTexture) {
        std::cerr << "<Map> IMG_LoadTexture() Failed: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    textures.push_back(tempTexture);
    return tempTexture;
}

void Map::setTile(int x, int y, char symbol) {
    x /= scale;
	y /= scale;

    if (x < 0 || x >= mapData[0].size() || y < 0 || y >= mapData.size()) {
        std::cerr << "Error: setTile out of bounds (" << x << ", " << y << ")" << std::endl;
        return;
    }
    mapData[y][x] = symbol;
}

char Map::getTileSymbol(int x, int y) const {
	x /= scale;
	y /= scale;
    
    if (y >= 0 && y < mapData.size() && x >= 0 && x < mapData[y].size()) {
        return mapData[y][x];
    }
    return '\0';
}

int Map::floodFillAndCount(int startX, int startY) {
    if (mapData[startY][startX] != ' ') {
        return 0;
    }

    int areaSize = 0;
    int nonWallCount = 0;
    std::queue<std::pair<int, int>> queue;
    queue.push({ startX, startY });
    int areaId = currentAreaID;
    mapData[startY][startX] = areaId;

    while (!queue.empty()) {
        auto [x, y] = queue.front();
        queue.pop();

        ++areaSize;

        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (std::abs(dy) == std::abs(dx)) continue;
                int nx = x + dx, ny = y + dy;
                if (nx >= 0 && nx < w && ny >= 0 && ny < h && mapData[ny][nx] == ' ') {
                    mapData[ny][nx] = areaId;
                    queue.push({ nx, ny });
                    ++nonWallCount;
                }
            }
        }
    }

    areaRegions.push_back(std::make_tuple(startX, startY, areaId, nonWallCount));
    

    //std::cout << "Flood-filled area ID: " << areaId
    //    << ", Size: " << areaSize
    //    << ", Non-wall count: " << nonWallCount << std::endl;

    currentAreaID++;
    return areaSize;
}

void Map::generateDungeon(Uint64 seed) {
    std::mt19937 rng(static_cast<unsigned int>(seed));
    std::uniform_int_distribution<int> dist(0, 1);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            mapData[y][x] = (dist(rng) == 0) ? 'W' : ' ';
        }
    }

    int centerX = (w / 2) / scale;
    int centerY = (h / 2) / scale;
    
    std::uniform_int_distribution<int> quadrantDist(0, 4);

    int quadrant = quadrantDist(rng);

    if (quadrant == 0) {
        centerX = w / 4 - 10;
        centerY = h / 4 - 10;
    } else if (quadrant == 1) {
        centerX = 3 * w / 4 - 10;
        centerY = h / 4 - 10;
    } else if (quadrant == 2) {
        centerX = w / 4 - 10;
        centerY = 3 * h / 4 - 10;
    } else if (quadrant == 3) {
        centerX = 3 * w / 4 - 10;
        centerY = 3 * h / 4 - 10;
    } else {
        centerX = w / 2 - 10;
        centerY = h / 2 - 10;
    }

    centerX = std::max(0, std::min(centerX, w - 20 / scale));
    centerY = std::max(0, std::min(centerY, h - 20 / scale));

    for (int y = centerY; y < centerY + 20 / scale; ++y) {
        for (int x = centerX; x < centerX + 20 / scale; ++x) {
            if (x >= 0 && x < w && y >= 0 && y < h) {
                mapData[y][x] = ' ';
            }
        }
    }

    int iterations = 5;
    for (int i = 0; i < iterations; ++i) {
        std::vector<std::vector<char>> newMap = mapData;

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                int neighborWalls = 0;

                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dy == 0 && dx == 0) continue;
                        int nx = x + dx, ny = y + dy;
                        if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                            if (mapData[ny][nx] == 'W') {
                                ++neighborWalls;
                            }
                        } else {
                            ++neighborWalls;
                        }
                    }
                }

                if (mapData[y][x] == 'W') {
                    if (neighborWalls < 4) {
                        newMap[y][x] = ' ';
                    }
                } else {
                    if (neighborWalls >= 5) {
                        newMap[y][x] = 'W';
                    }
                }
            }
        }

        mapData = newMap;
    }

    for (int y = 0; y < mapData.size(); y++) {
        for (int x = 0; x < mapData[0].size(); x++) {
            if (mapData[y][x] == ' ') {
                floodFillAndCount(x, y);
            }
        }
    }

    removeSmallAreas();

	connectAreas();
    connectAreas();

    for (int y = 0; y < mapData.size(); y++) {
        for (int x = 0; x < mapData[0].size(); x++) {
            if (mapData[y][x] != 'W') {
                mapData[y][x] = ' ';
            }
        }
    }

	randomizeGroundTiles();
	spawnItems();

}

void Map::removeSmallAreas() {
    for (auto it = areaRegions.begin(); it != areaRegions.end();) {
        int areaX, areaY, areaId, nonWallCount;
        std::tie(areaX, areaY, areaId, nonWallCount) = *it;

        std::cout << "Checking area ID: " << areaId
            << " with non-wall count: " << nonWallCount << std::endl;

        if (nonWallCount < 50) {
            std::cout << "Converting Area with ID " << areaId
                << " at (" << areaX << ", " << areaY
                << ") to walls due to " << nonWallCount << " empty tiles." << std::endl;

            for (int y = 0; y < h; ++y) {
                for (int x = 0; x < w; ++x) {
                    if (mapData[y][x] == areaId) {
                        mapData[y][x] = 'W';
                    }
                }
            }

            it = areaRegions.erase(it);
        } else {
            ++it;
        }
    }
}

void Map::connectAreas() {
    std::vector<std::pair<int, int>> centroids;
    for (const auto& region : areaRegions) {
        int areaX, areaY, areaId, nonWallCount;
        std::tie(areaX, areaY, areaId, nonWallCount) = region;
        centroids.push_back({ areaX, areaY });
    }

    std::sort(centroids.begin(), centroids.end());

    for (size_t i = 0; i < centroids.size() - 1; ++i) {
        auto [x1, y1] = centroids[i];
        auto [x2, y2] = centroids[i + 1];

        carvePath(x1, y1, x2, y2);
    }
}

void Map::carvePath(int x1, int y1, int x2, int y2) {
    int x = x1, y = y1;

    x1 /= scale;
	x2 /= scale;
	y1 /= scale;
	y2 /= scale;

    while (x != x2) {
        mapData[y][x] = ' ';
        x += (x2 > x) ? 1 : -1;
    }

    while (y != y2) {
        mapData[y][x] = ' ';
        y += (y2 > y) ? 1 : -1;
    }
}

void Map::randomizeGroundTiles() {
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			if (mapData[y][x] == ' ') {
				mapData[y][x] = (rand() % 2 == 0) ? '.' : ' ';
			}
		}
	}
}

void Map::spawnItems() {
    int maxItems = 10;
    int itemCount = 0;

    while (itemCount < maxItems) {
        int x = std::rand() % w;
        int y = std::rand() % h;

        if (mapData[y][x] == ' ') {
            int worldX = (x * 16) * scale;
            int worldY = (y * 16) * scale;

			int r = std::rand() % 2;

            switch (r) {
            case 0:
				items.createItem(worldX, worldY, scale, "Coin");
				break;
			case 1:
				items.createItem(worldX, worldY, scale, "HealthPotion");
                break;
            }
			++itemCount;

        }
    }
}

