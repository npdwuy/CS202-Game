#include "entities/enemies/BossEnemy.hpp"

#include <algorithm>

BossEnemy::BossEnemy(
    sf::Vector2f position,
    float minimumX,
    float maximumX,
    float speed
)
    : m_minimumX(std::min(minimumX, maximumX)),
      m_maximumX(std::max(minimumX, maximumX)),
      m_speed(speed) {
    m_body.setSize({72.f, 72.f});
    m_body.setPosition(position);
    m_body.setFillColor(sf::Color(185, 45, 35));
    m_body.setOutlineColor(sf::Color(65, 20, 15));
    m_body.setOutlineThickness(4.f);

    m_shell.setSize({48.f, 38.f});
    m_shell.setPosition(position.x + 12.f, position.y + 18.f);
    m_shell.setFillColor(sf::Color(40, 125, 55));
    m_shell.setOutlineColor(sf::Color(20, 65, 30));
    m_shell.setOutlineThickness(3.f);
}

void BossEnemy::Update(sf::Time timePerFrame) {
    if (!m_active) {
        return;
    }

    const float movement =
        static_cast<float>(m_direction) * m_speed * timePerFrame.asSeconds();
    m_body.move(movement, 0.f);
    m_shell.move(movement, 0.f);

    const float currentX = m_body.getPosition().x;
    if (currentX <= m_minimumX) {
        const float correction = m_minimumX - currentX;
        m_body.move(correction, 0.f);
        m_shell.move(correction, 0.f);
        m_direction = 1;
    } else if (currentX >= m_maximumX) {
        const float correction = m_maximumX - currentX;
        m_body.move(correction, 0.f);
        m_shell.move(correction, 0.f);
        m_direction = -1;
    }
}

void BossEnemy::Render(sf::RenderWindow& window) const {
    if (m_active) {
        window.draw(m_body);
        window.draw(m_shell);
    }
}

sf::FloatRect BossEnemy::GetBounds() const {
    return m_body.getGlobalBounds();
}

bool BossEnemy::IsActive() const {
    return m_active;
}

void BossEnemy::Deactivate() {
    m_active = false;
}
