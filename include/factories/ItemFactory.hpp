#pragma once

#include "entities/Item.hpp"

#include <SFML/Graphics.hpp>

#include <memory>

class ItemFactory {
public:
    static std::unique_ptr<Item> Create(
        char symbol,
        sf::Vector2f position
    );
};