#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include "Item.h"

class Items {
public:
    Items(SDL_Renderer* renderer);
    ~Items();

    void createItem(int x, int y, int scale, const std::string& name);
    void updateItems(double dt);
    //void renderItems();
    SDL_Texture* loadTexture(const std::string& file);
    std::map<std::string, std::string> itemDefinitions;
    std::vector<std::unique_ptr<Item>> items;

private:
    int scale;
    SDL_Renderer* renderer;

    std::vector<SDL_Texture*> textures;

    void initializeItemDefinitions();
    std::pair<int, int> getRandomPosition() const;
};

