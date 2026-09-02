#pragma once

#include "entities/strategies/MovementStrategy.hpp"

class BossChaseStrategy : public MovementStrategy {
public:
    BossChaseStrategy(float leftBound, float rightBound);

    void Update(sf::Sprite& sprite, float speed, sf::Time timePerFrame) override;
    void setPlayerPosition(sf::Vector2f playerPos) override;

private:
    float m_leftBound;
    float m_rightBound;
    sf::Vector2f m_playerPos;
    bool m_facingLeft = true;

    static constexpr float CHASE_DEADZONE = 5.f;
};
