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
};