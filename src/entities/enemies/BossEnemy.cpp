#include "entities/enemies/BossEnemy.hpp"
#include "resources/ResourceManager.hpp"
#include "events/GameEventManager.hpp"
#include <algorithm>
#include <cmath>

/* --------------------------------------------------------------
   Animation frames from boss.png sprite sheet.
   Frame size: width = 85 px, height = 65 px.
   -------------------------------------------------------------- */
static const std::vector<sf::IntRect> framesWalk = {
    sf::IntRect(12, 42, 85, 65),
    sf::IntRect(116, 42, 85, 65),
    sf::IntRect(217, 42, 85, 65),
    sf::IntRect(317, 42, 85, 65),
    sf::IntRect(417, 42, 85, 65)
};
static const std::vector<sf::IntRect> framesTurn = {
    sf::IntRect(524, 42, 85, 65),
    sf::IntRect(624, 42, 85, 65),
    sf::IntRect(724, 42, 85, 65)
};
static const std::vector<sf::IntRect> framesPrepareAttack = {
    sf::IntRect(12,   137, 85, 65),
    sf::IntRect(112,  137, 85, 65),
    sf::IntRect(212,  137, 85, 65),
    sf::IntRect(312,  137, 85, 65),
    sf::IntRect(412,  137, 85, 65)
};
static const std::vector<sf::IntRect> framesAttack = {
    sf::IntRect(519,  139, 85, 65),
    sf::IntRect(619,  139, 85, 65)
};
static const std::vector<sf::IntRect> framesHurt = {
    sf::IntRect(724,  139, 85, 65),
    sf::IntRect(824,  139, 85, 65)
};

static void advanceAnimation(const std::vector<sf::IntRect>& frames,
                             int& currentFrame,
                             float& timer,
                             float frameDuration,
                             bool loop,
                             bool& reachedLast)
{
    if (timer >= frameDuration) {
        timer -= frameDuration;
        ++currentFrame;
        if (static_cast<std::size_t>(currentFrame) >= frames.size()) {
            if (loop) {
                currentFrame = 0;
            } else {
                currentFrame = static_cast<int>(frames.size()) - 1;
                reachedLast = true;
            }
        }
    }
}

BossEnemy::BossEnemy(
    sf::Vector2f position,
    float minX,
    float maxX,
    float speed
)
    : m_position(position),
      m_playerPos(position),
      m_minX(std::min(minX, maxX)),
      m_maxX(std::max(minX, maxX)),
      m_speed(speed),
      m_state(BossState::Walk),
      m_health(5)
{
    const sf::Texture& tex = ResourceManager::getInstance().getTexture(
        "assets/sprites/enemies/boss.png");
    m_sprite.setTexture(tex);
    m_sprite.setTextureRect(framesWalk[0]);
    m_sprite.setOrigin(42.5f, 65.f); // Origin nằm ở dưới cùng ở giữa
    m_sprite.setScale(m_scale, m_scale);
    m_sprite.setPosition(m_position);
}

void BossEnemy::SetPlayerPosition(sf::Vector2f playerPos) {
    m_playerPos = playerPos;
}

