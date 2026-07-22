#pragma once

#include "entities/Enemy.hpp"

class Goomba : public Enemy {
public:
    Goomba(sf::Vector2f position,
           float speed,
           float leftBoundary,
           float rightBoundary);

    ~Goomba() override = default;

    void Update(sf::Time timePerFrame) override;
    void Render(sf::RenderWindow& window) const override;

    sf::FloatRect GetBounds() const override;

    bool IsActive() const override;
    void Deactivate() override;

private:
    sf::Texture m_texture;
    sf::Sprite m_sprite;

    float m_speed;
    float m_leftBoundary;
    float m_rightBoundary;

    int m_direction;
    bool m_active;
};