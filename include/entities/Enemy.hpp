#pragma once

#include <SFML/Graphics.hpp>

class Enemy {
public:
    virtual ~Enemy() = default;

    virtual void Update(sf::Time timePerFrame) = 0;
    virtual void Render(sf::RenderWindow& window) const = 0;

    virtual sf::FloatRect GetBounds() const = 0;

    virtual bool IsActive() const = 0;
    virtual void Deactivate() = 0;

    virtual void TakeDamage() { Deactivate(); }
    virtual bool IsHurt() const { return false; }
    virtual bool IsBoss() const { return false; }

    virtual void Fling() {}
    virtual bool IsFlung() const { return false; }

    virtual void SetPlayerPosition(sf::Vector2f /*pos*/) {}
};