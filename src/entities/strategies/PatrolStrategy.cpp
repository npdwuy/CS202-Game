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

    const float currentLeft = sprite.getGlobalBounds().left;
    const float spriteWidth = sprite.getGlobalBounds().width;

    if (currentLeft <= m_leftBoundary)
    {
        sprite.move(m_leftBoundary - currentLeft, 0.f);
        m_direction = 1;
    }
    else if (currentLeft + spriteWidth >= m_rightBoundary)
    {
        sprite.move(m_rightBoundary - (currentLeft + spriteWidth), 0.f);
        m_direction = -1;
    }
}