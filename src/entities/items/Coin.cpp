#include "entities/items/Coin.hpp"

#include <cmath>
#include <stdexcept>

Coin::Coin(sf::Vector2f position, int value)
    : m_baseY(position.y),
      m_animationTime(0.f),
      m_value(value),
      m_collected(false)
{
    if (m_value <= 0)
    {
        m_value = 1;
    }

    if (!m_texture.loadFromFile("assets/sprites/items/coin.png"))
    {
        throw std::runtime_error("Failed to load Coin sprite.");
    }

    m_sprite.setTexture(m_texture);

    const float targetSize = 32.f;
    sf::Vector2u textureSize = m_texture.getSize();

    m_sprite.setScale(
        targetSize / static_cast<float>(textureSize.x),
        targetSize / static_cast<float>(textureSize.y)
    );

    m_sprite.setPosition(position);
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

    m_sprite.setPosition(
        m_sprite.getPosition().x,
        m_baseY + offsetY
    );
}

void Coin::Render(sf::RenderWindow& window) const
{
    if (!m_collected)
    {
        window.draw(m_sprite);
    }
}

sf::FloatRect Coin::GetBounds() const
{
    return m_sprite.getGlobalBounds();
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