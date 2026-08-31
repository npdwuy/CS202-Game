#pragma once

#include "entities/Enemy.hpp"
#include "entities/strategies/MovementStrategy.hpp"

#include <memory>

class TileMap;

class Koopa : public Enemy {
public:
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

    void SetPlayerPosition(sf::Vector2f pos) override;

    // =========================================================
    // SHELL STATE
    // =========================================================
    void EnterShell();
    void KickShell(int direction);

    State GetState() const;

    bool IsWalking() const;
    bool IsShellIdle() const;
    bool IsShellMoving() const;

    bool CanKickShell() const;
    bool IsSafeFromPlayer() const;

    // Handles gravity, falling, landing,
    // and horizontal terrain collision for the shell.
    void UpdateShellPhysics(
        sf::Time timePerFrame,
        const TileMap& tileMap
    );

private:
    sf::Sprite m_sprite;

    std::unique_ptr<MovementStrategy> m_movementStrategy;

    float m_speed;
    float m_animationTime;

    int m_currentFrame;
    bool m_active;

    // =========================================================
    // KOOPA STATE
    // =========================================================
    State m_state = State::Walking;

    // =========================================================
    // SHELL MOVEMENT
    // =========================================================
    float m_shellSpeed = 450.f;

    int m_shellDirection = 1;

    float m_shellVerticalVelocity = 0.f;

    // Prevent the same stomp from immediately
    // kicking the shell after EnterShell().
    float m_shellKickDelay = 0.f;

    // =========================================================
    // FLING STATE
    // =========================================================
    bool m_flung = false;

    sf::Vector2f m_velocity;
};
