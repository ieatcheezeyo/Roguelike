#include "Map.h"

Map::Map(int w, int h, int scale, SDL_Renderer* renderer) : visited(h, std::vector<bool>(w, false)), currentAreaID(1), items(renderer),  scale(scale), renderer(renderer), w(w), h(h) {
    mapData.resize(h, std::vector<char>(w, 'W'));

    SDL_Texture* emptyTileTexture_01 = loadTileTexture("Assets/Images/Empty_tile_01.png");
	SDL_Texture* emptyTileTexture_02 = loadTileTexture("Assets/Images/Empty_tile_02.png");
    SDL_Texture* wallTileTexture = loadTileTexture("Assets/Images/Wall_tile.png");

	SDL_Texture* singleWallTexture = loadTileTexture("Assets/Images/Single_wall.png");

    SDL_Texture* verticalWallTopTexture = loadTileTexture("Assets/Images/Vertical_wall_top.png");
	SDL_Texture* verticalWallBottomTexture = loadTileTexture("Assets/Images/Vertical_wall_bottom.png");
	SDL_Texture* verticalWallTopAndBottomTexture = loadTileTexture("Assets/Images/Vertical_wall_top_and_bottom.png");

    SDL_Texture* horizontalWallRightTexture = loadTileTexture("Assets/Images/Horizontal_wall_right.png");
    SDL_Texture* horizontalWallLeftTexture = loadTileTexture("Assets/Images/Horizontal_wall_left.png");
	SDL_Texture* horizontalWallRightAndLeftTexture = loadTileTexture("Assets/Images/Horizontal_wall_right_and_left.png");

	SDL_Texture* cornerLeftAndBottomTexture = loadTileTexture("Assets/Images/Corner_left_and_bottom.png");
	SDL_Texture* cornerRightAndBottomTexture = loadTileTexture("Assets/Images/Corner_right_and_bottom.png");
	SDL_Texture* cornerTopAndRightTexture = loadTileTexture("Assets/Images/Corner_top_and_right.png");
	SDL_Texture* cornerTopAndLeftTexture = loadTileTexture("Assets/Images/Corner_top_and_left.png");

	SDL_Texture* wallRightLeftAndBottomTexture = loadTileTexture("Assets/Images/Wall_right_left_and_bottom.png");
	SDL_Texture* wallRightLeftAndTopTexture = loadTileTexture("Assets/Images/Wall_right_left_and_top.png");
	SDL_Texture* wallRightTopAndBottomTexture = loadTileTexture("Assets/Images/Wall_right_top_and_bottom.png");
	SDL_Texture* wallLeftTopAndBottomTexture = loadTileTexture("Assets/Images/Wall_left_top_and_bottom.png");

	SDL_Texture* innerCornerRightAndBottomTexture = loadTileTexture("Assets/Images/Inner_corner_right_bottom.png");
	SDL_Texture* innerCornerLeftAndBottomTexture = loadTileTexture("Assets/Images/Inner_corner_left_bottom.png");
	SDL_Texture* innerCornerLeftAndTopTexture = loadTileTexture("Assets/Images/Inner_corner_left_top.png");
	SDL_Texture* innerCornerRightAndTopTexture = loadTileTexture("Assets/Images/Inner_corner_right_top.png");

    SDL_Texture* surroundedTileTexture = loadTileTexture("Assets/Images/Surrounded_tile.png");

    tiles.push_back(Tile(' ', emptyTileTexture_01));
	tiles.push_back(Tile('.', emptyTileTexture_02));
    tiles.push_back(Tile('W', wallTileTexture));

	tiles.push_back(Tile('A', singleWallTexture));

	tiles.push_back(Tile('B', verticalWallTopTexture));
    tiles.push_back(Tile('C', verticalWallBottomTexture));
	tiles.push_back(Tile('D', verticalWallTopAndBottomTexture));

	
    tiles.push_back(Tile('E', horizontalWallRightTexture));
	tiles.push_back(Tile('F', horizontalWallLeftTexture));
    tiles.push_back(Tile('G', horizontalWallRightAndLeftTexture));

	tiles.push_back(Tile('H', cornerLeftAndBottomTexture));
	tiles.push_back(Tile('I', cornerRightAndBottomTexture));
	tiles.push_back(Tile('J', cornerTopAndRightTexture));
	tiles.push_back(Tile('K', cornerTopAndLeftTexture));

	tiles.push_back(Tile('L', wallRightLeftAndBottomTexture));
	tiles.push_back(Tile('M', wallRightLeftAndTopTexture));
	tiles.push_back(Tile('N', wallRightTopAndBottomTexture));
	tiles.push_back(Tile('O', wallLeftTopAndBottomTexture));

	tiles.push_back(Tile('P', innerCornerRightAndBottomTexture));
	tiles.push_back(Tile('Q', innerCornerLeftAndBottomTexture));
	tiles.push_back(Tile('R', innerCornerRightAndTopTexture));
	tiles.push_back(Tile('S', innerCornerLeftAndTopTexture));

	tiles.push_back(Tile('Z', surroundedTileTexture));

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
    if (visited[startY][startX] || mapData[startY][startX] != ' ') {
        return 0;
    }

    int areaSize = 0;
    int nonWallCount = 0;
    std::queue<std::pair<int, int>> queue;
    queue.push({ startX, startY });
    int areaId = currentAreaID;

    mapData[startY][startX] = areaId;
    visited[startY][startX] = true;

    //std::cout << "Starting flood fill with areaId: " << areaId << " at (" << startX << ", " << startY << ")" << std::endl;

    while (!queue.empty()) {
        auto [x, y] = queue.front();
        queue.pop();

        ++areaSize;

        const int directions[4][2] = { {0, 1}, {0, -1}, {1, 0}, {-1, 0} };
        for (const auto& [dx, dy] : directions) {
            int nx = x + dx, ny = y + dy;

            if (nx >= 0 && nx < w && ny >= 0 && ny < h && mapData[ny][nx] == ' ' && !visited[ny][nx]) {
                mapData[ny][nx] = areaId;
                visited[ny][nx] = true;

                queue.push({ nx, ny });
                ++nonWallCount;
            }
        }
    }

    if (areaSize == 1) {
        //std::cout << "Single-tile region at (" << startX << ", " << startY << ")\n";
    }

    areaRegions.push_back(std::make_tuple(startX, startY, areaId, nonWallCount));

    currentAreaID++;

    return areaSize;
}


