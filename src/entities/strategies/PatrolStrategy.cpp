#include "entities/strategies/PatrolStrategy.hpp"

#include <utility>

PatrolStrategy::PatrolStrategy(
    float leftBoundary,
    float rightBoundary
)
    : m_leftBoundary(leftBoundary),
      m_rightBoundary(rightBoundary),
      m_direction(1)
{
    if (m_leftBoundary > m_rightBoundary)
    {
        std::swap(m_leftBoundary, m_rightBoundary);
    }
}

void PatrolStrategy::Update(
    sf::Sprite& sprite,
    float speed,
    sf::Time timePerFrame
)
{
    float distance = speed
                   * static_cast<float>(m_direction)
                   * timePerFrame.asSeconds();

    sprite.move(distance, 0.f);

    float currentX = sprite.getPosition().x;
    float spriteWidth = sprite.getGlobalBounds().width;

    if (currentX <= m_leftBoundary)
    {
        sprite.setPosition(
            m_leftBoundary,
            sprite.getPosition().y
        );

        m_direction = 1;
    }
    else if (currentX + spriteWidth >= m_rightBoundary)
    {
        sprite.setPosition(
            m_rightBoundary - spriteWidth,
            sprite.getPosition().y
        );

        m_direction = -1;
    }
}