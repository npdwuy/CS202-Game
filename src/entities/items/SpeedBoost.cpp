#include "entities/items/SpeedBoost.hpp"

#include <cmath>

SpeedBoost::SpeedBoost(sf::Vector2f position)
    : FloatingItem(position, 6.f, 3.5f),
      m_badge(21.f, 8U) {
    m_badge.setFillColor(sf::Color(25, 175, 245));
    m_badge.setOutlineColor(sf::Color(210, 250, 255));
    m_badge.setOutlineThickness(3.f);

    m_arrow.setPointCount(7U);
    m_arrow.setPoint(0U, {0.f, 7.f});
    m_arrow.setPoint(1U, {18.f, 7.f});
    m_arrow.setPoint(2U, {18.f, 0.f});
    m_arrow.setPoint(3U, {32.f, 13.f});
    m_arrow.setPoint(4U, {18.f, 26.f});
    m_arrow.setPoint(5U, {18.f, 19.f});
    m_arrow.setPoint(6U, {0.f, 19.f});
    m_arrow.setFillColor(sf::Color::White);
    SetVisualPosition(position);
}

void SpeedBoost::Render(sf::RenderWindow& window) const {
    if (!IsCollected()) {
        window.draw(m_badge);
        window.draw(m_arrow);
    }
}

sf::FloatRect SpeedBoost::GetBounds() const {
    return m_badge.getGlobalBounds();
}

ItemEffect SpeedBoost::GetEffect() const {
    return {ItemEffectType::SpeedBoost, 8};
}

void SpeedBoost::SetVisualPosition(sf::Vector2f position) {
    m_badge.setPosition(position.x + 2.f, position.y + 2.f);
    m_arrow.setPosition(position.x + 7.f, position.y + 10.f);
}

void SpeedBoost::Animate(sf::Time timePerFrame) {
    m_pulseTime += timePerFrame.asSeconds();
    const auto channel = static_cast<sf::Uint8>(
        195.f + std::sin(m_pulseTime * 6.f) * 45.f
    );
    m_badge.setFillColor(sf::Color(25, channel, 245));
}