void Map::generateDungeon(Uint64 seed) {

	std::cout << "Seed: " << seed << " Map Size: (" << w << ")(" << h << ")" << std::endl;

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
                int areaSize = floodFillAndCount(x, y);
                if (areaSize > 0) {
                    //std::cout << "Region Starting at (" << x << ", " << y << ") has size: " << areaSize << std::endl;
                }
            }
        }
    }

    fillEdges();
    removeSmallAreas();
	connectAreas();
    
    removeSmallAreas();
    connectAreas();

    for (int y = 1; y < mapData.size() - 1; y++) {
        for (int x = 1; x < mapData[0].size() - 1; x++) {
            if (mapData[y][x] != 'W') {
                mapData[y][x] = ' ';
            }
        }
    }

    ensureConnectivity();

	randomizeGroundTiles();
	applyBitmasking();

    cleanIsolatedTiles();

	spawnItems();
    spawnEnemies();

}

void Map::ensureConnectivity() {
    std::vector<std::vector<int>> regionMap(h, std::vector<int>(w, -1));
    int currentRegionId = 0;

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (mapData[y][x] == ' ' && regionMap[y][x] == -1) {
                floodFillAssignRegion(x, y, regionMap, currentRegionId++);
            }
        }
    }

    std::unordered_map<int, std::pair<int, int>> regionCentroids;
    std::unordered_map<int, int> regionSizes;

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int regionId = regionMap[y][x];
            if (regionId != -1) {
                regionCentroids[regionId].first += x;
                regionCentroids[regionId].second += y;
                regionSizes[regionId]++;
            }
        }
    }

    for (auto& [regionId, centroid] : regionCentroids) {
        centroid.first /= regionSizes[regionId];
        centroid.second /= regionSizes[regionId];
    }

    int mainRegionId = std::max_element(
        regionSizes.begin(), regionSizes.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; }
    )->first;

    for (const auto& [regionId, centroid] : regionCentroids) {
        if (regionId != mainRegionId) {
            auto [x1, y1] = centroid;
            auto [x2, y2] = regionCentroids[mainRegionId];
            drunkardsWalk(x1, y1, x2, y2);
        }
    }
}

void Map::floodFillAssignRegion(int x, int y, std::vector<std::vector<int>>& regionMap, int regionId) {
    std::queue<std::pair<int, int>> toVisit;
    toVisit.push({ x, y });
    regionMap[y][x] = regionId;

    while (!toVisit.empty()) {
        auto [cx, cy] = toVisit.front();
        toVisit.pop();

        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (std::abs(dx) + std::abs(dy) != 1) continue;

                int nx = cx + dx;
                int ny = cy + dy;

                if (nx >= 0 && nx < w && ny >= 0 && ny < h && mapData[ny][nx] == ' ' && regionMap[ny][nx] == -1) {
                    regionMap[ny][nx] = regionId;
                    toVisit.push({ nx, ny });
                }
            }
        }
    }
}

