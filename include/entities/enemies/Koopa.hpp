#pragma once

#include "entities/Enemy.hpp"
#include "entities/strategies/MovementStrategy.hpp"

#include <memory>

class Koopa : public Enemy {
public:
    bool CanKickShell() const;
    enum class State
    {
        Walking,
        ShellIdle,
        ShellMoving
    };

    Koopa(
        sf::Vector2f position,
        float speed,
        std::unique_ptr<MovementStrategy> movementStrategy
    );

    ~Koopa() override = default;

    void Update(sf::Time timePerFrame) override;
    void Render(sf::RenderWindow& window) const override;

    sf::FloatRect GetBounds() const override;

    bool IsActive() const override;
    void Deactivate() override;

    void Fling() override;
    bool IsFlung() const override;

    void EnterShell();
    void KickShell(int direction);

    State GetState() const;
    bool IsWalking() const;
    bool IsShellIdle() const;
    bool IsShellMoving() const;

private:
    sf::Sprite m_sprite;

    std::unique_ptr<MovementStrategy> m_movementStrategy;

    float m_speed;
    float m_animationTime;

    int m_currentFrame;
    bool m_active;

    State m_state = State::Walking;

    float m_shellSpeed = 450.f;
    int m_shellDirection = 1;

    bool m_flung = false;
    sf::Vector2f m_velocity;
    float m_shellKickDelay = 0.f;
};