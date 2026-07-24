#include "entities/strategies/FlyingStrategy.hpp"

#include <utility>

FlyingStrategy::FlyingStrategy(
    float topBoundary,
    float bottomBoundary
)
    : m_topBoundary(topBoundary),
      m_bottomBoundary(bottomBoundary),
      m_direction(1)
{
    if (m_topBoundary > m_bottomBoundary)
    {
        std::swap(m_topBoundary, m_bottomBoundary);
    }
}

void FlyingStrategy::Update(
    sf::Sprite& sprite,
    float speed,
    sf::Time timePerFrame
)
{
    float distance = speed
                   * static_cast<float>(m_direction)
                   * timePerFrame.asSeconds();

    sprite.move(0.f, distance);

    float currentY = sprite.getPosition().y;
    float spriteHeight = sprite.getGlobalBounds().height;

    if (currentY <= m_topBoundary)
    {
        sprite.setPosition(
            sprite.getPosition().x,
            m_topBoundary
        );

        m_direction = 1;
    }
    else if (currentY + spriteHeight >= m_bottomBoundary)
    {
        sprite.setPosition(
            sprite.getPosition().x,
            m_bottomBoundary - spriteHeight
        );

        m_direction = -1;
    }
}