#pragma once

#include "entities/items/FloatingItem.hpp"

class FireFlower : public FloatingItem {
public:
    explicit FireFlower(sf::Vector2f position);

    ~FireFlower() override = default;

    void Render(sf::RenderWindow& window) const override;

    sf::FloatRect GetBounds() const override;
    ItemEffect GetEffect() const override;

private:
    void SetVisualPosition(sf::Vector2f position) override;

    sf::Sprite m_sprite;
};
