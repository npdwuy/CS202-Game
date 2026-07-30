#pragma once

#include "entities/Item.hpp"

class FireFlower : public Item {
public:
    explicit FireFlower(sf::Vector2f position);

    ~FireFlower() override = default;

    void Update(sf::Time timePerFrame) override;
    void Render(sf::RenderWindow& window) const override;

    sf::FloatRect GetBounds() const override;
    ItemEffect GetEffect() const override;

    bool IsCollected() const override;
    void Collect() override;

private:
    sf::Texture m_texture;
    sf::Sprite m_sprite;

    float m_animationTime;
    float m_startY;

    bool m_collected;
};