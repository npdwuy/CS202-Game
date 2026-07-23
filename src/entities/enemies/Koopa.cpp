#include "entities/enemies/Koopa.hpp"

#include <stdexcept>
#include <utility>

Koopa::Koopa(
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
            "Koopa requires a movement strategy."
        );
    }

    if (!m_texture.loadFromFile(
            "assets/sprites/enemies/koopa_walk.png"))
    {
        throw std::runtime_error(
            "Failed to load Koopa sprite."
        );
    }

    m_sprite.setTexture(m_texture);
    m_sprite.setTextureRect(
        sf::IntRect(0, 0, 48, 48)
    );
    m_sprite.setPosition(position);
}

void Koopa::Update(sf::Time timePerFrame)
{
    if (!m_active)
    {
        return;
    }

    m_animationTime += timePerFrame.asSeconds();

    const float frameDuration = 0.22f;

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

void Koopa::Render(sf::RenderWindow& window) const
{
    if (m_active)
    {
        window.draw(m_sprite);
    }
}

sf::FloatRect Koopa::GetBounds() const
{
    return m_sprite.getGlobalBounds();
}

bool Koopa::IsActive() const
{
    return m_active;
}

void Koopa::Deactivate()
{
    m_active = false;
}