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

// Helper: bỏ qua frame nếu là placeholder (0,0,0,0)
static bool isValidRect(const sf::IntRect& r) {
    return r.width > 0 && r.height > 0;
}

// Helper: giống advanceAnimation của BossEnemy
static void advanceAnimation(
    const std::vector<sf::IntRect>& frames,
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

    const sf::Texture& tex = ResourceManager::getInstance().getTexture(
        "assets/sprites/enemies/hammer_bro.png");
    m_sprite.setTexture(tex);

    const auto& walkFrames = m_isBig ? framesWalkBig : framesWalkSmall;
    sf::IntRect rect = walkFrames[0];
    if (isValidRect(rect)) {
        m_sprite.setTextureRect(rect);
        m_sprite.setOrigin(rect.width * 0.5f, static_cast<float>(rect.height));
    }
    float scaleVal = m_isBig ? 0.8f : 0.7f;
    m_sprite.setScale(scaleVal, scaleVal);
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

    // Nếu bị tung lên trời
    if (m_flung) {
        m_velocity.y += 2000.f * delta;
        m_sprite.move(m_velocity * delta);
        if (m_sprite.getPosition().y > 2000.f) {
            m_active = false;
        }
        return;
    }

    m_stateTimer += delta;
    m_animationTimer += delta;
    bool animationEnded = false;

    const auto& walkFrames    = m_isBig ? framesWalkBig            : framesWalkSmall;
    const auto& prepFrames    = m_isBig ? framesPrepareAttackBig   : framesPrepareAttackSmall;
    const auto& attackFrames  = m_isBig ? framesAttackBig          : framesAttackSmall;

    // Khoảng thời gian animation — Big chậm hơn Small
    const float walkInterval    = m_isBig ? 0.35f : 0.25f;
    const float prepInterval    = m_isBig ? 0.15f : 0.12f;
    const float attackInterval  = m_isBig ? 0.18f : 0.15f;
    // Khoảng cách để kích hoạt tấn công
    constexpr float ATTACK_RANGE_X = 450.f;

    switch (m_state) {

        // ── WALK ─────────────────────────────────────────────────────────────
        case HammerBroState::Walk: {
            // Đi bộ tuần tra / đuổi theo Mario (ChaseStrategy)
            m_movementStrategy->Update(m_sprite, m_speed, dt);

            // Animation đi bộ (loop)
            advanceAnimation(walkFrames, m_currentFrame, m_animationTimer,
                             walkInterval, true, animationEnded);
            {
                const sf::IntRect& fr = walkFrames[m_currentFrame];
                if (isValidRect(fr)) m_sprite.setTextureRect(fr);
            }

            // Đủ thời gian + Mario trong tầm → chuyển sang PrepareAttack
            float distX = std::abs(m_playerPos.x -
                (m_sprite.getGlobalBounds().left + m_sprite.getGlobalBounds().width * 0.5f));

            if (m_stateTimer >= 2.5f) {
                if (distX <= ATTACK_RANGE_X) {
                    m_state = HammerBroState::PrepareAttack;
                    m_stateTimer = 0.f;
                    m_currentFrame = 0;
                    m_animationTimer = 0.f;
                } else {
                    // Mario còn quá xa, reset timer nhỏ để sớm kiểm tra lại
                    m_stateTimer = 1.5f;
                }
            }
            break;
        }

        // ── PREPARE ATTACK ───────────────────────────────────────────────────
        case HammerBroState::PrepareAttack: {
            // Dừng di chuyển, hướng mặt về phía Mario
            bool faceLeft = (m_playerPos.x < m_sprite.getPosition().x);
            float absScale = std::abs(m_sprite.getScale().x);
            m_sprite.setScale(faceLeft ? absScale : -absScale, absScale);

            // Animation prepare (một lần, không loop)
            advanceAnimation(prepFrames, m_currentFrame, m_animationTimer,
                             prepInterval, false, animationEnded);
            {
                const sf::IntRect& fr = prepFrames[m_currentFrame];
                if (isValidRect(fr)) m_sprite.setTextureRect(fr);
            }

            // Khi animation kết thúc → ném búa rồi chuyển sang Attack
            if (animationEnded) {
                FireProjectile();
                m_state = HammerBroState::Attack;
                m_currentFrame = 0;
                m_animationTimer = 0.f;
                animationEnded = false;
            }
            break;
        }

        // ── ATTACK ───────────────────────────────────────────────────────────
        case HammerBroState::Attack: {
            // Animation attack (một lần, không loop)
            advanceAnimation(attackFrames, m_currentFrame, m_animationTimer,
                             attackInterval, false, animationEnded);
            {
                const sf::IntRect& fr = attackFrames[m_currentFrame];
                if (isValidRect(fr)) m_sprite.setTextureRect(fr);
            }

            // Khi animation kết thúc → quay lại Walk
            if (animationEnded) {
                m_state = HammerBroState::Walk;
                m_stateTimer = 0.f;
                m_currentFrame = 0;
                m_animationTimer = 0.f;
                animationEnded = false;
            }
            break;
        }

        default:
            break;
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
    sf::FloatRect full = m_sprite.getGlobalBounds();
    const float shrinkX = full.width * 0.15f;
    const float shrinkY = full.height * 0.10f;
    return sf::FloatRect(
        full.left + shrinkX,
        full.top + shrinkY,
        full.width - shrinkX * 2.f,
        full.height - shrinkY * 2.f
    );
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
    float spriteLeft  = m_sprite.getGlobalBounds().left;
    float spriteWidth = m_sprite.getGlobalBounds().width;
    float spriteTop   = m_sprite.getGlobalBounds().top;

    bool facingLeft = (m_sprite.getScale().x > 0.f);
    sf::Vector2f spawnPos;
    spawnPos.x = facingLeft ? spriteLeft : (spriteLeft + spriteWidth);
    spawnPos.y = spriteTop + 15.f;

    sf::Vector2f velocity(facingLeft ? -220.f : 220.f, -450.f);
    // Small hammer projectile: -30% (0.525f), Big hammer projectile: +10% (0.825f)
    float hammerScale = m_isBig ? 0.825f : 0.525f;

    std::string eventData =
        std::to_string(spawnPos.x) + "," +
        std::to_string(spawnPos.y) + "," +
        std::to_string(velocity.x) + "," +
        std::to_string(velocity.y) + "," +
        std::to_string(hammerScale);

    GameEventManager::GetInstance().Notify({
        GameEventType::EnemyFiredProjectile, 0,
        "HammerFiredProjectile:" + eventData
    });
}
