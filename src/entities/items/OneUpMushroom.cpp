#include "entities/items/OneUpMushroom.hpp"

#include "resources/ResourceManager.hpp"

OneUpMushroom::OneUpMushroom(sf::Vector2f position)
    : FloatingItem(position, 0.f, 0.f) {
    const sf::Texture& texture = ResourceManager::getInstance().getTexture(
        "assets/sprites/items/mushroom.png"
    );
    m_sprite.setTexture(texture);
    m_sprite.setColor(sf::Color(90, 255, 120));

    const sf::Vector2u size = texture.getSize();
    if (size.x > 0U && size.y > 0U) {
        m_sprite.setScale(
            40.f / static_cast<float>(size.x),
            40.f / static_cast<float>(size.y)
        );
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
