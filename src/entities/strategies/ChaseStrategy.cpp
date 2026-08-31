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
    const float dt = timePerFrame.asSeconds();

    // BUG FIX 2: read the sprite's original absolute scale once per frame.
    // We only flip the *sign* of scaleX; the magnitude must stay unchanged.
    // (Goomba is 3.f, Koopa is 1.f – we must not overwrite that.)
    const float absScaleX = std::abs(sprite.getScale().x);
    const float scaleY    = sprite.getScale().y;

    if (m_hasPlayerPos) {
        const float dx = m_playerPos.x - sprite.getPosition().x;

        if (std::abs(dx) <= m_aggroRadius) {
            // ── Chase mode: move toward the player at 130 % speed ────────────
            const float dir = (dx > 0.f) ? 1.f : -1.f;
            sprite.move(dir * speed * 1.3f * dt, 0.f);

            // Flip sprite to face the player (preserve original scale magnitude)
            sprite.setScale(dir * absScaleX, scaleY);
            return;
        }
    }

    // ── Patrol mode: walk between leftBound and rightBound ───────────────────
    // BUG FIX 1: use sprite.getPosition().x (top-left) AND account for
    // spriteWidth when testing the right boundary, matching PatrolStrategy.
    float newX = sprite.getPosition().x
               + static_cast<float>(m_direction) * speed * dt;

    const float spriteWidth = sprite.getGlobalBounds().width;

    if (newX <= m_leftBound) {
        m_direction = 1;
        newX        = m_leftBound;
    } else if (newX + spriteWidth >= m_rightBound) {
        m_direction = -1;
        newX        = m_rightBound - spriteWidth;
    }

    sprite.setPosition(newX, sprite.getPosition().y);

    // Flip to face the walking direction (preserve original scale magnitude)
    sprite.setScale(static_cast<float>(m_direction) * absScaleX, scaleY);
}
