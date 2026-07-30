#include "entities/items/Mushroom.hpp"

#include <cmath>
#include <stdexcept>

Mushroom::Mushroom(sf::Vector2f position)
    : m_animationTime(0.f),
      m_startY(position.y),
      m_collected(false)
{
    if (!m_texture.loadFromFile(
            "assets/sprites/items/mushroom.png"))
    {
        throw std::runtime_error(
            "Failed to load Mushroom sprite."
        );
    }

    m_sprite.setTexture(m_texture);
    m_sprite.setPosition(position);
}

void Mushroom::Update(sf::Time timePerFrame)
{
    if (m_collected)
    {
        return;
    }

    m_animationTime += timePerFrame.asSeconds();

    const float floatingDistance = 6.f;
    const float floatingSpeed = 2.f;

    float offsetY = std::sin(
        m_animationTime * floatingSpeed
    ) * floatingDistance;

    m_sprite.setPosition(
        m_sprite.getPosition().x,
        m_startY + offsetY
    );
}

void Mushroom::Render(sf::RenderWindow& window) const
{
    if (!m_collected)
    {
        window.draw(m_sprite);
    }
}

sf::FloatRect Mushroom::GetBounds() const
{
    return m_sprite.getGlobalBounds();
}

ItemEffect Mushroom::GetEffect() const
{
    return {
        ItemEffectType::GrowPlayer,
        1
    };
}

bool Mushroom::IsCollected() const
{
    return m_collected;
}

void Mushroom::Collect()
{
    m_collected = true;
}