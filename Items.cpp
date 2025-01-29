#include "Items.h"

#include <cstdlib>
#include <ctime>

Items::Items(SDL_Renderer* renderer) : renderer(renderer) {
    initializeItemDefinitions();
}

Items::~Items() {
    for (SDL_Texture* texture : textures) {
        std::cout << "<Items> Destroying Texture: " << texture << std::endl;
        SDL_DestroyTexture(texture);
    }
    textures.clear();
}

void Items::initializeItemDefinitions() {
    itemDefinitions["Health Potion"];
    itemDefinitions["Short Sword"];
    itemDefinitions["Wooden Shield"];
    itemDefinitions["Coin"];
    itemDefinitions["Raw Steak"];
}

SDL_Texture* Items::loadTexture(const std::string& file) {
    static std::map<std::string, SDL_Texture*> textureCache;

    if (textureCache.find(file) != textureCache.end()) {
        return textureCache[file];
    }

    SDL_Texture* texture = IMG_LoadTexture(renderer, file.c_str());
    if (!texture) {
        std::cerr << "Failed to load texture: " << file << "\n";
        return nullptr;
    }

    textureCache[file] = texture;
    textures.push_back(texture);
    return texture;
}

std::pair<int, int> Items::getRandomPosition() const {
    return { 0, 0 };
}

Item* Items::createItem(int x, int y, int scale, const std::string& name) {
    auto it = itemDefinitions.find(name);
    if (it == itemDefinitions.end()) {
        std::cerr << "Item name not defined: " << name << "\n";
        return nullptr;
    }

    SDL_Texture* texture = loadTexture("Assets/Images/Items/" + name + ".png");
    if (!texture) return nullptr;

    auto item = std::make_unique<Item>(name, texture, x, y, scale);
    Item* itemPtr = item.get();
    items.push_back(std::move(item));
    return itemPtr;
}

void Items::updateItems(double dt) {
    for (auto& item : items) {
        item->update(dt);
    }
}


