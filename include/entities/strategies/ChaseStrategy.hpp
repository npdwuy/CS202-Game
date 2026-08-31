#pragma once

#include "entities/strategies/MovementStrategy.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Sprite.hpp>

class ChaseStrategy : public MovementStrategy {
public:
    // leftBound / rightBound : patrol range (world-space X)
    // aggroRadius            : distance in px to activate chase (default 280)
    ChaseStrategy(float leftBound, float rightBound, float aggroRadius = 280.f);

    ~ChaseStrategy() override = default;

    void Update(sf::Sprite& sprite, float speed, sf::Time timePerFrame) override;

    // Called every frame by the enemy; PlayState passes player position here.
    void setPlayerPosition(sf::Vector2f playerPos) override;

private:
    float m_leftBound;
    float m_rightBound;
    float m_aggroRadius;

    sf::Vector2f m_playerPos;
    bool  m_hasPlayerPos = false;
    int   m_direction    = -1;   // patrol direction: -1 = left, +1 = right
    int   m_patrolBouncesRemaining = 0; // Cooldown cycles after hitting bounds during chase
};
