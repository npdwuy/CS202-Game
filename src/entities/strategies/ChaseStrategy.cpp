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

    // ── Use getGlobalBounds().left as the LEFT edge of the sprite ────────────
    // IMPORTANT: never use negative scaleX to flip the sprite here.
    // A negative scaleX shifts the sprite's visual left edge away from
    // position.x, breaking all boundary math. We just move the sprite
    // forward/backward and let the sprite sheet face one way (same as the
    // original PatrolStrategy which also never flipped).
    const float left  = sprite.getGlobalBounds().left;
    const float width = sprite.getGlobalBounds().width;

    if (m_hasPlayerPos) {
        // Use the centre of the sprite as reference for aggro distance
        const float spriteCentreX = left + width * 0.5f;
        const float dx = m_playerPos.x - spriteCentreX;

        if (std::abs(dx) <= m_aggroRadius) {
            // ── Chase mode: move toward the player at 130 % normal speed ─────
            const float dir = (dx > 0.f) ? 1.f : -1.f;
            sprite.move(dir * speed * 1.3f * dt, 0.f);
            return;
        }
    }

    // ── Patrol mode: walk between leftBound and rightBound ───────────────────
    // Matches PatrolStrategy exactly: move first, then clamp.
    const float distance = static_cast<float>(m_direction) * speed * dt;
    sprite.move(distance, 0.f);

    const float newLeft  = sprite.getGlobalBounds().left;
    const float newWidth = sprite.getGlobalBounds().width;

    if (newLeft <= m_leftBound) {
        sprite.setPosition(m_leftBound, sprite.getPosition().y);
        m_direction = 1;
    } else if (newLeft + newWidth >= m_rightBound) {
        sprite.setPosition(m_rightBound - newWidth, sprite.getPosition().y);
        m_direction = -1;
    }
}
