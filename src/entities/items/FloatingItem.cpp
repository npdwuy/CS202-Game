#include "entities/items/FloatingItem.hpp"

#include <cmath>

FloatingItem::FloatingItem(
    sf::Vector2f position,
    float floatingDistance,
    float floatingSpeed
)
    : m_basePosition(position),
      m_floatingDistance(floatingDistance),
      m_floatingSpeed(floatingSpeed) {
}

void FloatingItem::Update(sf::Time timePerFrame) {
    if (m_collected) {
        return;
    }

    if (m_isSpawning) {
        m_spawnTime += timePerFrame.asSeconds();
        float t = std::min(m_spawnTime / m_spawnDuration, 1.0f);
        // Dùng ease-out quad để item trồi lên mượt hơn (chậm dần về cuối)
        float easedT = 1.0f - (1.0f - t) * (1.0f - t);
        m_basePosition.y = m_spawnStartPosition.y + (m_spawnTargetY - m_spawnStartPosition.y) * easedT;
        SetVisualPosition(m_basePosition);
        if (t >= 1.0f) {
            m_isSpawning = false;
        }
        return;
    }

    m_animationTime += timePerFrame.asSeconds();
    const float offset = std::sin(m_animationTime * m_floatingSpeed)
                       * m_floatingDistance;
    SetVisualPosition({m_basePosition.x, m_basePosition.y + offset});
    Animate(timePerFrame);
}

bool FloatingItem::IsCollected() const {
    return m_collected;
}

void FloatingItem::Collect() {
    m_collected = true;
}

void FloatingItem::Animate(sf::Time) {
}

void FloatingItem::StartSpawning(float targetY, float duration) {
    m_isSpawning = true;
    m_spawnTargetY = targetY;
    m_spawnDuration = duration;
    m_spawnTime = 0.f;
    m_spawnStartPosition = m_basePosition;
}
