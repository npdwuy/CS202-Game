#include "entities/enemies/BossEnemy.hpp"
#include "entities/SpriteAnimation.hpp"
#include "resources/ResourceManager.hpp"
#include "events/GameEventManager.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

static const std::vector<sf::IntRect> WALK_FRAMES = {
    {12, 42, 85, 65},
    {116, 42, 85, 65},
    {217, 42, 85, 65},
    {317, 42, 85, 65},
    {417, 42, 85, 65}
};

static const std::vector<sf::IntRect> PREPARE_ATTACK_FRAMES = {
    {12, 137, 85, 65},
    {112, 137, 85, 65},
    {212, 137, 85, 65},
    {312, 137, 85, 65},
    {412, 137, 85, 65}
};

static const std::vector<sf::IntRect> ATTACK_FRAMES = {
    {519, 139, 85, 65},
    {619, 139, 85, 65}
};

static const std::vector<sf::IntRect> HURT_FRAMES = {
    {724, 139, 85, 65},
    {824, 139, 85, 65}
};

BossEnemy::BossEnemy(
    sf::Vector2f position,
    float speed,
    std::unique_ptr<MovementStrategy> movementStrategy)
    : m_movementStrategy(std::move(movementStrategy)),
      m_speed(speed)
{
    if (!m_movementStrategy) {
        throw std::invalid_argument("BossEnemy requires a movement strategy.");
    }

    const sf::Texture& texture = ResourceManager::getInstance().getTexture(
        "assets/sprites/enemies/boss.png");
    m_sprite.setTexture(texture);
    m_sprite.setTextureRect(WALK_FRAMES[0]);
    m_sprite.setOrigin(FRAME_WIDTH * 0.5f, FRAME_HEIGHT);
    m_sprite.setScale(m_spriteScale, m_spriteScale);
    m_sprite.setPosition(position);
    m_playerPos = position;
}

void BossEnemy::SetPlayerPosition(sf::Vector2f playerPos) {
    m_playerPos = playerPos;
    m_movementStrategy->setPlayerPosition(playerPos);
}

void BossEnemy::Update(sf::Time dt) {
    if (!m_active || m_state == BossState::Dead) return;

    const float delta = dt.asSeconds();
    m_stateTimer += delta;
    m_animationTimer += delta;
    bool animationEnded = false;

    switch (m_state) {
        case BossState::Walk: {
            m_movementStrategy->Update(m_sprite, m_speed, dt);

            SpriteAnimation::Advance(
                WALK_FRAMES, m_currentFrame, m_animationTimer,
                0.15f, true, animationEnded);
            m_sprite.setTextureRect(WALK_FRAMES[m_currentFrame]);

            float spriteCenterX = m_sprite.getPosition().x;
            float distX = std::abs(m_playerPos.x - spriteCenterX);
            float distY = std::abs(m_playerPos.y - m_sprite.getPosition().y);

            if (m_stateTimer >= WALK_COOLDOWN) {
                if (distX <= ATTACK_RANGE_X && distY <= ATTACK_RANGE_Y) {
                    bool faceLeft = (m_playerPos.x < spriteCenterX);
                    float absScale = std::abs(m_sprite.getScale().x);
                    m_sprite.setScale(faceLeft ? absScale : -absScale, absScale);

                    m_state = BossState::PrepareAttack;
                    m_stateTimer = 0.f;
                    m_currentFrame = 0;
                    m_animationTimer = 0.f;
                } else {
                    m_stateTimer = WALK_RESET;
                }
            }
            break;
        }

        case BossState::PrepareAttack: {
            bool faceLeft = (m_playerPos.x < m_sprite.getPosition().x);
            float absScale = std::abs(m_sprite.getScale().x);
            m_sprite.setScale(faceLeft ? absScale : -absScale, absScale);

            SpriteAnimation::Advance(
                PREPARE_ATTACK_FRAMES, m_currentFrame, m_animationTimer,
                0.12f, false, animationEnded);
            m_sprite.setTextureRect(PREPARE_ATTACK_FRAMES[m_currentFrame]);

            if (animationEnded) {
                FireProjectile();
                m_state = BossState::Attack;
                m_currentFrame = 0;
                m_animationTimer = 0.f;
            }
            break;
        }

        case BossState::Attack: {
            SpriteAnimation::Advance(
                ATTACK_FRAMES, m_currentFrame, m_animationTimer,
                0.15f, false, animationEnded);
            m_sprite.setTextureRect(ATTACK_FRAMES[m_currentFrame]);

            if (animationEnded) {
                m_state = BossState::Walk;
                m_stateTimer = 0.f;
                m_currentFrame = 0;
                m_animationTimer = 0.f;
            }
            break;
        }

        case BossState::Hurt: {
            SpriteAnimation::Advance(
                HURT_FRAMES, m_currentFrame, m_animationTimer,
                0.15f, false, animationEnded);
            m_sprite.setTextureRect(HURT_FRAMES[m_currentFrame]);

            bool flashRed = static_cast<int>(m_stateTimer / HURT_FLASH_INTERVAL) % 2 == 0;
            m_sprite.setColor(flashRed ? sf::Color(255, 60, 60) : sf::Color::White);

            if (m_stateTimer >= HURT_DURATION) {
                m_state = BossState::Walk;
                m_stateTimer = 0.f;
                m_currentFrame = 0;
                m_animationTimer = 0.f;
                m_sprite.setColor(sf::Color::White);
            }
            break;
        }

        default:
            break;
    }
}

