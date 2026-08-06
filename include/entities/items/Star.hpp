#pragma once

#include "entities/items/FloatingItem.hpp"

class Star : public FloatingItem {
public:
    explicit Star(sf::Vector2f position);

    void Render(sf::RenderWindow& window) const override;
    sf::FloatRect GetBounds() const override;
    ItemEffect GetEffect() const override;

private:
    void SetVisualPosition(sf::Vector2f position) override;
    void Animate(sf::Time timePerFrame) override;

    sf::ConvexShape m_star;
};
