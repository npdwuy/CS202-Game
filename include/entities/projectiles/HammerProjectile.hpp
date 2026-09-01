#pragma once

#include <SFML/Graphics.hpp>

class HammerProjectile {
public:
    HammerProjectile(sf::Vector2f position, sf::Vector2f velocity);

    void Update(sf::Time dt);
    void Render(sf::RenderWindow& window);
    sf::FloatRect GetBounds() const;

    bool IsDestroyed() const { return m_destroyed; }
    void Destroy() { m_destroyed = true; }

private:
    sf::Sprite m_sprite;
    sf::Vector2f m_velocity;
    sf::Vector2f m_position;
    float m_animationTimer = 0.f;
    int m_currentFrame = 0;
    bool m_destroyed = false;
};