void Map::removeSmallAreas() {
    for (auto it = areaRegions.begin(); it != areaRegions.end();) {
        int areaX, areaY, areaId, nonWallCount;
        std::tie(areaX, areaY, areaId, nonWallCount) = *it;

        if (nonWallCount < 400) {
            int nearestX = -1, nearestY = -1, nearestId = -1, minDistance = INT_MAX;
            for (const auto& region : areaRegions) {
                int targetX, targetY, targetId, targetCount;
                std::tie(targetX, targetY, targetId, targetCount) = region;

                if (targetId != areaId && targetCount >= 400) {
                    int distance = std::abs(areaX - targetX) + std::abs(areaY - targetY);
                    if (distance < minDistance) {
                        nearestX = targetX;
                        nearestY = targetY;
                        nearestId = targetId;
                        minDistance = distance;
                    }
                }
            }

            if (nearestId != -1) {
                drunkardsWalk(areaX, areaY, nearestX, nearestY);
            }

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

void Map::drunkardsWalk(int x1, int y1, int x2, int y2) {
    int currentX = x1, currentY = y1;

    while (currentX != x2 || currentY != y2) {
        if (currentX <= 1 || currentX >= w - 2 ||
            currentY <= 1 || currentY >= h - 2) {
            break;
        }

        mapData[currentY][currentX] = ' ';

        int dx = x2 - currentX;
        int dy = y2 - currentY;

        int distance = std::abs(dx) + std::abs(dy);
        bool randomDeviation = (std::rand() % 10 < (distance > 5 ? 5 : 3));

        if (randomDeviation) {
            int randomSteps = 1 + std::rand() % 3;
            for (int i = 0; i < randomSteps; ++i) {
                int randomDir = std::rand() % 4;
                int newX = currentX, newY = currentY;

                if (randomDir == 0 && currentX > 1) --newX;                 // Left
                else if (randomDir == 1 && currentX < w - 2) ++newX;        // Right
                else if (randomDir == 2 && currentY > 1) --newY;            // Up
                else if (randomDir == 3 && currentY < h - 2) ++newY;        // Down

                if (newX > 1 && newX < w - 2 &&
                    newY > 1 && newY < h - 2) {
                    currentX = newX;
                    currentY = newY;
                    mapData[currentY][currentX] = ' ';
                }
            }
        } else {
            if (std::rand() % 2 == 0) {
                if (dx != 0) currentX += (dx > 0 ? 1 : -1);
                else if (dy != 0) currentY += (dy > 0 ? 1 : -1);
            } else {
                if (dy != 0) currentY += (dy > 0 ? 1 : -1);
                else if (dx != 0) currentX += (dx > 0 ? 1 : -1);
            }
        }
    }
}

void Map::cleanIsolatedTiles() {
    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<int> dist(0, 1);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (mapData[y][x] == 'A') {
                bool hasAdjacentOpenTile = false;

                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dy == 0) continue;
                        int nx = x + dx, ny = y + dy;
                        if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                            if (mapData[ny][nx] == ' ' || mapData[ny][nx] == '.') {
                                //std::cout << "Found Isolated Wall at: " << nx << ", " << ny << std::endl;
                                hasAdjacentOpenTile = true;
                                break;
                            }
                        }
                    }
                    if (hasAdjacentOpenTile) break;
                }

                if (hasAdjacentOpenTile) {
                    mapData[y][x] = (dist(rng) == 0) ? ' ' : '.';
                }
            }
        }
    }
}

