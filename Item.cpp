#include "Item.h"

Item::Item(const std::string& name, SDL_Texture* p_texture, int x, int y, int scale) : name(name), texture(p_texture), x(x), y(y), scale(scale), atk(0), def(0), value(0), equipable(false) {
    w = 16 * scale;
    h = 16 * scale;
}

Item::~Item() {
    
}

void Item::update(double dt) {
    
}
