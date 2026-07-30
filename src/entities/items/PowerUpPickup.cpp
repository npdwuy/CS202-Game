#include "entities/items/PowerUpPickup.hpp"

#include <cmath>

PowerUpPickup::PowerUpPickup(sf::Vector2f position, PowerUpKind kind)
    : m_kind(kind),
      m_baseY(position.y) {
    m_body.setSize({24.f, 20.f});
    m_body.setPosition(position.x + 4.f, position.y + 14.f);
    m_body.setOutlineColor(sf::Color(60, 35, 20));
    m_body.setOutlineThickness(2.f);

    m_cap.setRadius(16.f);
    m_cap.setPosition(position);
    m_cap.setOutlineColor(sf::Color::White);
    m_cap.setOutlineThickness(2.f);

    if (m_kind == PowerUpKind::Mushroom) {
        m_body.setFillColor(sf::Color(245, 224, 180));
        m_cap.setFillColor(sf::Color(220, 55, 45));
    } else {
        m_body.setFillColor(sf::Color(65, 170, 70));
        m_cap.setFillColor(sf::Color(255, 175, 35));
    }
}

void PowerUpPickup::Update(sf::Time timePerFrame) {
    if (m_collected) {
        return;
    }

    m_animationTime += timePerFrame.asSeconds();
    const float offset = std::sin(m_animationTime * 3.f) * 4.f;
    const sf::Vector2f capPosition = m_cap.getPosition();
    m_cap.setPosition(capPosition.x, m_baseY + offset);
    m_body.setPosition(capPosition.x + 4.f, m_baseY + 14.f + offset);
}

void PowerUpPickup::Render(sf::RenderWindow& window) const {
    if (!m_collected) {
        window.draw(m_body);
        window.draw(m_cap);
    }
}

sf::FloatRect PowerUpPickup::GetBounds() const {
    return m_cap.getGlobalBounds();
}

void PowerUpPickup::Collect() {
    m_collected = true;
}

bool PowerUpPickup::IsCollected() const {
    return m_collected;
}

#if __has_include("entities/items/ItemEffect.hpp")
ItemEffect PowerUpPickup::GetEffect() const {
    return {
        m_kind == PowerUpKind::Mushroom
            ? ItemEffectType::GrowPlayer
            : ItemEffectType::EnableFirePower,
        1
    };
}
#endif

PowerUpKind PowerUpPickup::getKind() const {
    return m_kind;
}
