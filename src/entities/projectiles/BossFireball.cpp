#include "entities/projectiles/BossFireball.hpp"
#include "resources/ResourceManager.hpp"
#include <vector>

BossFireball::BossFireball(sf::Vector2f position, sf::Vector2f velocity)
    : m_velocity(velocity), m_position(position) {
    
    // Load boss_2.png from assets via ResourceManager
    const sf::Texture& texture = ResourceManager::getInstance().getTexture("assets/sprites/enemies/boss.png");
    m_sprite.setTexture(texture);

    // Initial texture rect for the fireball (approximate grid on the right side)
    // Adjust coordinates based on boss_2.png sprite sheet structure.
    m_sprite.setTextureRect(sf::IntRect(1237, 53, 37, 25));
    m_sprite.setOrigin(18.f, 12.f);
    m_sprite.setScale(velocity.x < 0 ? sf::Vector2f(1.5f, 1.5f) : sf::Vector2f(-1.5f, 1.5f));
    m_sprite.setPosition(m_position);
}

void BossFireball::Update(sf::Time dt) {
    m_position += m_velocity * dt.asSeconds();
    m_sprite.setPosition(m_position);

    // Update animation: Cycle through 4 fireball frames every 0.1 seconds
    m_animationTimer += dt.asSeconds();
    if (m_animationTimer >= 0.1f) {
        m_animationTimer -= 0.1f;
        m_currentFrame = (m_currentFrame + 1) % 4;
    }

    // 4 fireball frames assumed side-by-side or stacked on the right side
    static const std::vector<sf::IntRect> frames = {
        sf::IntRect(1238, 79, 24, 24),
        sf::IntRect(1263, 79, 24, 24),
        sf::IntRect(1290, 79, 24, 24),
        sf::IntRect(1315, 79, 24, 24)
    };
    m_sprite.setTextureRect(frames[m_currentFrame]);
}

void BossFireball::Render(sf::RenderWindow& window) {
    if (!m_destroyed) {
        window.draw(m_sprite);
    }
}

sf::FloatRect BossFireball::GetBounds() const {
    return m_sprite.getGlobalBounds();
}
