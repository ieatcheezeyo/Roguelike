#pragma once

#include <SDL.h>
#include <string>

enum ItemType {
    treasure,
    recover_hp,
    poison,
    weapon,
    food,
    armor,
    shield,
    ring,
    undefined
};

class Item {
public:
    Item(const std::string& name, SDL_Texture* p_texture, int x, int y, int scale);
    ~Item();

    void update(double dt);

    std::string name;
    std::string description;
    bool equipable;
    bool equiped;
    bool dropable;
    bool cursed;
    int atk;
    int def;
    int value;
    ItemType type;

    SDL_Texture* texture;
    int x, y;
    int w, h;
    int scale;
};

