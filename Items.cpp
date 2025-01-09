#include "Items.h"

#include <cstdlib>
#include <ctime>

Items::Items(SDL_Renderer* renderer) : renderer(renderer) {
    std::srand(static_cast<unsigned>(std::time(0))); // Seed for random number generation
    initializeItemDefinitions();
}

Items::~Items() {
    for (SDL_Texture* texture : textures) {
        SDL_DestroyTexture(texture);
    }
    textures.clear();
}

void Items::initializeItemDefinitions() {
    itemDefinitions["Coin"] = "Assets/Images/Coin.png";
	itemDefinitions["HealthPotion"] = "Assets/Images/Health_Potion.png";
    // Add other items here (e.g., "Potion", "Sword")
}

SDL_Texture* Items::loadTexture(const std::string& file) {
    SDL_Texture* tempTexture = IMG_LoadTexture(renderer, file.c_str());
    if (!tempTexture) {
        std::cerr << "IMG_LoadTexture() Failed: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    textures.push_back(tempTexture);
    return tempTexture;
}

std::pair<int, int> Items::getRandomPosition() const {
    return { 0, 0 };
}

void Items::createItem(int x, int y, int scale, const std::string& name) {
    scale = scale;
    auto it = itemDefinitions.find(name);
    if (it == itemDefinitions.end()) {
        std::cerr << "Item definition not found for: " << name << std::endl;
        return;
    }

    SDL_Texture* texture = loadTexture(it->second);
    if (!texture) return;

    x = x;
    y = y;

    items.push_back(std::make_unique<Item>(name, texture, x, y));
}

void Items::updateItems(double dt) {
    for (auto& item : items) {
        item->update(dt);
    }
}
