#pragma once

#include "entities/Enemy.hpp"
#include "entities/Item.hpp"

#include <memory>

class TileMap;

class LevelObjectFactory {
public:
    std::unique_ptr<Enemy> createEnemy(
        char symbol,
        sf::Vector2f position,
        float tileSize,
        float levelWidth,
        const TileMap& tileMap
    ) const;

    std::unique_ptr<Item> createItem(
        char symbol,
        sf::Vector2f position
    ) const;

    static bool isEnemySymbol(char symbol);
    static bool isItemSymbol(char symbol);
};
