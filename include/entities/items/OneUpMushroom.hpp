#pragma once

#include "entities/items/FloatingItem.hpp"

class OneUpMushroom : public FloatingItem {
public:
    explicit OneUpMushroom(sf::Vector2f position);

    void Render(sf::RenderWindow& window) const override;
    sf::FloatRect GetBounds() const override;
    ItemEffect GetEffect() const override;

private:
    void SetVisualPosition(sf::Vector2f position) override;

    sf::Sprite m_sprite;
};
