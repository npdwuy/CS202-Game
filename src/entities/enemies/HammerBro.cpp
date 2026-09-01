#include "entities/enemies/HammerBro.hpp"
#include "resources/ResourceManager.hpp"
#include "events/GameEventManager.hpp"
#include <algorithm>
#include <cmath>

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

static void advanceAnim(const std::vector<sf::IntRect>& frames,
                        int& currentFrame,
                        float& timer,
                        float frameDuration,
                        bool loop,
                        bool& reachedEnd)
{
    if (frames.empty()) return;
    if (timer >= frameDuration) {
        timer -= frameDuration;
        ++currentFrame;
        if (static_cast<std::size_t>(currentFrame) >= frames.size()) {
            if (loop) {
                currentFrame = 0;
            } else {
                currentFrame = static_cast<int>(frames.size()) - 1;
                reachedEnd = true;
            }
        }
    }
}

HammerBro::HammerBro(
    sf::Vector2f position,
    float minX,
    float maxX,
    float speed,
    bool isBig
)
    : m_position(position),
      m_playerPos(position),
      m_minX(std::min(minX, maxX)),
      m_maxX(std::max(minX, maxX)),
      m_speed(speed),
      m_isBig(isBig),
      m_state(HammerBroState::Walk),
      m_health(1),
      m_active(true)
{
    m_scale = m_isBig ? 1.5f : 1.0f;

    const sf::Texture* tex = nullptr;
    try {
        tex = &ResourceManager::getInstance().getTexture("assets/sprites/enemies/hammer_bro.png");
    } catch (...) {
        tex = &ResourceManager::getInstance().getTexture("assets/sprites/enemies/boss.png");
    }
    m_sprite.setTexture(*tex);

    const auto& walkFrames = m_isBig ? framesWalkBig : framesWalkSmall;
    m_sprite.setTextureRect(walkFrames[0]);

    float w = static_cast<float>(walkFrames[0].width > 0 ? walkFrames[0].width : (m_isBig ? 48 : 32));
    float h = static_cast<float>(walkFrames[0].height > 0 ? walkFrames[0].height : (m_isBig ? 64 : 40));
    m_sprite.setOrigin(w * 0.5f, h);
    m_sprite.setScale(m_scale, m_scale);
    m_sprite.setPosition(m_position);
}

void HammerBro::SetPlayerPosition(sf::Vector2f playerPos) {
    m_playerPos = playerPos;
}

void HammerBro::Update(sf::Time dt) {
    if (!m_active || m_state == HammerBroState::Dead) return;

    const float delta = dt.asSeconds();
    m_stateTimer += delta;
    m_animationTimer += delta;
    bool animEnded = false;

    if (m_flung) {
        m_position.y += 400.f * delta;
        m_sprite.setPosition(m_position);
        if (m_position.y > 2000.f) {
            m_active = false;
        }
        return;
    }

    const auto& walkFrames = m_isBig ? framesWalkBig : framesWalkSmall;
    const auto& prepFrames = m_isBig ? framesPrepareAttackBig : framesPrepareAttackSmall;
    const auto& atkFrames  = m_isBig ? framesAttackBig : framesAttackSmall;

    switch (m_state) {
        case HammerBroState::Walk: {
            float distToPlayer = m_playerPos.x - m_position.x;
            float moveDist = m_speed * delta;

            if (distToPlayer > 5.f) {
                m_position.x += moveDist;
                if (m_facingLeft) {
                    m_facingLeft = false;
                    m_sprite.setScale(-m_scale, m_scale);
                }
            } else if (distToPlayer < -5.f) {
                m_position.x -= moveDist;
                if (!m_facingLeft) {
                    m_facingLeft = true;
                    m_sprite.setScale(m_scale, m_scale);
                }
            }

            if (m_position.x < m_minX) m_position.x = m_minX;
            if (m_position.x > m_maxX) m_position.x = m_maxX;

            advanceAnim(walkFrames, m_currentFrame, m_animationTimer, 0.2f, true, animEnded);
            m_sprite.setTextureRect(walkFrames[m_currentFrame]);

            float distX = std::abs(m_playerPos.x - m_position.x);
            float distY = std::abs(m_playerPos.y - m_position.y);
            constexpr float ATTACK_RANGE_X = 400.f;
            constexpr float ATTACK_RANGE_Y = 250.f;

            if (m_stateTimer >= 2.0f) {
                if (distX <= ATTACK_RANGE_X && distY <= ATTACK_RANGE_Y) {
                    m_facingLeft = (m_playerPos.x < m_position.x);
                    m_sprite.setScale(m_facingLeft ? m_scale : -m_scale, m_scale);

                    m_state = HammerBroState::PrepareAttack;
                    m_stateTimer = 0.f;
                    m_currentFrame = 0;
                    m_animationTimer = 0.f;
                } else {
                    m_stateTimer = 1.0f;
                }
            }
            break;
        }

        case HammerBroState::PrepareAttack: {
            m_facingLeft = (m_playerPos.x < m_position.x);
            m_sprite.setScale(m_facingLeft ? m_scale : -m_scale, m_scale);

            advanceAnim(prepFrames, m_currentFrame, m_animationTimer, 0.15f, false, animEnded);
            m_sprite.setTextureRect(prepFrames[m_currentFrame]);

            if (animEnded || m_stateTimer >= 0.35f) {
                FireProjectile();
                m_state = HammerBroState::Attack;
                m_stateTimer = 0.f;
                m_currentFrame = 0;
                m_animationTimer = 0.f;
            }
            break;
        }

        case HammerBroState::Attack: {
            advanceAnim(atkFrames, m_currentFrame, m_animationTimer, 0.15f, false, animEnded);
            m_sprite.setTextureRect(atkFrames[m_currentFrame]);

            if (animEnded || m_stateTimer >= 0.3f) {
                m_state = HammerBroState::Walk;
                m_stateTimer = 0.f;
                m_currentFrame = 0;
                m_animationTimer = 0.f;
            }
            break;
        }

        case HammerBroState::Dead:
            m_active = false;
            break;
    }

    m_sprite.setPosition(m_position);
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

    float w = (m_isBig ? 48.f : 32.f) * m_scale;
    float h = (m_isBig ? 64.f : 40.f) * m_scale;
    float left = m_position.x - w / 2.f;
    float top  = m_position.y - h;
    return sf::FloatRect(left, top, w, h);
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
    m_sprite.setScale(m_sprite.getScale().x, -m_sprite.getScale().y);
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
    m_facingLeft = (m_playerPos.x < m_position.x);
    m_sprite.setScale(m_facingLeft ? m_scale : -m_scale, m_scale);

    sf::Vector2f spawnPos = m_position;
    spawnPos.y -= (m_isBig ? 45.f : 30.f) * m_scale;
    spawnPos.x += m_facingLeft ? -20.f * m_scale : 20.f * m_scale;

    sf::Vector2f velocity(m_facingLeft ? -220.f : 220.f, -450.f);

    std::string eventData = std::to_string(spawnPos.x) + "," +
                            std::to_string(spawnPos.y) + "," +
                            std::to_string(velocity.x) + "," +
                            std::to_string(velocity.y);

    GameEventManager::GetInstance().Notify({
        GameEventType::EnemyFiredProjectile, 0,
        "HammerFiredProjectile:" + eventData
    });
}
