#include "entities/projectiles/Fireball.hpp"

Fireball::Fireball(sf::Vector2f position, int direction)
    : Character(position, FireballRadius * 2.f, FireballRadius * 2.f)
{
    setFacing(direction);
    velocity_.x = direction * FireballSpeed;
    velocity_.y = 0.f; // Initial y velocity

    m_shape.setRadius(FireballRadius);
    m_shape.setFillColor(sf::Color(255, 100, 0)); // Bright orange/red
    m_shape.setOutlineColor(sf::Color(255, 200, 0)); // Yellow outline
    m_shape.setOutlineThickness(1.f);
}

void Fireball::update(sf::Time timePerFrame) {
    if (m_destroyed) return;

    // Apply gravity
    velocity_.y += FireballGravity * timePerFrame.asSeconds();

    // Move and resolve collisions via Character's moveCharacter
    moveCharacter(timePerFrame);

    // Update shape position (center on Character bounds)
    m_shape.setPosition(position_.x, position_.y);

    // Zig-zag / Bouncing logic
    if (onGround()) {
        velocity_.y = -BounceSpeed;
        setOnGround(false); // Pop it off the ground so it moves up next frame
    }

    // Destroy if it hit a wall (horizontal velocity became 0)
    if (velocity_.x == 0.f) {
        Destroy();
    }
}

void Fireball::Render(sf::RenderWindow& window) const {
    if (!m_destroyed) {
        window.draw(m_shape);
    }
}
