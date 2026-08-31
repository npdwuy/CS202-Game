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

// Helper: Safely flip the sprite without shifting its visual bounding box
static void setFacing(sf::Sprite& sprite, float dir) {
    const float absScale = std::abs(sprite.getScale().x);
    const float newScale = (dir < 0.f) ? absScale : -absScale; // facing left = positive scale, facing right = negative scale (because original sprite faces left)

    if (sprite.getScale().x != newScale) {
        // Record visual left edge before flipping
        const float currentLeft = sprite.getGlobalBounds().left;
        
        // Flip
        sprite.setScale(newScale, sprite.getScale().y);
        
        // Re-anchor to prevent position jumping
        // If scale is negative, the sprite's origin (top-left) is now physically on the right side of the visual bounds.
        if (newScale < 0.f) {
            sprite.setPosition(currentLeft + sprite.getGlobalBounds().width, sprite.getPosition().y);
        } else {
            sprite.setPosition(currentLeft, sprite.getPosition().y);
        }
    }
}

void ChaseStrategy::Update(sf::Sprite& sprite, float speed, sf::Time timePerFrame) {
    const float dt = timePerFrame.asSeconds();

    const float left  = sprite.getGlobalBounds().left;
    const float width = sprite.getGlobalBounds().width;

    if (m_hasPlayerPos) {
        const float spriteCentreX = left + width * 0.5f;
        const float dx = m_playerPos.x - spriteCentreX;

        if (std::abs(dx) <= m_aggroRadius) {
            // ── Chase mode: move toward player ────────────────────────────────
            const float dir = (dx > 0.f) ? 1.f : -1.f;
            sprite.move(dir * speed * 1.3f * dt, 0.f);
            
            // Xoay hướng
            setFacing(sprite, dir);
            
            // Giới hạn không cho quái vật đi xuyên tường/rơi khỏi platform bằng cách
            // ép nó phải ở trong m_leftBound và m_rightBound kể cả khi đang chase.
            const float newLeft  = sprite.getGlobalBounds().left;
            const float newWidth = sprite.getGlobalBounds().width;

            if (newLeft <= m_leftBound) {
                if (sprite.getScale().x < 0.f) {
                    sprite.setPosition(m_leftBound + newWidth, sprite.getPosition().y);
                } else {
                    sprite.setPosition(m_leftBound, sprite.getPosition().y);
                }
            } else if (newLeft + newWidth >= m_rightBound) {
                if (sprite.getScale().x < 0.f) {
                    sprite.setPosition(m_rightBound, sprite.getPosition().y);
                } else {
                    sprite.setPosition(m_rightBound - newWidth, sprite.getPosition().y);
                }
            }
            
            return;
        }
    }

    // ── Patrol mode: walk between leftBound and rightBound ───────────────────
    const float distance = static_cast<float>(m_direction) * speed * dt;
    sprite.move(distance, 0.f);
    
    // Xoay hướng tuần tra
    setFacing(sprite, static_cast<float>(m_direction));

    const float newLeft  = sprite.getGlobalBounds().left;
    const float newWidth = sprite.getGlobalBounds().width;

    if (newLeft <= m_leftBound) {
        // Clamp left edge
        if (sprite.getScale().x < 0.f) {
            sprite.setPosition(m_leftBound + newWidth, sprite.getPosition().y);
        } else {
            sprite.setPosition(m_leftBound, sprite.getPosition().y);
        }
        m_direction = 1;
    } else if (newLeft + newWidth >= m_rightBound) {
        // Clamp right edge
        if (sprite.getScale().x < 0.f) {
            sprite.setPosition(m_rightBound, sprite.getPosition().y);
        } else {
            sprite.setPosition(m_rightBound - newWidth, sprite.getPosition().y);
        }
        m_direction = -1;
    }
}
