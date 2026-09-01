#include "entities/enemies/HammerBro.hpp"
#include "resources/ResourceManager.hpp"
#include "events/GameEventManager.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

// --- DẠNG NHỎ (SMALL) ---
static const std::vector<sf::IntRect> framesWalkSmall = {
    sf::IntRect(/* Small Walk Frame 1 */ 0, 0, 0, 0),
    sf::IntRect(/* Small Walk Frame 2 */ 0, 0, 0, 0)
};
static const std::vector<sf::IntRect> framesPrepareAttackSmall = {
    sf::IntRect(/* Small Prep Frame 1 */ 0, 0, 0, 0),
    sf::IntRect(/* Small Prep Frame 2 */ 0, 0, 0, 0)
};
static const std::vector<sf::IntRect> framesAttackSmall = {
    sf::IntRect(/* Small Attack Frame 1 */ 0, 0, 0, 0),
    sf::IntRect(/* Small Attack Frame 2 */ 0, 0, 0, 0)
};

// --- DẠNG LỚN (BIG) ---
static const std::vector<sf::IntRect> framesWalkBig = {
    sf::IntRect(/* Big Walk Frame 1 */ 0, 0, 0, 0),
    sf::IntRect(/* Big Walk Frame 2 */ 0, 0, 0, 0)
};
static const std::vector<sf::IntRect> framesPrepareAttackBig = {
    sf::IntRect(/* Big Prep Frame 1 */ 0, 0, 0, 0),
    sf::IntRect(/* Big Prep Frame 2 */ 0, 0, 0, 0)
};
static const std::vector<sf::IntRect> framesAttackBig = {
    sf::IntRect(/* Big Attack Frame 1 */ 0, 0, 0, 0),
    sf::IntRect(/* Big Attack Frame 2 */ 0, 0, 0, 0)
};

static sf::IntRect getValidRect(const std::vector<sf::IntRect>& frames, int frameIndex, bool isBig) {
    if (frames.empty() || static_cast<size_t>(frameIndex) >= frames.size()) {
        return sf::IntRect((frameIndex % 2) * 48, 0, 48, 48);
    }
    sf::IntRect r = frames[frameIndex];
    if (r.width <= 0 || r.height <= 0) {
        return sf::IntRect((frameIndex % 2) * 48, 0, 48, 48);
    }
    return r;
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

    const sf::Texture* tex = nullptr;
    try {
        tex = &ResourceManager::getInstance().getTexture("assets/sprites/enemies/hammer_bro.png");
    } catch (...) {
        try {
            tex = &ResourceManager::getInstance().getTexture("assets/sprites/enemies/koopa_walk.png");
        } catch (...) {
            tex = &ResourceManager::getInstance().getTexture("assets/sprites/enemies/boss.png");
        }
    }
    m_sprite.setTexture(*tex);

    const auto& walkFrames = m_isBig ? framesWalkBig : framesWalkSmall;
    m_sprite.setTextureRect(getValidRect(walkFrames, 0, m_isBig));
    m_sprite.setScale(m_isBig ? 1.5f : 1.0f, m_isBig ? 1.5f : 1.0f);
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
    m_sprite.setTextureRect(getValidRect(walkFrames, m_currentFrame, m_isBig));

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

    sf::FloatRect bounds = m_sprite.getGlobalBounds();
    bounds.left += 4.f;
    bounds.width -= 8.f;
    bounds.top += 4.f;
    bounds.height -= 4.f;
    return bounds;
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
    spawnPos.y = spriteTop + 10.f;

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
