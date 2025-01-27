#pragma once

#include <vector>
#include <algorithm>

#include "Item.h"

class Inventory {
public:
    Inventory();
    Inventory(int inventorySize);
    ~Inventory();

    bool addItem(Item* item);
    bool removeItem(Item* item);

    void printInventory() const;

    int inventorySize;
    int index;
    std::vector<Item*> inventory;
};
