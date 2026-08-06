#include "entities/items/Coin.hpp"
#include "resources/ResourceManager.hpp"

Coin::Coin(sf::Vector2f position, int value)
    : FloatingItem(position, 5.f, 3.f),
      m_value(value)
{
    if (m_value <= 0)
    {
        m_value = 1;
    }

    const sf::Texture& texture = ResourceManager::getInstance().getTexture(
        "assets/sprites/items/coin.png"
    );
    m_sprite.setTexture(texture);

    const float targetSize = 32.f;
    const sf::Vector2u textureSize = texture.getSize();

    m_sprite.setScale(
        targetSize / static_cast<float>(textureSize.x),
        targetSize / static_cast<float>(textureSize.y)
    );

    m_sprite.setPosition(position);
}

void Coin::Render(sf::RenderWindow& window) const
{
    if (!IsCollected())
    {
        window.draw(m_sprite);
    }
}

sf::FloatRect Coin::GetBounds() const
{
    return m_sprite.getGlobalBounds();
}

ItemEffect Coin::GetEffect() const
{
    return {
        ItemEffectType::AddScore,
        m_value
    };
}

int Coin::GetValue() const
{
    return m_value;
}

void Coin::SetVisualPosition(sf::Vector2f position) {
    m_sprite.setPosition(position);
}
