#include "entities/enemies/Goomba.hpp"
#include "resources/ResourceManager.hpp"

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

    m_sprite.setTexture(ResourceManager::getInstance().getTexture(
        "assets/sprites/enemies/goomba_walk.png"
    ));
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

    if (m_flung) {
        m_velocity.y += 2000.f * timePerFrame.asSeconds();
        m_sprite.move(m_velocity * timePerFrame.asSeconds());
        if (m_sprite.getPosition().y > 2000.f) { // offscreen
            m_active = false;
        }
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

void Goomba::Fling() {
    m_flung = true;
    m_velocity = {0.f, -500.f};
    m_sprite.setScale(m_sprite.getScale().x, -m_sprite.getScale().y);
}

bool Goomba::IsFlung() const {
    return m_flung;
}

void Goomba::SetPlayerPosition(sf::Vector2f pos) {
    m_movementStrategy->setPlayerPosition(pos);
}
