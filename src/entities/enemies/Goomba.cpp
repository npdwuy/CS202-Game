#include "entities/enemies/Goomba.hpp"

#include <stdexcept>

Goomba::Goomba(sf::Vector2f position,
               float speed,
               float leftBoundary,
               float rightBoundary)
    : m_speed(speed),
      m_leftBoundary(leftBoundary),
      m_rightBoundary(rightBoundary),
      m_direction(1),
      m_active(true)
{
    if (m_leftBoundary > m_rightBoundary)
    {
        float temp = m_leftBoundary;
        m_leftBoundary = m_rightBoundary;
        m_rightBoundary = temp;
    }

    if (!m_texture.loadFromFile(
            "assets/sprites/enemies/goomba_walk.png"))
    {
        throw std::runtime_error(
            "Failed to load Goomba sprite."
        );
    }

    m_sprite.setTexture(m_texture);

    // Use the first 16 x 16 frame.
    m_sprite.setTextureRect(sf::IntRect(0, 0, 16, 16));

    // Scale the original sprite to 48 x 48 pixels.
    m_sprite.setScale(3.f, 3.f);

    m_sprite.setPosition(position);
}

void Goomba::Update(sf::Time timePerFrame)
{
    if (!m_active)
    {
        return;
    }

    float distance = m_speed
                   * static_cast<float>(m_direction)
                   * timePerFrame.asSeconds();

    m_sprite.move(distance, 0.f);

    float currentX = m_sprite.getPosition().x;
    float spriteWidth = m_sprite.getGlobalBounds().width;

    if (currentX <= m_leftBoundary)
    {
        m_sprite.setPosition(
            m_leftBoundary,
            m_sprite.getPosition().y
        );

        m_direction = 1;
    }
    else if (currentX + spriteWidth >= m_rightBoundary)
    {
        m_sprite.setPosition(
            m_rightBoundary - spriteWidth,
            m_sprite.getPosition().y
        );

        m_direction = -1;
    }
}

void Goomba::Render(sf::RenderWindow& window) const
{
    if (m_active)
    {
        window.draw(m_sprite);
    }
}

sf::FloatRect Goomba::GetBounds() const
{
    return m_sprite.getGlobalBounds();
}

bool Goomba::IsActive() const
{
    return m_active;
}

void Goomba::Deactivate()
{
    m_active = false;
}