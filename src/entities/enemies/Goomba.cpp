#include "entities/enemies/Goomba.hpp"

Goomba::Goomba(sf::Vector2f position,
               float speed,
               float leftBoundary,
               float rightBoundary)
    : m_body(sf::Vector2f(48.f, 48.f)),
      m_speed(speed),
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

    m_body.setPosition(position);
    m_body.setFillColor(sf::Color(139, 69, 19));
}

void Goomba::Update(sf::Time timePerFrame)
{
    (void)timePerFrame;
}

void Goomba::Render(sf::RenderWindow& window) const
{
    if (m_active)
    {
        window.draw(m_body);
    }
}

sf::FloatRect Goomba::GetBounds() const
{
    return m_body.getGlobalBounds();
}

bool Goomba::IsActive() const
{
    return m_active;
}

void Goomba::Deactivate()
{
    m_active = false;
}