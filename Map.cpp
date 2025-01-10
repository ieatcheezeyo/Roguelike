#include "Map.h"

Map::Map(int w, int h, int scale, SDL_Renderer* renderer) : items(renderer),  scale(scale), renderer(renderer), w(w), h(h) {
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


    //std::srand(static_cast<unsigned int>(std::time(0)));
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
                floodFillAndCount(x, y);
            }
        }
    }

    removeSmallAreas();

	connectAreas();
    
    removeSmallAreas();

    for (int y = 0; y < mapData.size(); y++) {
        for (int x = 0; x < mapData[0].size(); x++) {
            if (mapData[y][x] != 'W') {
                mapData[y][x] = ' ';
            }
        }
    }

	randomizeGroundTiles();
	applyBitmasking();
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

void Map::drunkardsWalk(int x1, int y1, int x2, int y2) {
    int currentX = x1, currentY = y1;

    while (currentX != x2 || currentY != y2) {
        // Check bounds to avoid edge carving
        if (currentX <= 1 || currentX >= w - 2 ||
            currentY <= 1 || currentY >= h - 2) {
            break; // Stop carving if near the edge
        }

        mapData[currentY][currentX] = ' '; // Carve the current tile

        // Calculate direction to target
        int dx = x2 - currentX;
        int dy = y2 - currentY;

        // Chance of random deviation (higher when farther from the target)
        int distance = std::abs(dx) + std::abs(dy);
        bool randomDeviation = (std::rand() % 10 < (distance > 5 ? 5 : 3)); // Higher randomness if farther

        if (randomDeviation) {
            // Multi-step random deviation
            int randomSteps = 1 + std::rand() % 3; // 1 to 3 random steps
            for (int i = 0; i < randomSteps; ++i) {
                int randomDir = std::rand() % 4;
                int newX = currentX, newY = currentY;

                if (randomDir == 0 && currentX > 1) --newX;               // Left
                else if (randomDir == 1 && currentX < w - 2) ++newX; // Right
                else if (randomDir == 2 && currentY > 1) --newY;          // Up
                else if (randomDir == 3 && currentY < h - 2) ++newY; // Down

                // Carve only if within bounds
                if (newX > 1 && newX < w - 2 &&
                    newY > 1 && newY < h - 2) {
                    currentX = newX;
                    currentY = newY;
                    mapData[currentY][currentX] = ' '; // Carve
                }
            }
        } else {
            // Favor movement toward the destination
            if (std::rand() % 2 == 0) { // Randomly choose axis priority
                if (dx != 0) currentX += (dx > 0 ? 1 : -1);
                else if (dy != 0) currentY += (dy > 0 ? 1 : -1);
            } else {
                if (dy != 0) currentY += (dy > 0 ? 1 : -1);
                else if (dx != 0) currentX += (dx > 0 ? 1 : -1);
            }
        }
    }
}

void Map::fillEdges() {
    // Fill edges with walls
    for (int x = 0; x < w; ++x) {
        mapData[0][x] = 'W';               // Top edge
        mapData[h - 1][x] = 'W';   // Bottom edge
    }
    for (int y = 0; y < h; ++y) {
        mapData[y][0] = 'W';               // Left edge
        mapData[y][w - 1] = 'W';    // Right edge
    }
}

