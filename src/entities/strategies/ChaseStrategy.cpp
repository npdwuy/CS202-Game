#include "entities/strategies/ChaseStrategy.hpp"

#include <cmath>
#include <algorithm>

ChaseStrategy::ChaseStrategy(float leftBound, float rightBound, float aggroRadius)
    : m_leftBound(leftBound),
      m_rightBound(rightBound),
      m_aggroRadius(aggroRadius)
{}

void ChaseStrategy::setPlayerPosition(sf::Vector2f playerPos) {
    m_playerPos    = playerPos;
    m_hasPlayerPos = true;
}

void ChaseStrategy::Update(sf::Sprite& sprite, float speed, sf::Time timePerFrame) {
    const float dt  = timePerFrame.asSeconds();
    sf::Vector2f pos = sprite.getPosition();

    if (m_hasPlayerPos) {
        const float dx = m_playerPos.x - pos.x;

        if (std::abs(dx) <= m_aggroRadius) {
            // ── Chase mode: move toward the player, 30 % faster ──────────────
            const float chaseSpeed = speed * 1.3f;
            const float dir        = (dx > 0.f) ? 1.f : -1.f;

            sprite.move(dir * chaseSpeed * dt, 0.f);

            // Flip sprite horizontally so it faces the player
            const float absScaleX = std::abs(sprite.getScale().x);
            const float scaleY    = sprite.getScale().y;
            sprite.setScale(dir * absScaleX, scaleY);
            return;
        }
    }

    // ── Patrol mode: walk between leftBound and rightBound ──────────────────
    float newX = pos.x + static_cast<float>(m_direction) * speed * dt;

    if (newX <= m_leftBound) {
        m_direction = 1;
        newX        = m_leftBound;
    } else if (newX >= m_rightBound) {
        m_direction = -1;
        newX        = m_rightBound;
    }

    sprite.setPosition(newX, pos.y);

    // Flip to face walking direction
    const float absScaleX = std::abs(sprite.getScale().x);
    const float scaleY    = sprite.getScale().y;
    sprite.setScale(static_cast<float>(m_direction) * absScaleX, scaleY);
}
