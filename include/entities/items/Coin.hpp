#pragma once

#include "entities/items/FloatingItem.hpp"

class Coin : public FloatingItem {
public:
    Coin(sf::Vector2f position, int value);

    ~Coin() override = default;

    void Render(sf::RenderWindow& window) const override;

    sf::FloatRect GetBounds() const override;
    ItemEffect GetEffect() const override;

    int GetValue() const;

private:
    void SetVisualPosition(sf::Vector2f position) override;

    sf::Sprite m_sprite;

    int m_value;
};
