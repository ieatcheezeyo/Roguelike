#include "Item.h"

Item::Item(const std::string& name, SDL_Texture* p_texture, int x, int y, int scale)
    : name(name), texture(p_texture), x(x), y(y), scale(scale) {
    w = 16 * scale;
    h = 16 * scale;
}

Item::~Item() {
    
}

void Item::update(double dt) {
    
}

//void Item::render(SDL_Renderer* renderer) {
//    SDL_Rect destRect = { x, y, w, h };
//    SDL_RenderCopy(renderer, texture, nullptr, &destRect);
//}