void BossEnemy::Render(sf::RenderWindow& window) const {
    if (m_active) {
        window.draw(m_sprite);
    }
}

sf::FloatRect BossEnemy::GetBounds() const {
    if (m_state == BossState::Dead) {
        return {0.f, 0.f, 0.f, 0.f};
    }

    float w = FRAME_WIDTH * m_spriteScale;
    float h = FRAME_HEIGHT * m_spriteScale;
    float left = m_sprite.getPosition().x - w * 0.5f;
    float top = m_sprite.getPosition().y - h;
    return {left, top, w, h};
}

bool BossEnemy::IsActive() const { return m_active; }

void BossEnemy::Deactivate() {
    m_active = false;
    m_state = BossState::Dead;
}

void BossEnemy::TakeDamage() {
    if (m_state == BossState::Hurt || m_state == BossState::Dead) return;

    --m_health;
    if (m_health <= 0) {
        m_state = BossState::Dead;
        m_active = false;
        GameEventManager::GetInstance().Notify({
            GameEventType::EnemyDefeated, DEFEAT_SCORE, "Boss Defeated"});
    } else {
        m_state = BossState::Hurt;
        m_stateTimer = 0.f;
        m_currentFrame = 0;
        m_animationTimer = 0.f;
        m_sprite.setColor(sf::Color(255, 60, 60));
    }
}

bool BossEnemy::IsHurt() const {
    return m_state == BossState::Hurt;
}

bool BossEnemy::IsBoss() const {
    return true;
}

void BossEnemy::Fling() {
    m_flung = true;
}

bool BossEnemy::IsFlung() const {
    return m_flung;
}

void BossEnemy::FireProjectile() {
    bool faceLeft = (m_playerPos.x < m_sprite.getPosition().x);
    float absScale = std::abs(m_sprite.getScale().x);
    m_sprite.setScale(faceLeft ? absScale : -absScale, absScale);

    sf::Vector2f spawnPos = m_sprite.getPosition();
    spawnPos.y -= 35.f * m_spriteScale;
    spawnPos.x += faceLeft ? -45.f * m_spriteScale : 45.f * m_spriteScale;

    sf::Vector2f velocity(faceLeft ? -320.f : 320.f, 0.f);

    std::string eventData = std::to_string(spawnPos.x) + "," +
                            std::to_string(spawnPos.y) + "," +
                            std::to_string(velocity.x) + "," +
                            std::to_string(velocity.y);

    GameEventManager::GetInstance().Notify({
        GameEventType::EnemyFiredProjectile, 0,
        "BossFiredProjectile:" + eventData});
}