#include "entities/enemies/FlyingEnemy.hpp"

#include <stdexcept>
#include <utility>

FlyingEnemy::FlyingEnemy(
    sf::Vector2f position,
    float speed,
    std::unique_ptr<MovementStrategy> movementStrategy
)
    : m_movementStrategy(std::move(movementStrategy)),
      m_speed(speed),
      m_animationTime(0.f),
      m_currentFrame(0),
      m_active(true)
{
    if (!m_movementStrategy)
    {
        throw std::invalid_argument(
            "FlyingEnemy requires a movement strategy."
        );
    }

    if (!m_texture.loadFromFile(
            "assets/sprites/enemies/flying_enemy.png"))
    {
        throw std::runtime_error(
            "Failed to load FlyingEnemy sprite."
        );
    }

    m_sprite.setTexture(m_texture);
    m_sprite.setTextureRect(
        sf::IntRect(0, 0, 48, 48)
    );
    m_sprite.setPosition(position);
}

void FlyingEnemy::Update(sf::Time timePerFrame)
{
    if (!m_active)
    {
        return;
    }

    m_animationTime += timePerFrame.asSeconds();

    const float frameDuration = 0.2f;

    if (m_animationTime >= frameDuration)
    {
        m_animationTime = 0.f;
        m_currentFrame = (m_currentFrame + 1) % 2;

        m_sprite.setTextureRect(
            sf::IntRect(
                m_currentFrame * 48,
                0,
                48,
                48
            )
        );
    }

    m_movementStrategy->Update(
        m_sprite,
        m_speed,
        timePerFrame
    );
}

void FlyingEnemy::Render(sf::RenderWindow& window) const
{
    if (m_active)
    {
        window.draw(m_sprite);
    }
}

sf::FloatRect FlyingEnemy::GetBounds() const
{
    return m_sprite.getGlobalBounds();
}

bool FlyingEnemy::IsActive() const
{
    return m_active;
}

void FlyingEnemy::Deactivate()
{
    m_active = false;
}