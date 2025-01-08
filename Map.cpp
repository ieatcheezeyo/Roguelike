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

void Map::generateDungeon() {
    // Initialize the root BSP node covering the entire map
    BSPNode* root = new BSPNode(0, 0, w, h);

    // Split the space recursively
    splitBSP(root, 6, 12);  // min and max room size

    // Create rooms in the leaf nodes
    createRoom(root, 6, 12);

    // Connect the rooms
    connectRooms();
}

void Map::splitBSP(BSPNode* node, int minRoomSize, int maxRoomSize) {
    // Don't split if the node is too small
    if (node->w <= minRoomSize * 2 || node->h <= minRoomSize * 2) {
        return;
    }

    int splitDirection = std::rand() % 2;  // 0 for horizontal, 1 for vertical

    if (splitDirection == 0 && node->w > minRoomSize * 2) {
        // Horizontal split: Split along the width of the node
        int splitX = std::rand() % (node->w - minRoomSize * 2) + minRoomSize;
        node->left = new BSPNode(node->x, node->y, splitX, node->h);
        node->right = new BSPNode(node->x + splitX, node->y, node->w - splitX, node->h);
    }
    else if (splitDirection == 1 && node->h > minRoomSize * 2) {
        // Vertical split: Split along the height of the node
        int splitY = std::rand() % (node->h - minRoomSize * 2) + minRoomSize;
        node->left = new BSPNode(node->x, node->y, node->w, splitY);
        node->right = new BSPNode(node->x, node->y + splitY, node->w, node->h - splitY);
    }

    // Recursively split further on both new nodes
    if (node->left) splitBSP(node->left, minRoomSize, maxRoomSize);
    if (node->right) splitBSP(node->right, minRoomSize, maxRoomSize);
}

void Map::createRoom(BSPNode* node, int minRoomSize, int maxRoomSize) {
    if (!node) return;

    // Only create rooms at leaf nodes
    if (!node->left && !node->right) {
        // Room size is random within the given range
        int roomWidth = std::rand() % (maxRoomSize - minRoomSize + 1) + minRoomSize;
        int roomHeight = std::rand() % (maxRoomSize - minRoomSize + 1) + minRoomSize;

        // Place the room inside the node's boundaries
        int roomX = std::rand() % (node->w - roomWidth - 1) + node->x;
        int roomY = std::rand() % (node->h - roomHeight - 1) + node->y;

        // Fill the room with empty space
        for (int y = roomY; y < roomY + roomHeight; ++y) {
            for (int x = roomX; x < roomX + roomWidth; ++x) {
                mapData[y][x] = ' ';
            }
        }

        // Store the center of the room
        int centerX = roomX + roomWidth / 2;
        int centerY = roomY + roomHeight / 2;
        roomCenters.push_back({ centerX, centerY });
    }
    else {
        // Recursively create rooms for child nodes
        createRoom(node->left, minRoomSize, maxRoomSize);
        createRoom(node->right, minRoomSize, maxRoomSize);
    }
}

void Map::connectRooms() {
    for (size_t i = 1; i < roomCenters.size(); ++i) {
        int x1 = roomCenters[i - 1].first;
        int y1 = roomCenters[i - 1].second;
        int x2 = roomCenters[i].first;
        int y2 = roomCenters[i].second;

        // Horizontal corridor (move x-axis from x1 to x2)
        if (x1 != x2) {
            int minX = std::min(x1, x2);
            int maxX = std::max(x1, x2);
            for (int x = minX; x <= maxX; ++x) {
                mapData[y1][x] = ' ';
            }
        }

        // Vertical corridor (move y-axis from y1 to y2)
        if (y1 != y2) {
            int minY = std::min(y1, y2);
            int maxY = std::max(y1, y2);
            for (int y = minY; y <= maxY; ++y) {
                mapData[y][x1] = ' ';
            }
        }
    }
}