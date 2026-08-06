#pragma once

#include "entities/items/FloatingItem.hpp"

class Mushroom : public FloatingItem {
public:
    explicit Mushroom(sf::Vector2f position);

    ~Mushroom() override = default;

    void Render(sf::RenderWindow& window) const override;

    sf::FloatRect GetBounds() const override;
    ItemEffect GetEffect() const override;

private:
    void SetVisualPosition(sf::Vector2f position) override;

    sf::Sprite m_sprite;
};
