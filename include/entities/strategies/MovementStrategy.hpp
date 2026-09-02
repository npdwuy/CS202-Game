#pragma once

#include <SFML/Graphics.hpp>

class MovementStrategy {
public:
    virtual ~MovementStrategy() = default;

    virtual void Update(
        sf::Sprite& sprite,
        float speed,
        sf::Time timePerFrame
    ) = 0;

    // Override in strategies that react to the player's world position.
    virtual void setPlayerPosition(sf::Vector2f /*pos*/) {}
};