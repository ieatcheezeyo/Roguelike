#include "Item.h"

std::vector<std::vector<const char*>> ItemDescriptors = {
    // Treasure-related descriptors
    {"Ancient", "Golden", "Jeweled", "Mysterious", "Glowing", "Rare"},

    // HP recovery items
    {"Refreshing", "Healing", "Revitalizing", "Soothing", "Restorative"},

    // Poisonous items
    {"Toxic", "Venomous", "Cursed", "Rotten", "Corrupting", "Noxious"},

    // Weapons
    {"Sharp", "Heavy", "Legendary", "Enchanted", "Rusted", "Swift"},

    // Food items
    {"Delicious", "Stale", "Nutritious", "Moldy", "Exotic", "Spicy"},

    // Armor
    {"Sturdy", "Reinforced", "Shining", "Ancient", "Tattered", "Resilient"},

    // Shields
    {"Impenetrable", "Worn", "Decorated", "Rugged", "Engraved", "Holy"},

    // Rings
    {"Mystic", "Cursed", "Blessed", "Infernal", "Radiant", "Runed"},

    // Undefined/Misc
    {"Strange", "Unknown", "Mysterious", "Odd", "Otherworldly"}
};

Item::Item(const std::string& name, SDL_Texture* p_texture, int x, int y, int scale) : name(name), texture(p_texture), x(x), y(y), scale(scale), atk(0), def(0), value(0), equipable(false), equiped(false), type(undefined), dropable(false) {
    w = 16 * scale;
    h = 16 * scale;
}

Item::~Item() {
    
}

void Item::update(double dt) {
    
}
