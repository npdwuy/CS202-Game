#include "entities/strategies/BossChaseStrategy.hpp"

#include <algorithm>
#include <cmath>

BossChaseStrategy::BossChaseStrategy(float leftBound, float rightBound)
    : m_leftBound(std::min(leftBound, rightBound)),
      m_rightBound(std::max(leftBound, rightBound))
{}

void BossChaseStrategy::setPlayerPosition(sf::Vector2f playerPos) {
    m_playerPos = playerPos;
}

void BossChaseStrategy::Update(
    sf::Sprite& sprite,
    float speed,
    sf::Time timePerFrame)
{
    const float delta = timePerFrame.asSeconds();
    const float distanceToPlayer = m_playerPos.x - sprite.getPosition().x;
    const float moveStep = speed * delta;

    if (distanceToPlayer > CHASE_DEADZONE) {
        sprite.move(moveStep, 0.f);
        if (m_facingLeft) {
            m_facingLeft = false;
        }
    } else if (distanceToPlayer < -CHASE_DEADZONE) {
        sprite.move(-moveStep, 0.f);
        if (!m_facingLeft) {
            m_facingLeft = true;
        }
    }

    const float absScale = std::abs(sprite.getScale().x);
    sprite.setScale(m_facingLeft ? absScale : -absScale, absScale);

    sf::FloatRect bounds = sprite.getGlobalBounds();

    if (bounds.left < m_leftBound) {
        sprite.move(m_leftBound - bounds.left, 0.f);
    } else if (bounds.left + bounds.width > m_rightBound) {
        sprite.move(m_rightBound - (bounds.left + bounds.width), 0.f);
    }
}
