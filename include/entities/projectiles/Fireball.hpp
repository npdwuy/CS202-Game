#pragma once

#include "entities/player/Character.hpp"
#include <SFML/Graphics.hpp>

class Fireball : public Character {
public:
    static constexpr float FireballRadius = 12.f;
    static constexpr float FireballSpeed = 400.f;
    static constexpr float BounceSpeed = 300.f;
    static constexpr float FireballGravity = 1500.f;

    Fireball(sf::Vector2f position, int direction);

    std::string name() const override { return "Fireball"; }
    void update(sf::Time timePerFrame) override;
    void Render(sf::RenderWindow& window) const override;
    
    sf::FloatRect GetBounds() const {
        return {position_.x, position_.y, width_, height_};
    }

    bool IsDestroyed() const { return m_destroyed; }
    void Destroy() { m_destroyed = true; }

private:
    bool m_destroyed = false;
    sf::CircleShape m_shape;
};
