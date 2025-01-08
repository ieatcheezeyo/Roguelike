#include "Map.h"

Map::Map(int w, int h, SDL_Renderer* renderer) : renderer(renderer), w(w), h(h) {
    mapData.resize(h, std::vector<char>(w, 'W'));

    SDL_Texture* emptyTileTexture = loadTileTexture("Assets/Images/Empty_tile.png");
    SDL_Texture* wallTileTexture = loadTileTexture("Assets/Images/Wall_tile.png");

    tiles.push_back(Tile(' ', emptyTileTexture));
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
    if (x < 0 || x >= mapData[0].size() || y < 0 || y >= mapData.size()) {
        std::cerr << "Error: setTile out of bounds (" << x << ", " << y << ")" << std::endl;
        return;
    }
    mapData[y][x] = symbol;
}

char Map::getTileSymbol(int x, int y) const {
    if (y >= 0 && y < mapData.size() && x >= 0 && x < mapData[y].size()) {
        return mapData[y][x];
    }
    return '\0';  // Return a null character if out of bounds
}

int Map::floodFillAndCount(int startX, int startY) {
    int areaSize = 0;
    int nonWallCount = 0;
    std::queue<std::pair<int, int>> queue;
    queue.push({ startX, startY });
    mapData[startY][startX] = 'A' + currentAreaID;

    areaRegions.push_back(std::make_tuple(startX, startY, currentAreaID, nonWallCount));

    if (mapData[startY][startX] == ' ') {
        nonWallCount++;
    }

    while (!queue.empty()) {
        std::pair<int, int> current = queue.front();
        queue.pop();

        int x = current.first;
        int y = current.second;

        ++areaSize;  // Count the current cell as part of the area

        // Explore all 4 directions (up, down, left, right)
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dy == 0 && dx == 0) continue;  // Skip the current cell
                int nx = x + dx, ny = y + dy;
                if (nx >= 0 && nx < w && ny >= 0 && ny < h && mapData[ny][nx] == ' ') {
                    mapData[ny][nx] = 'F';
                    queue.push({ nx, ny });
                    nonWallCount++;
                }
            }
        }
    }

    areaRegions[currentAreaID - 1] = std::make_tuple(startX, startY, currentAreaID, nonWallCount);

    currentAreaID++;
    return areaSize;  // Return the size of the connected area
}

void Map::generateDungeon(Uint64 seed) {
    std::mt19937 rng(static_cast<unsigned int>(seed));
    std::uniform_int_distribution<int> dist(0, 1);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            mapData[y][x] = (dist(rng) == 0) ? 'W' : ' ';
        }
    }

    int centerX, centerY;
    std::uniform_int_distribution<int> quadrantDist(0, 4);

    int quadrant = quadrantDist(rng);

    if (quadrant == 0) {
        // Top-left quadrant
        centerX = w / 4 - 10;
        centerY = h / 4 - 10;
    } else if (quadrant == 1) {
        // Top-right quadrant
        centerX = 3 * w / 4 - 10;
        centerY = h / 4 - 10;
    } else if (quadrant == 2) {
        // Bottom-left quadrant
        centerX = w / 4 - 10;
        centerY = 3 * h / 4 - 10;
    } else if (quadrant == 3) {
        // Bottom-right quadrant
        centerX = 3 * w / 4 - 10;
        centerY = 3 * h / 4 - 10;
    } else {
        // Center of the map
        centerX = w / 2 - 10;
        centerY = h / 2 - 10;
    }

    // Ensure the carving area is within bounds
    centerX = std::max(0, std::min(centerX, w - 20));
    centerY = std::max(0, std::min(centerY, h - 20));

    // Step 4: Carve out a 20x20 empty space in the chosen area
    for (int y = centerY; y < centerY + 20; ++y) {
        for (int x = centerX; x < centerX + 20; ++x) {
            if (x >= 0 && x < w && y >= 0 && y < h) {
                mapData[y][x] = ' ';  // Clear this area (empty space)
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
                //std::cout << floodFillAndCount(x, y) << std::endl;
            }
        }
    }

    //removeSmallAreas();

    for (int y = 0; y < mapData.size(); y++) {
        for (int x = 0; x < mapData[0].size(); x++) {
            if (mapData[y][x] != 'W') {
                mapData[y][x] = ' ';
            }
        }
    }
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
                    if (mapData[y][x] == 'A' + areaId - 1) {
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

