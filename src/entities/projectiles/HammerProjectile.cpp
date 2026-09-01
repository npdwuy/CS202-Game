#include "entities/projectiles/HammerProjectile.hpp"
#include "resources/ResourceManager.hpp"
#include <vector>

static const std::vector<sf::IntRect> framesHammer = {
    sf::IntRect(/* Frame 1 */ 0, 0, 0, 0),
    sf::IntRect(/* Frame 2 */ 0, 0, 0, 0)
};

static sf::IntRect getValidHammerRect(const std::vector<sf::IntRect>& frames, int frameIndex) {
    if (frames.empty() || static_cast<size_t>(frameIndex) >= frames.size()) {
        return sf::IntRect(0, 0, 24, 24);
    }
    sf::IntRect r = frames[frameIndex];
    if (r.width <= 0 || r.height <= 0) {
        return sf::IntRect(0, 0, 24, 24);
    }
    return r;
}

HammerProjectile::HammerProjectile(sf::Vector2f position, sf::Vector2f velocity)
    : m_velocity(velocity), m_position(position) {

    const sf::Texture* texture = nullptr;
    try {
        texture = &ResourceManager::getInstance().getTexture("assets/sprites/enemies/hammer_bro.png");
    } catch (...) {
        texture = &ResourceManager::getInstance().getTexture("assets/sprites/enemies/boss.png");
    }
    m_sprite.setTexture(*texture);

    sf::IntRect rect = getValidHammerRect(framesHammer, 0);
    m_sprite.setTextureRect(rect);
    m_sprite.setOrigin(rect.width * 0.5f, rect.height * 0.5f);
    m_sprite.setScale(1.2f, 1.2f);
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

    // Cycle through animation frames
    m_animationTimer += dtSec;
    if (m_animationTimer >= 0.1f) {
        m_animationTimer -= 0.1f;
        m_currentFrame = (m_currentFrame + 1) % 2;
        sf::IntRect rect = getValidHammerRect(framesHammer, m_currentFrame);
        m_sprite.setTextureRect(rect);
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
