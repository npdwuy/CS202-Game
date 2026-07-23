#pragma once

#include "entities/items/ItemEffect.hpp"

#include <SFML/Graphics.hpp>

class Item {
public:
    virtual ~Item() = default;

    virtual void Update(sf::Time timePerFrame) = 0;
    virtual void Render(sf::RenderWindow& window) const = 0;

    virtual sf::FloatRect GetBounds() const = 0;
    virtual ItemEffect GetEffect() const = 0;

    virtual bool IsCollected() const = 0;
    virtual void Collect() = 0;
};