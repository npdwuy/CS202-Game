#include "entities/items/Coin.hpp"
#include<cmath>

Coin::Coin(sf::Vector2f position, int value)
    : m_body(16.f),
      m_baseY(position.y),
      m_animationTime(0.f),
      m_value(value),
      m_collected(false)
{
    if (m_value <= 0)
    {
        m_value = 1;
    }

    m_body.setPosition(position);
    m_body.setFillColor(sf::Color::Yellow);
}

void Coin::Update(sf::Time timePerFrame)
{
    if (m_collected)
    {
        return;
    }

    m_animationTime += timePerFrame.asSeconds();

    const float amplitude = 5.f;
    const float frequency = 3.f;

    float offsetY = std::sin(m_animationTime * frequency) * amplitude;

    m_body.setPosition(
        m_body.getPosition().x,
        m_baseY + offsetY
    );
}

void Coin::Render(sf::RenderWindow& window) const
{
    if (!m_collected)
    {
        window.draw(m_body);
    }
}

sf::FloatRect Coin::GetBounds() const
{
    return m_body.getGlobalBounds();
}

void Coin::Collect()
{
    m_collected = true;
}

bool Coin::IsCollected() const
{
    return m_collected;
}

int Coin::GetValue() const
{
    return m_value;
}