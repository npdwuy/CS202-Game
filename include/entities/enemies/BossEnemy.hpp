#pragma once

#include "entities/Enemy.hpp"
#include <SFML/Graphics.hpp>
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
        float minX,
        float maxX,
        float speed
    );

    void Update(sf::Time dt) override;
    void Render(sf::RenderWindow& window) const override;
    sf::FloatRect GetBounds() const override;
    bool IsActive() const override;
    void Deactivate() override;
    
    void Fling() override { m_flung = true; }
    bool IsFlung() const override { return m_flung; }

    void TakeDamage();
    bool IsHurt() const { return m_state == BossState::Hurt; }
    
    // Hàm nhận vị trí của Mario từ PlayState
    void SetPlayerPosition(sf::Vector2f playerPos);

private:
    void FireProjectile();
    
    sf::Sprite m_sprite;
    sf::Vector2f m_position;
    sf::Vector2f m_playerPos;

    float m_minX;
    float m_maxX;
    float m_speed;
    
    BossState m_state;
    int m_health;
    bool m_active = true;
    bool m_flung = false;

    bool m_facingLeft = true;
    float m_scale = 1.44f; // Tăng kích thước 20% (1.2f * 1.20)

    float m_stateTimer = 0.f;
    float m_animationTimer = 0.f;
    int m_currentFrame = 0;
};