#include "Inventory.h"
#include <iostream>

Inventory::Inventory(int inventorySize) : inventorySize(inventorySize), index(0) {
    inventory.resize(inventorySize, nullptr); // Initialize with empty slots
}

Inventory::Inventory() : inventorySize(10) {
    inventory.resize(inventorySize, nullptr);
}

Inventory::~Inventory() {
    // Clear items if inventory owns them
    for (auto item : inventory) {
        delete item; // Ensure safe deletion; skip nullptr
    }
    inventory.clear();
}

bool Inventory::addItem(Item* item) {
    if (!item) {
        std::cerr << "Cannot add a null item to the inventory.\n";
        return false;
    }

    for (size_t i = 0; i < inventory.size(); ++i) {
        if (inventory[i] == nullptr) {
            inventory[i] = new Item(*item); // Create a deep copy
            return true;
        }
    }

    std::cerr << "Inventory full! Cannot add item: " << item->name << ".\n";
    return false;
}

bool Inventory::removeItem(Item* item) {
    // Find the item in the inventory
    auto it = std::find(inventory.begin(), inventory.end(), item);
    if (it != inventory.end()) {
        *it = nullptr; // Mark slot as empty
        return true; // Successfully removed
    }
    std::cerr << "Item not found in inventory!\n";
    return false; // Item not found
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
