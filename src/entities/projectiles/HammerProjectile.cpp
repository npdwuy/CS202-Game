#include "entities/projectiles/HammerProjectile.hpp"
#include "resources/ResourceManager.hpp"
#include <vector>

static const std::vector<sf::IntRect> framesHammer = {
    sf::IntRect(/* Frame 1 */ 0, 0, 0, 0),
    sf::IntRect(/* Frame 2 */ 0, 0, 0, 0)
};

HammerProjectile::HammerProjectile(sf::Vector2f position, sf::Vector2f velocity)
    : m_velocity(velocity), m_position(position) {

    const sf::Texture* texture = nullptr;
    try {
        texture = &ResourceManager::getInstance().getTexture("assets/sprites/enemies/hammer_bro.png");
    } catch (...) {
        texture = &ResourceManager::getInstance().getTexture("assets/sprites/enemies/boss.png");
    }
    m_sprite.setTexture(*texture);
    m_sprite.setTextureRect(framesHammer[0]);
    float w = static_cast<float>(framesHammer[0].width > 0 ? framesHammer[0].width : 16);
    float h = static_cast<float>(framesHammer[0].height > 0 ? framesHammer[0].height : 16);
    m_sprite.setOrigin(w * 0.5f, h * 0.5f);
    m_sprite.setScale(velocity.x < 0 ? sf::Vector2f(1.5f, 1.5f) : sf::Vector2f(-1.5f, 1.5f));
    m_sprite.setPosition(m_position);
}

void HammerProjectile::Update(sf::Time dt) {
    const float dtSec = dt.asSeconds();

    // Gravity acceleration for arc trajectory
    m_velocity.y += 980.f * dtSec;
    m_position += m_velocity * dtSec;
    m_sprite.setPosition(m_position);

    // Rotate hammer sprite in flight
    m_sprite.rotate(720.f * dtSec);

    // Cycle through animation frames if non-empty
    m_animationTimer += dtSec;
    if (m_animationTimer >= 0.1f) {
        m_animationTimer -= 0.1f;
        m_currentFrame = (m_currentFrame + 1) % framesHammer.size();
        m_sprite.setTextureRect(framesHammer[m_currentFrame]);
    }
}

void HammerProjectile::Render(sf::RenderWindow& window) {
    if (!m_destroyed) {
        window.draw(m_sprite);
    }
}

sf::FloatRect HammerProjectile::GetBounds() const {
    return m_sprite.getGlobalBounds();
}
