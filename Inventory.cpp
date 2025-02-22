#include "Inventory.h"
#include <iostream>

Inventory::Inventory(int inventorySize) : inventorySize(inventorySize), index(0) {
    inventory.resize(inventorySize, nullptr);
}

Inventory::Inventory() : inventorySize(10), index(0) {
    inventory.resize(inventorySize, nullptr);
}

Inventory::~Inventory() {
    for (auto item : inventory) {
        delete item;
    }
    inventory.clear();
}

bool Inventory::addItem(Item* item) {
    if (!item) {
        //std::cerr << "Cannot add a null item to the inventory.\n";
        return false;
    }

    for (size_t i = 0; i < inventory.size(); ++i) {
        if (inventory[i] == nullptr) {
            inventory[i] = new Item(*item);
            return true;
        }
    }

    //std::cerr << "Inventory full! Cannot add item: " << item->name << ".\n";
    return false;
}

void Inventory::compact() {
    size_t writeIndex = 0;

    for (size_t readIndex = 0; readIndex < inventory.size(); ++readIndex) {
        if (inventory[readIndex] != nullptr) {
            inventory[writeIndex] = inventory[readIndex];
            ++writeIndex;
        }
    }

    for (size_t i = writeIndex; i < inventory.size(); ++i) {
        inventory[i] = nullptr;
    }
}

bool Inventory::removeItem(Item* item) {
    auto it = std::find(inventory.begin(), inventory.end(), item);
    if (it != inventory.end()) {
        *it = nullptr;
        return true;
    }
    //std::cerr << "Item not found in inventory!\n";
    return false;
}

void Inventory::printInventory() const {
    std::cout << "Inventory:\n";
    for (size_t i = 0; i < inventory.size(); ++i) {
        if (inventory[i]) {
            std::cout << "Slot " << i << ": " << inventory[i]->name << "\n";
        } else {
            std::cout << "Slot " << i << ": Empty\n";
        }
    }
}
