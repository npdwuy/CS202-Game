#include "entities/strategies/ChaseStrategy.hpp"

#include <cmath>
#include <algorithm>

ChaseStrategy::ChaseStrategy(float leftBound, float rightBound, float aggroRadius, bool baseFacingLeft)
    : m_leftBound(leftBound),
      m_rightBound(rightBound),
      m_aggroRadius(aggroRadius),
      m_baseFacingLeft(baseFacingLeft)
{}

void ChaseStrategy::setPlayerPosition(sf::Vector2f playerPos) {
    m_playerPos    = playerPos;
    m_hasPlayerPos = true;
}

// Helper: Safely flip the sprite without shifting its visual bounding box
static void setFacing(sf::Sprite& sprite, float dir, bool baseFacingLeft) {
    const float absScale = std::abs(sprite.getScale().x);
    // Nếu baseFacingLeft = true (Goomba): sprite gốc quay mặt sang TRÁI.
    //   -> Hướng < 0 (trái) -> scale dương (+absScale).
    //   -> Hướng > 0 (phải) -> scale âm (-absScale).
    // Nếu baseFacingLeft = false (Koopa): sprite gốc quay mặt sang PHẢI.
    //   -> Hướng > 0 (phải) -> scale dương (+absScale).
    //   -> Hướng < 0 (trái) -> scale âm (-absScale).
    const bool faceSameAsBase = baseFacingLeft ? (dir < 0.f) : (dir > 0.f);
    const float newScale = faceSameAsBase ? absScale : -absScale;

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
    const float spriteCentreX = left + width * 0.5f;

    // Khởi tạo vị trí đổi hướng lần đầu
    if (m_lastDirChangeX == -9999.f) {
        m_lastDirChangeX = spriteCentreX;
    }

    // Check for chase mode ONLY if we are not in cooldown
    if (m_hasPlayerPos && m_patrolBouncesRemaining <= 0) {
        const float dx = m_playerPos.x - spriteCentreX;

        if (std::abs(dx) <= m_aggroRadius) {
            // Chase mode: move toward player
            const float targetDir = (dx > 0.f) ? 1.f : -1.f;
            
            // Giới hạn: phải di chuyển ít nhất 2 blocks (96 pixels) mới được đổi hướng lại,
            // trừ khi đụng tường thì bắt buộc phải đổi hướng (xử lý ở dưới).
            if (targetDir != static_cast<float>(m_direction)) {
                if (std::abs(spriteCentreX - m_lastDirChangeX) >= 96.f) {
                    m_direction = static_cast<int>(targetDir);
                    m_lastDirChangeX = spriteCentreX;
                }
            }

            sprite.move(static_cast<float>(m_direction) * speed * 1.3f * dt, 0.f);
            
            // Xoay hướng
            setFacing(sprite, static_cast<float>(m_direction), m_baseFacingLeft);
            
            // Giới hạn không cho quái vật đi xuyên tường/rơi khỏi platform
            const float newLeft  = sprite.getGlobalBounds().left;
            const float newWidth = sprite.getGlobalBounds().width;

            bool hitBound = false;
            if (newLeft <= m_leftBound) {
                if (sprite.getScale().x < 0.f) {
                    sprite.setPosition(m_leftBound + newWidth, sprite.getPosition().y);
                } else {
                    sprite.setPosition(m_leftBound, sprite.getPosition().y);
                }
                hitBound = true;
                m_direction = 1;
                m_lastDirChangeX = m_leftBound + newWidth * 0.5f; // Cập nhật gốc đổi hướng
            } else if (newLeft + newWidth >= m_rightBound) {
                if (sprite.getScale().x < 0.f) {
                    sprite.setPosition(m_rightBound, sprite.getPosition().y);
                } else {
                    sprite.setPosition(m_rightBound - newWidth, sprite.getPosition().y);
                }
                hitBound = true;
                m_direction = -1;
                m_lastDirChangeX = m_rightBound - newWidth * 0.5f; // Cập nhật gốc đổi hướng
            }
            
            if (hitBound) {
                // Tắt đi theo Mario tối thiểu 1 chu kỳ (2 lần chạm biên)
                m_patrolBouncesRemaining = 2;
                setFacing(sprite, static_cast<float>(m_direction), m_baseFacingLeft);
            }
            
            return;
        }
    }

    // Patrol mode: walk between leftBound and rightBound
    const float distance = static_cast<float>(m_direction) * speed * dt;
    sprite.move(distance, 0.f);
    
    // Xoay hướng tuần tra
    setFacing(sprite, static_cast<float>(m_direction), m_baseFacingLeft);

    const float newLeft  = sprite.getGlobalBounds().left;
    const float newWidth = sprite.getGlobalBounds().width;

    bool hitBound = false;
    if (newLeft <= m_leftBound) {
        // Clamp left edge
        if (sprite.getScale().x < 0.f) {
            sprite.setPosition(m_leftBound + newWidth, sprite.getPosition().y);
        } else {
            sprite.setPosition(m_leftBound, sprite.getPosition().y);
        }
        m_direction = 1;
        m_lastDirChangeX = m_leftBound + newWidth * 0.5f;
        hitBound = true;
    } else if (newLeft + newWidth >= m_rightBound) {
        // Clamp right edge
        if (sprite.getScale().x < 0.f) {
            sprite.setPosition(m_rightBound, sprite.getPosition().y);
        } else {
            sprite.setPosition(m_rightBound - newWidth, sprite.getPosition().y);
        }
        m_direction = -1;
        m_lastDirChangeX = m_rightBound - newWidth * 0.5f;
        hitBound = true;
    }

    if (hitBound && m_patrolBouncesRemaining > 0) {
        m_patrolBouncesRemaining--;
    }
}
