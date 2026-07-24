#pragma once

#include <SFML/Graphics.hpp>

class Enemy {
public:
    virtual ~Enemy() = default;

    virtual void Update(sf::Time timePerFrame) = 0;
    virtual void Render(sf::RenderWindow& window) const = 0;

    virtual sf::FloatRect GetBounds() const = 0;

    virtual bool IsActive() const = 0;
    virtual void Deactivate() = 0;
};