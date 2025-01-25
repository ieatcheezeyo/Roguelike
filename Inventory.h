#pragma once

#include <vector>
#include <algorithm>
#include "Item.h"

class Inventory {
public:
    Inventory(int inventorySize);
    ~Inventory();

    bool addItem(Item* item);
    bool removeItem(Item* item);

    void printInventory() const;

private:
    int inventorySize;
    std::vector<Item*> inventory;
};
