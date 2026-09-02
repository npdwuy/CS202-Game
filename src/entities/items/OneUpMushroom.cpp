#include "entities/items/OneUpMushroom.hpp"

#include "resources/ResourceManager.hpp"

OneUpMushroom::OneUpMushroom(sf::Vector2f position)
    : FloatingItem(position, 0.f, 0.f) {
    constexpr float TARGET_SPRITE_SIZE = 40.f;

    const sf::Texture& texture = ResourceManager::getInstance().getTexture(
        "assets/sprites/items/green_mushroom-removebg-preview.png"
    );
    m_sprite.setTexture(texture);

    const sf::Vector2u textureSize = texture.getSize();
    if (textureSize.x > 0U && textureSize.y > 0U) {
        const float scaleX = TARGET_SPRITE_SIZE / static_cast<float>(textureSize.x);
        const float scaleY = TARGET_SPRITE_SIZE / static_cast<float>(textureSize.y);
        m_sprite.setScale(scaleX, scaleY);
    }
    m_sprite.setPosition(position);
}

void OneUpMushroom::Render(sf::RenderWindow& window) const {
    if (!IsCollected()) {
        window.draw(m_sprite);
    }
}

sf::FloatRect OneUpMushroom::GetBounds() const {
    return m_sprite.getGlobalBounds();
}

ItemEffect OneUpMushroom::GetEffect() const {
    return {ItemEffectType::ExtraLife, 1};
}

void OneUpMushroom::SetVisualPosition(sf::Vector2f position) {
    m_sprite.setPosition(position);
}
