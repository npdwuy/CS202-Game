#pragma once

#include "entities/Enemy.hpp"
#include "entities/strategies/MovementStrategy.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

enum class BossState {
    Walk,
    PrepareAttack,
    Attack,
    Hurt,
    Dead
};

class BossEnemy : public Enemy {
public:
    BossEnemy(
        sf::Vector2f position,
        float speed,
        std::unique_ptr<MovementStrategy> movementStrategy
    );

    void Update(sf::Time dt) override;
    void Render(sf::RenderWindow& window) const override;
    sf::FloatRect GetBounds() const override;
    bool IsActive() const override;
    void Deactivate() override;

    void TakeDamage() override;
    bool IsHurt() const override;
    bool IsBoss() const override;

    void Fling() override;
    bool IsFlung() const override;

    void SetPlayerPosition(sf::Vector2f playerPos) override;

private:
    void FireProjectile();

    sf::Sprite m_sprite;
    std::unique_ptr<MovementStrategy> m_movementStrategy;
    sf::Vector2f m_playerPos;

    float m_speed;
    float m_spriteScale = 1.44f;

    BossState m_state = BossState::Walk;
    int m_health = 5;
    bool m_active = true;
    bool m_flung = false;

    float m_stateTimer = 0.f;
    float m_animationTimer = 0.f;
    int m_currentFrame = 0;

    static constexpr float FRAME_WIDTH = 85.f;
    static constexpr float FRAME_HEIGHT = 65.f;
    static constexpr float ATTACK_RANGE_X = 450.f;
    static constexpr float ATTACK_RANGE_Y = 250.f;
    static constexpr float WALK_COOLDOWN = 2.5f;
    static constexpr float WALK_RESET = 1.5f;
    static constexpr float HURT_DURATION = 0.8f;
    static constexpr float HURT_FLASH_INTERVAL = 0.08f;
    static constexpr int DEFEAT_SCORE = 500;
};