void Map::fillEdges() {
    for (int x = 0; x < w; ++x) {
        mapData[0][x] = 'W';                // Top edge
        mapData[h - 1][x] = 'W';            // Bottom edge
    }
    for (int y = 0; y < h; ++y) {
        mapData[y][0] = 'W';                // Left edge
        mapData[y][w - 1] = 'W';            // Right edge
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

        drunkardsWalk(x1, y1, x2, y2);
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

void Map::applyBitmasking() {
    //std::cout << "Applying Bitmasking..." << std::endl;

    std::vector<std::vector<char>> newMap = mapData;

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (mapData[y][x] != 'W') continue;

            int bitmask = 0;

            auto isWall = [&](int ny, int nx) {
                if (ny < 0 || ny >= h || nx < 0 || nx >= w) return false;
                return mapData[ny][nx] == 'W';
            };

            bitmask |= isWall(y - 1, x) ? 1 : 0;           // North
            bitmask |= isWall(y - 1, x + 1) ? 2 : 0;       // NE
            bitmask |= isWall(y, x + 1) ? 4 : 0;           // East
            bitmask |= isWall(y + 1, x + 1) ? 8 : 0;       // SE
            bitmask |= isWall(y + 1, x) ? 16 : 0;          // South
            bitmask |= isWall(y + 1, x - 1) ? 32 : 0;      // SW
            bitmask |= isWall(y, x - 1) ? 64 : 0;          // West
            bitmask |= isWall(y - 1, x - 1) ? 128 : 0;     // NW

            if (bitmaskToTile.find(bitmask) == bitmaskToTile.end()) {
                std::ofstream logFile("unmatched_bitmasks.log", std::ios::app);
                logFile << "Unmatched bitmask: " << std::bitset<8>(bitmask)
                    << " at (" << x << ", " << y << ")\n";
                logFile << "Neighbors:\n";
                for (int i = -1; i <= 1; ++i) {
                    for (int j = -1; j <= 1; ++j) {
                        if (y + i >= 0 && y + i < h && x + j >= 0 && x + j < w) {
                            logFile << mapData[y + i][x + j] << " ";
                        } else {
                            logFile << "X ";
                        }
                    }
                    logFile << "\n";
                }
                logFile.close();

            }

            newMap[y][x] = (bitmaskToTile.find(bitmask) != bitmaskToTile.end())
                ? bitmaskToTile[bitmask]
                : 'W';
        }
    }

    mapData = newMap;

    // Debugging output for updated map
    //std::cout << "Updated Map:" << std::endl;
    //for (const auto& row : mapData) {
    //    for (char tile : row) std::cout << tile;
    //    std::cout << std::endl;
    //}
}

void Map::spawnItems() {
    int maxItems = 50;
    int itemCount = 0;

    while (itemCount < maxItems) {
        int x = std::rand() % w;
        int y = std::rand() % h;

        if (mapData[y][x] == ' ') {
            int worldX = (x * 16) * scale;
            int worldY = (y * 16) * scale;

			int r = std::rand() % 5;

            Item* newItem = nullptr;
            switch (r) {
            case 0:
                newItem = items.createItem(worldX, worldY, scale, "Coin");
                if (newItem) {
                    newItem->atk = 0;
                    newItem->def = 0;
                    newItem->value = 1;
                    newItem->description = "Shit, How did you see this?";
                    newItem->equipable = false;
                    newItem->dropable = false;
                    newItem->cursed = false;
                    newItem->type = treasure;
                }
				break;
			case 1:
                newItem = items.createItem(worldX, worldY, scale, "Health Potion");
                if (newItem) {
                    newItem->atk = 0;
                    newItem->def = 0;
                    newItem->value = 10;
                    newItem->description = "Restores some health points.";
                    newItem->equipable = false;
                    newItem->dropable = true;
                    newItem->cursed = false;
                    newItem->type = recover_hp;
                }
                break;
            case 2:
                newItem = items.createItem(worldX, worldY, scale, "Short Sword");
                if (newItem) {
                    newItem->atk = rand() % 20;
                    newItem->def = 0;
                    newItem->value = 100 + newItem->atk / 2;
                    newItem->description = "A standard short sword, good for swinging.";
                    newItem->equipable = true;
                    newItem->dropable = true;
                    newItem->cursed = false;
                    newItem->type = weapon;
                }
                break;
            case 3:
                newItem = items.createItem(worldX, worldY, scale, "Wooden Shield");
                if (newItem) {
                    newItem->atk = 0;
                    newItem->def = rand() % 15;
                    newItem->value = 50 + newItem->def / 2;
                    newItem->description = "A puny wooden shield, better than nothing.";
                    newItem->equipable = true;
                    newItem->dropable = true;
                    newItem->cursed = false;
                    newItem->type = shield;
                }
                break;
            case 4:
                newItem = items.createItem(worldX, worldY, scale, "Raw Steak");
                if (newItem) {
                    newItem->atk = 0;
                    newItem->def = 0;
                    newItem->value = 15;
                    newItem->description = "A Raw Steak you found on the floor.";
                    newItem->equipable = false;
                    newItem->dropable = true;
                    newItem->cursed = false;
                    newItem->type = food;
                }
                break;
            }
			++itemCount;

        }
    }
}

void Map::spawnEnemies() {
    Enemy enemySpawner(renderer, *this);
    for (int i = 0; i < 130; i++) {
        enemies.push_back(enemySpawner.createEnemy(mapData));
    }
}