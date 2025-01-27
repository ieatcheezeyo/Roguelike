#pragma once

#include <SDL.h>
#include <string>

enum ItemType {
    recover_hp,
    poison,
    weapon,
    armor
};

class Item {
public:
    Item(const std::string& name, SDL_Texture* p_texture, int x, int y, int scale);
    ~Item();

    void update(double dt);

    std::string name;
    std::string description;
    bool equipable;
    int atk;
    int def;
    int value;

    SDL_Texture* texture;
    int x, y;
    int w, h;
    int scale;
};

