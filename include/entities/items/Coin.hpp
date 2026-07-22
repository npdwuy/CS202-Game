#pragma once

#include "entities/Item.hpp"

class Coin : public Item {
public:
    Coin(sf::Vector2f position, int value);

    ~Coin() override = default;

    void Update(sf::Time timePerFrame) override;
    void Render(sf::RenderWindow& window) const override;

    sf::FloatRect GetBounds() const override;

    void Collect() override;
    bool IsCollected() const override;

    int GetValue() const;

private:
    sf::Texture m_texture;
    sf::Sprite m_sprite;

    float m_baseY;
    float m_animationTime;

    int m_value;
    bool m_collected;
};