void Map::connectAreas() {
    fillEdges();

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
    std::cout << "Applying Bitmasking..." << std::endl;

    // Updated bitmask-to-tile map
    std::unordered_map<int, char> bitmaskToTile = {
        {0b00000000, 'A'}, // Isolated single wall
        {0b00000010, 'A'},
        {0b10000000, 'A'},
        {0b00100000, 'A'},
        {0b00000001, 'B'}, // Vertical wall top
        {0b00000011, 'B'},
        {0b00100001, 'B'},
        {0b00010010, 'B'},
        {0b10000011, 'B'},
        {0b00011000, 'C'}, // Vertical wall bottom
        {0b00110000, 'C'},
        {0b00010000, 'C'},
        {0b00111000, 'C'},
        {0b00010001, 'D'}, // Vertical wall top and bottom
        {0b00110001, 'D'},
        {0b10010001, 'D'},
        {0b10011001, 'D'},
        {0b00000100, 'E'}, // Horizontal wall right
        {0b00001100, 'E'},
        {0b00000110, 'E'},
        {0b00001110, 'E'},
        {0b01000000, 'F'}, // Horizontal wall left
        {0b01100000, 'F'},
        {0b01001000, 'F'},
        {0b00000101, 'G'}, // Horizontal wall right and left
        {0b11100100, 'G'},
        {0b11001100, 'G'},
        {0b01010000, 'H'}, // Corner left-bottom
        {0b11010000, 'H'},
        {0b11110000, 'H'},
        {0b01111000, 'H'},
        {0b11111000, 'H'},
        {0b01110000, 'H'},
        {0b00010100, 'I'}, // Corner right-bottom
        {0b01110011, 'I'},
        {0b00011100, 'I'},
        {0b00110110, 'I'},
        {0b00110100, 'I'},
        {0b00111110, 'I'},
        {0b00011110, 'I'},
        {0b00111100, 'I'},
        {0b10001111, 'J'}, // Corner top-right
        {0b00001101, 'J'},
        {0b00001111, 'J'},
        {0b00100111, 'J'},
        {0b10100111, 'J'},
        {0b00000111, 'J'},
        {0b10000111, 'J'},
        {0b11100011, 'K'}, // Corner top-left
        {0b01100011, 'K'},
        {0b11000001, 'K'},
        {0b11100001, 'K'},
        {0b11000011, 'K'},
        {0b01000101, 'L'}, // WallRightLeftAndBottom
        {0b11111110, 'L'},
        {0b01011100, 'L'},
        {0b01011110, 'L'},
        {0b11111100, 'L'},
        {0b01110110, 'L'},
        {0b01111110, 'L'},
        {0b01110100, 'L'},
        {0b01111100, 'L'},
        {0b11101111, 'M'}, // WallRightLeftAndTop
        {0b01001111, 'M'},
        {0b10111111, 'M'},
        {0b11100101, 'M'},
        {0b01100111, 'M'},
        {0b11001111, 'M'},
        {0b11100111, 'M'},
        {0b11000111, 'M'},
        {0b00011111, 'N'}, // WallRightTopAndBottom
        {0b10010111, 'N'},
        {0b10011111, 'N'},
        {0b00110111, 'N'},
        {0b00111111, 'N'},
        {0b11110011, 'O'}, // WallLeftTopAndBottom
        {0b01111011, 'O'},
        {0b11011001, 'O'},
        {0b11010001, 'O'},
        {0b01110001, 'O'},
        {0b11111001, 'O'},
        {0b11110001, 'O'},
        {0b11110111, 'P'}, //InnerCornerRightandBottom
		{0b11011111, 'Q'}, //InnerCornerLeftandBottom

		{0b11111101, 'R'}, //InnerCornerLeftandTop
		{0b01111111, 'S'}, //InnerCornerRightandTop
        
        {0b11111111, 'Z'}, // Fully surrounded by walls
    };

    std::vector<std::vector<char>> newMap = mapData;

    for (int y = 1; y < h - 1; ++y) {
        for (int x = 1; x < w - 1; ++x) {
            if (mapData[y][x] != 'W') continue; // Skip non-wall tiles

            int bitmask = 0;

            // Calculate the bitmask for the current tile
            bitmask |= (mapData[y - 1][x] == 'W') ? 1 : 0;       // North
            bitmask |= (mapData[y - 1][x + 1] == 'W') ? 2 : 0;   // North-East
            bitmask |= (mapData[y][x + 1] == 'W') ? 4 : 0;       // East
            bitmask |= (mapData[y + 1][x + 1] == 'W') ? 8 : 0;   // South-East
            bitmask |= (mapData[y + 1][x] == 'W') ? 16 : 0;      // South
            bitmask |= (mapData[y + 1][x - 1] == 'W') ? 32 : 0;  // South-West
            bitmask |= (mapData[y][x - 1] == 'W') ? 64 : 0;      // West
            bitmask |= (mapData[y - 1][x - 1] == 'W') ? 128 : 0; // North-West

            // Debugging output for unmatched cases
            //if (bitmaskToTile.find(bitmask) == bitmaskToTile.end()) {
            //    std::cout << "Unmatched bitmask: " << std::bitset<8>(bitmask)
            //        << " at (" << x << ", " << y << ")" << std::endl;

                // Print neighboring tiles for context
                //std::cout << "Neighbors:\n";
                //for (int i = -1; i <= 1; ++i) {
                //    for (int j = -1; j <= 1; ++j) {
                //        std::cout << mapData[y + i][x + j] << " ";
                //    }
                //    std::cout << std::endl;
                //}
            //}

            if (bitmaskToTile.find(bitmask) == bitmaskToTile.end()) {
                std::ofstream logFile("unmatched_bitmasks.log", std::ios::app);
                logFile << "Unmatched bitmask: " << std::bitset<8>(bitmask)
                    << " at (" << x << ", " << y << ")\n";
                logFile << "Neighbors:\n";
                for (int i = -1; i <= 1; ++i) {
                    for (int j = -1; j <= 1; ++j) {
                        logFile << mapData[y + i][x + j] << " ";
                    }
                    logFile << "\n";
                }
                logFile.close();
            }

            // Assign to new map based on bitmask
            newMap[y][x] = (bitmaskToTile.find(bitmask) != bitmaskToTile.end())
                ? bitmaskToTile[bitmask]
                : 'W'; // Default wall tile
        }
    }

    // Copy the temporary map back to mapData
    mapData = newMap;

    // Debugging output for updated map
    //std::cout << "Updated Map:" << std::endl;
    //for (const auto& row : mapData) {
    //    for (char tile : row) std::cout << tile;
    //    std::cout << std::endl;
    //}
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

