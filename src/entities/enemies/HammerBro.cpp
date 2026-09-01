#include "entities/enemies/HammerBro.hpp"
#include "resources/ResourceManager.hpp"
#include "events/GameEventManager.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

// --- DẠNG NHỎ (SMALL) ---
static const std::vector<sf::IntRect> framesWalkSmall = {
    sf::IntRect(/* Small Walk Frame 1 */ 8, 23, 51, 75),
    sf::IntRect(/* Small Walk Frame 2 */ 60, 23, 51, 75)
};
static const std::vector<sf::IntRect> framesPrepareAttackSmall = {
    sf::IntRect(/* Small Prep Frame 1 */ 123, 23, 51, 75),
    sf::IntRect(/* Small Prep Frame 2 */ 174, 23, 51, 75)
};
static const std::vector<sf::IntRect> framesAttackSmall = {
    sf::IntRect(/* Small Attack Frame 1 */ 236, 23, 51, 75)
};

// --- DẠNG LỚN (BIG) ---
static const std::vector<sf::IntRect> framesWalkBig = {
    sf::IntRect(/* Big Walk Frame 1 */ 300, 0, 75, 97),
    sf::IntRect(/* Big Walk Frame 2 */ 375, 0, 75, 97)
};
static const std::vector<sf::IntRect> framesPrepareAttackBig = {
    sf::IntRect(/* Big Prep Frame 2 */ 462, 0, 75, 97)
};
static const std::vector<sf::IntRect> framesAttackBig = {
    sf::IntRect(/* Big Attack Frame 2 */ 538, 0, 75, 97)
};

static sf::IntRect getValidRect(const std::vector<sf::IntRect>& frames, int frameIndex, bool isBig) {
    if (!frames.empty() && static_cast<size_t>(frameIndex) < frames.size()) {
        sf::IntRect r = frames[frameIndex];
        if (r.width > 0 && r.height > 0) {
            return r;
        }
    }
    int idx = (frameIndex % 2);
    if (isBig) {
        return sf::IntRect((6 + idx) * 48, 0, 48, 97);
    } else {
        return sf::IntRect(idx * 48, 23, 48, 74);
    }
}

HammerBro::HammerBro(
    sf::Vector2f position,
    float speed,
    std::unique_ptr<MovementStrategy> movementStrategy,
    bool isBig
)
    : m_movementStrategy(std::move(movementStrategy)),
      m_speed(speed),
      m_isBig(isBig),
      m_state(HammerBroState::Walk),
      m_health(1),
      m_active(true)
{
    if (!m_movementStrategy) {
        throw std::invalid_argument("HammerBro requires a movement strategy.");
    }

    const sf::Texture& tex = ResourceManager::getInstance().getTexture("assets/sprites/enemies/hammer_bro.png");
    m_sprite.setTexture(tex);

    const auto& walkFrames = m_isBig ? framesWalkBig : framesWalkSmall;
    sf::IntRect rect = getValidRect(walkFrames, 0, m_isBig);
    m_sprite.setTextureRect(rect);
    m_sprite.setOrigin(rect.width * 0.5f, static_cast<float>(rect.height));
    m_sprite.setScale(1.0f, 1.0f);
    m_sprite.setPosition(position);
}

void HammerBro::SetPlayerPosition(sf::Vector2f playerPos) {
    m_playerPos = playerPos;
    if (m_movementStrategy) {
        m_movementStrategy->setPlayerPosition(playerPos);
    }
}

void HammerBro::Update(sf::Time dt) {
    if (!m_active || m_state == HammerBroState::Dead) return;

    const float delta = dt.asSeconds();

    if (m_flung) {
        m_velocity.y += 2000.f * delta;
        m_sprite.move(m_velocity * delta);
        if (m_sprite.getPosition().y > 2000.f) {
            m_active = false;
        }
        return;
    }

    // 1. Walk Animation
    m_animationTimer += delta;
    if (m_animationTimer >= 0.18f) {
        m_animationTimer = 0.f;
        m_currentFrame = (m_currentFrame + 1) % 2;
    }

    const auto& walkFrames = m_isBig ? framesWalkBig : framesWalkSmall;
    sf::IntRect rect = getValidRect(walkFrames, m_currentFrame, m_isBig);
    m_sprite.setTextureRect(rect);

    // 2. Continuous Chase/Patrol Movement (like Goomba & Koopa)
    m_movementStrategy->Update(m_sprite, m_speed, dt);

    // 3. Periodic Hammer Attack (when near Mario)
    float spriteCentreX = m_sprite.getGlobalBounds().left + m_sprite.getGlobalBounds().width * 0.5f;
    float distX = std::abs(m_playerPos.x - spriteCentreX);

    m_attackTimer += delta;
    if (m_attackTimer >= 2.5f) {
        m_attackTimer = 0.f;
        if (distX <= 400.f) {
            FireProjectile();
        }
    }
}

void HammerBro::Render(sf::RenderWindow& window) const {
    if (m_active && m_state != HammerBroState::Dead) {
        window.draw(m_sprite);
    }
}

sf::FloatRect HammerBro::GetBounds() const {
    if (m_state == HammerBroState::Dead || !m_active) {
        return sf::FloatRect(0.f, 0.f, 0.f, 0.f);
    }
    return m_sprite.getGlobalBounds();
}

bool HammerBro::IsActive() const {
    return m_active && m_state != HammerBroState::Dead;
}

void HammerBro::Deactivate() {
    if (!m_active) return;
    m_active = false;
    m_state = HammerBroState::Dead;
}

void HammerBro::Fling() {
    m_flung = true;
    m_velocity = {0.f, -500.f};
    m_sprite.setScale(m_sprite.getScale().x, -std::abs(m_sprite.getScale().y));
    TakeDamage();
}

bool HammerBro::IsFlung() const {
    return m_flung;
}

void HammerBro::TakeDamage() {
    if (!m_active || m_state == HammerBroState::Dead) return;

    --m_health;
    m_state = HammerBroState::Dead;
    m_active = false;

    GameEventManager::GetInstance().Notify({
        GameEventType::EnemyDefeated, 100, "HammerBro Defeated"
    });
}

void HammerBro::FireProjectile() {
    float spriteLeft = m_sprite.getGlobalBounds().left;
    float spriteWidth = m_sprite.getGlobalBounds().width;
    float spriteTop = m_sprite.getGlobalBounds().top;

    bool facingLeft = (m_sprite.getScale().x > 0.f);
    sf::Vector2f spawnPos;
    spawnPos.x = facingLeft ? spriteLeft : (spriteLeft + spriteWidth);
    spawnPos.y = spriteTop + 15.f;

    sf::Vector2f velocity(facingLeft ? -220.f : 220.f, -450.f);

    std::string eventData = std::to_string(spawnPos.x) + "," +
                            std::to_string(spawnPos.y) + "," +
                            std::to_string(velocity.x) + "," +
                            std::to_string(velocity.y);

    GameEventManager::GetInstance().Notify({
        GameEventType::EnemyFiredProjectile, 0,
        "HammerFiredProjectile:" + eventData
    });
}
