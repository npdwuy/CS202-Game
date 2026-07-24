#pragma once

#include "entities/strategies/MovementStrategy.hpp"

class PatrolStrategy : public MovementStrategy {
public:
    PatrolStrategy(float leftBoundary, float rightBoundary);

    ~PatrolStrategy() override = default;

    void Update(
        sf::Sprite& sprite,
        float speed,
        sf::Time timePerFrame
    ) override;

private:
    float m_leftBoundary;
    float m_rightBoundary;

    int m_direction;
};