void BossEnemy::Update(sf::Time dt) {
    if (!m_active || m_state == BossState::Dead) return;

    const float delta = dt.asSeconds();
    m_stateTimer += delta;
    m_animationTimer += delta;
    bool animationEnded = false;

    switch (m_state) {
        case BossState::Walk: {
            // 1. Logic đi theo Mario (Chasing)
            float distToPlayer = m_playerPos.x - m_position.x;
            float moveDistance = m_speed * delta;

            // Khoảng cách an toàn (deadzone) để Boss không bị giật lag khi đứng quá gần
            if (distToPlayer > 5.f) { 
                // Mario ở bên phải -> Đi sang phải
                m_position.x += moveDistance;
                if (m_facingLeft) {
                    m_facingLeft = false;
                    m_sprite.setScale(-m_scale, m_scale);
                }
            } else if (distToPlayer < -5.f) { 
                // Mario ở bên trái -> Đi sang trái
                m_position.x -= moveDistance;
                if (!m_facingLeft) {
                    m_facingLeft = true;
                    m_sprite.setScale(m_scale, m_scale);
                }
            }

            // 2. Chặn Boss không đi lố giới hạn của map
            if (m_position.x < m_minX) m_position.x = m_minX;
            if (m_position.x > m_maxX) m_position.x = m_maxX;

            // 3. Animation đi bộ
            advanceAnimation(framesWalk, m_currentFrame, m_animationTimer,
                             0.15f, true, animationEnded);
            m_sprite.setTextureRect(framesWalk[m_currentFrame]);

            // 4. Chuyển sang chuẩn bị tấn công khi Mario nằm trong phạm vi
            float distX = std::abs(m_playerPos.x - m_position.x);
            float distY = std::abs(m_playerPos.y - m_position.y);
            constexpr float ATTACK_RANGE_X = 450.f;
            constexpr float ATTACK_RANGE_Y = 250.f;

            if (m_stateTimer >= 2.5f) {
                if (distX <= ATTACK_RANGE_X && distY <= ATTACK_RANGE_Y) {
                    m_facingLeft = (m_playerPos.x < m_position.x);
                    m_sprite.setScale(m_facingLeft ? m_scale : -m_scale, m_scale);

                    m_state = BossState::PrepareAttack;
                    m_stateTimer = 0.f;
                    m_currentFrame = 0;
                    m_animationTimer = 0.f;
                } else {
                    m_stateTimer = 1.5f;
                }
            }
            break;
        }
        case BossState::PrepareAttack: {
            m_facingLeft = (m_playerPos.x < m_position.x);
            m_sprite.setScale(m_facingLeft ? m_scale : -m_scale, m_scale);

            advanceAnimation(framesPrepareAttack, m_currentFrame,
                             m_animationTimer, 0.12f, false, animationEnded);
            m_sprite.setTextureRect(framesPrepareAttack[m_currentFrame]);

            if (animationEnded) {
                FireProjectile();
                m_state = BossState::Attack;
                m_currentFrame = 0;
                m_animationTimer = 0.f;
                animationEnded = false;
            }
            break;
        }
        case BossState::Attack: {
            advanceAnimation(framesAttack, m_currentFrame, m_animationTimer,
                             0.15f, false, animationEnded);
            m_sprite.setTextureRect(framesAttack[m_currentFrame]);

            if (animationEnded) {
                m_state = BossState::Walk;
                m_stateTimer = 0.f;
                m_currentFrame = 0;
                m_animationTimer = 0.f;
                animationEnded = false;
            }
            break;
        }
        case BossState::Hurt: {
            advanceAnimation(framesHurt, m_currentFrame, m_animationTimer,
                             0.15f, false, animationEnded);
            m_sprite.setTextureRect(framesHurt[m_currentFrame]);
            
            // Hiệu ứng nhấp nháy đỏ khi bị tấn công
            if (static_cast<int>(m_stateTimer / 0.08f) % 2 == 0) {
                m_sprite.setColor(sf::Color(255, 60, 60));
            } else {
                m_sprite.setColor(sf::Color::White);
            }

            if (m_stateTimer >= 0.8f) {
                m_state = BossState::Walk;
                m_stateTimer = 0.f;
                m_currentFrame = 0;
                m_animationTimer = 0.f;
                m_sprite.setColor(sf::Color::White);
                animationEnded = false;
            }
            break;
        }
        default:
            break;
    }

    // Cập nhật tọa độ thực tế lên màn hình
    m_sprite.setPosition(m_position);
}


void BossEnemy::Render(sf::RenderWindow& window) const {
    if (m_active) window.draw(m_sprite);
}

sf::FloatRect BossEnemy::GetBounds() const {
    if (m_state == BossState::Dead) {
        return sf::FloatRect(0.f, 0.f, 0.f, 0.f);
    }

    float w = 85.f * m_scale;
    float h = 65.f * m_scale;
    float left = m_position.x - w / 2.f;
    float top  = m_position.y - h;
    return sf::FloatRect(left, top, w, h);
}

bool BossEnemy::IsActive() const { return m_active; }

void BossEnemy::Deactivate() { m_active = false; }

void BossEnemy::TakeDamage() {
    if (m_state == BossState::Hurt || m_state == BossState::Dead) return;

    --m_health;
    if (m_health <= 0) {
        m_state = BossState::Dead;
        m_active = false;
        GameEventManager::GetInstance().Notify({
            GameEventType::EnemyDefeated, 500, "Boss Defeated" });
    } else {
        m_state = BossState::Hurt;
        m_stateTimer = 0.f;
        m_currentFrame = 0;
        m_animationTimer = 0.f;
        m_sprite.setColor(sf::Color(255, 60, 60));
    }
}

void BossEnemy::FireProjectile() {
    m_facingLeft = (m_playerPos.x < m_position.x);
    m_sprite.setScale(m_facingLeft ? m_scale : -m_scale, m_scale);

    sf::Vector2f spawnPos = m_position;
    spawnPos.y -= 35.f * m_scale;
    spawnPos.x += m_facingLeft ? -45.f * m_scale : 45.f * m_scale;

    sf::Vector2f velocity = sf::Vector2f(m_facingLeft ? -320.f : 320.f, 0.f);

    std::string eventData = std::to_string(spawnPos.x) + "," +
                            std::to_string(spawnPos.y) + "," +
                            std::to_string(velocity.x) + "," +
                            std::to_string(velocity.y);

    GameEventManager::GetInstance().Notify({
        GameEventType::EnemyFiredProjectile, 0,
        "BossFiredProjectile:" + eventData });
}