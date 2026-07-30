#include "entities/items/FireFlower.hpp"

#include <cmath>
#include <stdexcept>

FireFlower::FireFlower(sf::Vector2f position)
    : m_animationTime(0.f),
      m_startY(position.y),
      m_collected(false)
{
    if (!m_texture.loadFromFile(
            "assets/sprites/items/fire_flower.png"))
    {
        throw std::runtime_error(
            "Failed to load FireFlower sprite."
        );
    }

    m_sprite.setTexture(m_texture);
    m_sprite.setPosition(position);
}

void FireFlower::Update(sf::Time timePerFrame)
{
    if (m_collected)
    {
        return;
    }

    m_animationTime += timePerFrame.asSeconds();

    const float floatingDistance = 7.f;
    const float floatingSpeed = 2.5f;

    float offsetY = std::sin(
        m_animationTime * floatingSpeed
    ) * floatingDistance;

    m_sprite.setPosition(
        m_sprite.getPosition().x,
        m_startY + offsetY
    );
}

void FireFlower::Render(sf::RenderWindow& window) const
{
    if (!m_collected)
    {
        window.draw(m_sprite);
    }
}

sf::FloatRect FireFlower::GetBounds() const
{
    return m_sprite.getGlobalBounds();
}

ItemEffect FireFlower::GetEffect() const
{
    return {
        ItemEffectType::EnableFirePower,
        1
    };
}

bool FireFlower::IsCollected() const
{
    return m_collected;
}

void FireFlower::Collect()
{
    m_collected = true;
}