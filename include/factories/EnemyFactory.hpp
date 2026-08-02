#pragma once

#include "entities/Enemy.hpp"

#include <SFML/Graphics.hpp>

#include <memory>

class EnemyFactory {
public:
    static std::unique_ptr<Enemy> Create(
        char symbol,
        sf::Vector2f position,
        float tileSize,
        float levelWidth
    );
};