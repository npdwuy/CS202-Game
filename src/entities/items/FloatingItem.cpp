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
