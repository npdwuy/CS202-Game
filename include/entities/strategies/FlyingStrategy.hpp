#pragma once

#include "entities/strategies/MovementStrategy.hpp"

class FlyingStrategy : public MovementStrategy {
public:
    FlyingStrategy(
        float topBoundary,
        float bottomBoundary
    );

    ~FlyingStrategy() override = default;

    void Update(
        sf::Sprite& sprite,
        float speed,
        sf::Time timePerFrame
    ) override;

private:
    float m_topBoundary;
    float m_bottomBoundary;

    int m_direction;
};