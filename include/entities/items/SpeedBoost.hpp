#pragma once

#include "entities/items/FloatingItem.hpp"

class SpeedBoost : public FloatingItem {
public:
    explicit SpeedBoost(sf::Vector2f position);

    void Render(sf::RenderWindow& window) const override;
    sf::FloatRect GetBounds() const override;
    ItemEffect GetEffect() const override;

private:
    void SetVisualPosition(sf::Vector2f position) override;
    void Animate(sf::Time timePerFrame) override;

    sf::CircleShape m_badge;
    sf::ConvexShape m_arrow;
    float m_pulseTime = 0.f;
};
