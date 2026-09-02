#pragma once

#include "entities/Enemy.hpp"
#include "entities/strategies/MovementStrategy.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

enum class HammerBroState {
    Walk,
    PrepareAttack,
    Attack,
    Dead
};

class HammerBro : public Enemy {
public:
    HammerBro(
        sf::Vector2f position,
        float speed,
        std::unique_ptr<MovementStrategy> movementStrategy,
        bool isBig = false
    );

    ~HammerBro() override = default;

    void Update(sf::Time dt) override;
    void Render(sf::RenderWindow& window) const override;
    sf::FloatRect GetBounds() const override;
    bool IsActive() const override;
    void Deactivate() override;

    void Fling() override;
    bool IsFlung() const override;

    void TakeDamage();
    bool IsBig() const { return m_isBig; }

    void SetPlayerPosition(sf::Vector2f playerPos) override;

private:
    void FireProjectile();

    sf::Sprite m_sprite;
    std::unique_ptr<MovementStrategy> m_movementStrategy;
    sf::Vector2f m_playerPos;
    sf::Vector2f m_velocity;

    float m_speed;
    bool m_isBig = false;
    HammerBroState m_state = HammerBroState::Walk;
    int m_health = 1;
    bool m_active = true;
    bool m_flung = false;

    float m_attackCooldown = 0.f;
    float m_stateTimer = 0.f;
    float m_animationTimer = 0.f;
    int m_currentFrame = 0;
};
