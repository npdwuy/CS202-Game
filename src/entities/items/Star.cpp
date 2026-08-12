#include "entities/items/Star.hpp"

#include <cmath>

namespace {
constexpr float Pi = 3.14159265358979323846f;
}

Star::Star(sf::Vector2f position)
    : FloatingItem(position, 8.f, 3.f) {
    constexpr std::size_t PointCount = 10U;
    constexpr float OuterRadius = 22.f;
    constexpr float InnerRadius = 10.f;

    m_star.setPointCount(PointCount);
    for (std::size_t index = 0; index < PointCount; ++index) {
        const float radius = index % 2U == 0U ? OuterRadius : InnerRadius;
        const float angle = -Pi / 2.f + static_cast<float>(index) * Pi / 5.f;
        m_star.setPoint(index, {
            OuterRadius + std::cos(angle) * radius,
            OuterRadius + std::sin(angle) * radius
        });
    }

    m_star.setOrigin(OuterRadius, OuterRadius);
    m_star.setFillColor(sf::Color(255, 225, 45));
    m_star.setOutlineColor(sf::Color(255, 125, 20));
    m_star.setOutlineThickness(2.f);
    SetVisualPosition(position);
}

void Star::Render(sf::RenderWindow& window) const {
    if (!IsCollected()) {
        window.draw(m_star);
    }
}

sf::FloatRect Star::GetBounds() const {
    return m_star.getGlobalBounds();
}

ItemEffect Star::GetEffect() const {
    return {ItemEffectType::Invincibility, 5};
}

void Star::SetVisualPosition(sf::Vector2f position) {
    m_star.setPosition(position.x + 22.f, position.y + 22.f);
}

void Star::Animate(sf::Time timePerFrame) {
    m_star.rotate(100.f * timePerFrame.asSeconds());
}
