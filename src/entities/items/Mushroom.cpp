#include "entities/items/Mushroom.hpp"
#include "resources/ResourceManager.hpp"

Mushroom::Mushroom(sf::Vector2f position)
    : FloatingItem(position, 6.f, 2.f)
{
    m_sprite.setTexture(ResourceManager::getInstance().getTexture(
        "assets/sprites/items/mushroom.png"
    ));
    m_sprite.setPosition(position);
}

void Mushroom::Render(sf::RenderWindow& window) const
{
    if (!IsCollected())
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

void Mushroom::SetVisualPosition(sf::Vector2f position) {
    m_sprite.setPosition(position);
}
