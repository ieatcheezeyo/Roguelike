#pragma once

#include <SDL.h>
#include <string>

class Item {
public:
    Item(const std::string& name, SDL_Texture* p_texture, int x = 0, int y = 0, int scale = 1);
    ~Item();

    void update(double dt);
    //void render(SDL_Renderer* renderer);

    std::string name;
    SDL_Texture* texture;
    int x, y;
    int w, h;
    int scale;
};

