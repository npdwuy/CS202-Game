#include "entities/items/FireFlower.hpp"
#include "resources/ResourceManager.hpp"

FireFlower::FireFlower(sf::Vector2f position)
    : FloatingItem(position, 7.f, 2.5f)
{
    m_sprite.setTexture(ResourceManager::getInstance().getTexture(
        "assets/sprites/items/fire_flower.png"
    ));
    m_sprite.setPosition(position);
}

void FireFlower::Render(sf::RenderWindow& window) const
{
    if (!IsCollected())
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

void FireFlower::SetVisualPosition(sf::Vector2f position) {
    m_sprite.setPosition(position);
}
