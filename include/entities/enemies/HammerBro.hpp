#pragma once

#include "entities/Enemy.hpp"
#include <SFML/Graphics.hpp>
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
        float minX,
        float maxX,
        float speed = 40.f,
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
    sf::Vector2f m_position;
    sf::Vector2f m_playerPos;

    float m_minX;
    float m_maxX;
    float m_speed;

    bool m_isBig = false;
    HammerBroState m_state = HammerBroState::Walk;
    int m_health = 1;
    bool m_active = true;
    bool m_flung = false;

    bool m_facingLeft = true;
    float m_scale = 1.0f;

    float m_stateTimer = 0.f;
    float m_animationTimer = 0.f;
    int m_currentFrame = 0;
};
