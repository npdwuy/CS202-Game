#pragma once

#include "entities/Enemy.hpp"

class BossEnemy : public Enemy {
public:
    BossEnemy(
        sf::Vector2f position,
        float minimumX,
        float maximumX,
        float speed
    );

    void Update(sf::Time timePerFrame) override;
    void Render(sf::RenderWindow& window) const override;
    sf::FloatRect GetBounds() const override;
    bool IsActive() const override;
    void Deactivate() override;

private:
    sf::RectangleShape m_body;
    sf::RectangleShape m_shell;
    float m_minimumX;
    float m_maximumX;
    float m_speed;
    int m_direction = -1;
    bool m_active = true;
};
