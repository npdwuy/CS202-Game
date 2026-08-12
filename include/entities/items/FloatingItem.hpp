#pragma once

#include "entities/Item.hpp"

class FloatingItem : public Item {
public:
    void Update(sf::Time timePerFrame) final;
    bool IsCollected() const final;
    void Collect() final;
    
    void StartSpawning(float targetY, float duration);

protected:
    FloatingItem(
        sf::Vector2f position,
        float floatingDistance,
        float floatingSpeed
    );

    virtual void SetVisualPosition(sf::Vector2f position) = 0;
    virtual void Animate(sf::Time timePerFrame);

private:
    sf::Vector2f m_basePosition;
    float m_floatingDistance;
    float m_floatingSpeed;
    float m_animationTime = 0.f;
    bool m_collected = false;

    // Spawning animation
    bool m_isSpawning = false;
    float m_spawnTargetY = 0.f;
    float m_spawnDuration = 0.f;
    float m_spawnTime = 0.f;
    sf::Vector2f m_spawnStartPosition;
};
