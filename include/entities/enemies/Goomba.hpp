#pragma once

#include "entities/Enemy.hpp"
#include "entities/strategies/MovementStrategy.hpp"

#include <memory>

class Goomba : public Enemy {
public:
    Goomba(
        sf::Vector2f position,
        float speed,
        std::unique_ptr<MovementStrategy> movementStrategy
    );

    ~Goomba() override = default;

    void Update(sf::Time timePerFrame) override;
    void Render(sf::RenderWindow& window) const override;

    sf::FloatRect GetBounds() const override;

    bool IsActive() const override;
    void Deactivate() override;
    
    void Fling() override;
    bool IsFlung() const override;

    void SetPlayerPosition(sf::Vector2f pos) override;

private:
    sf::Sprite m_sprite;

    std::unique_ptr<MovementStrategy> m_movementStrategy;

    float m_speed;
    float m_animationTime;

    int m_currentFrame;
    bool m_active;
    bool m_flung = false;
    sf::Vector2f m_velocity;
};
