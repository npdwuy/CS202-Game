#include "entities/enemies/Goomba.hpp"

#include <stdexcept>
#include <utility>

Goomba::Goomba(
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
            "Goomba requires a movement strategy."
        );
    }

    if (!m_texture.loadFromFile(
            "assets/sprites/enemies/goomba_walk.png"))
    {
        throw std::runtime_error(
            "Failed to load Goomba sprite."
        );
    }

    m_sprite.setTexture(m_texture);
    m_sprite.setTextureRect(sf::IntRect(0, 0, 16, 16));
    m_sprite.setScale(3.f, 3.f);
    m_sprite.setPosition(position);
}

void Goomba::Update(sf::Time timePerFrame)
{
    if (!m_active)
    {
        return;
    }

    m_animationTime += timePerFrame.asSeconds();

    const float frameDuration = 0.18f;

    if (m_animationTime >= frameDuration)
    {
        m_animationTime = 0.f;
        m_currentFrame = (m_currentFrame + 1) % 2;

        m_sprite.setTextureRect(
            sf::IntRect(m_currentFrame * 16, 0, 16, 16)
        );
    }

    m_movementStrategy->Update(
        m_sprite,
        m_speed,
        timePerFrame
    );
}

void Goomba::Render(sf::RenderWindow& window) const
{
    if (m_active)
    {
        window.draw(m_sprite);
    }
}

sf::FloatRect Goomba::GetBounds() const
{
    return m_sprite.getGlobalBounds();
}

bool Goomba::IsActive() const
{
    return m_active;
}

void Goomba::Deactivate()
{
    m_active = false;
}