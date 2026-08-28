#include "entities/enemies/Koopa.hpp"
#include "resources/ResourceManager.hpp"

#include <stdexcept>
#include <utility>
#include <cmath>

Koopa::Koopa(
    sf::Vector2f position,
    float speed,
    std::unique_ptr<MovementStrategy> movementStrategy
)
: m_movementStrategy(std::move(movementStrategy)),
  m_speed(speed),
  m_animationTime(0.f),
  m_currentFrame(0),
  m_active(true),
  m_state(State::Walking),
  m_shellSpeed(450.f),
  m_shellDirection(1),
  m_flung(false),
  m_velocity(0.f, 0.f)
{
    if (!m_movementStrategy)
    {
        throw std::invalid_argument(
            "Koopa requires a movement strategy."
        );
    }

    m_sprite.setTexture(ResourceManager::getInstance().getTexture(
        "assets/sprites/enemies/koopa_walk.png"
    ));
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

    if (m_flung)
    {
        m_velocity.y +=
            2000.f * timePerFrame.asSeconds();

        m_sprite.move(
            m_velocity * timePerFrame.asSeconds()
        );

        if (m_sprite.getPosition().y > 2000.f)
        {
            m_active = false;
        }

        return;
    }

    if (m_state == State::ShellIdle)
    {
        return;
    }

    if (m_state == State::ShellMoving)
    {
        const float distance =
            m_shellSpeed
            * static_cast<float>(m_shellDirection)
            * timePerFrame.asSeconds();

        m_sprite.move(distance, 0.f);

        return;
    }

    m_animationTime += timePerFrame.asSeconds();

    const float frameDuration = 0.22f;

    if (m_animationTime >= frameDuration)
    {
        m_animationTime = 0.f;
        m_currentFrame =
            (m_currentFrame + 1) % 2;

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

void Koopa::EnterShell()
{
    if (!m_active)
    {
        return;
    }

    m_state = State::ShellIdle;
    m_animationTime = 0.f;
    m_currentFrame = 0;
}

void Koopa::KickShell(int direction)
{
    if (!m_active)
    {
        return;
    }

    m_state = State::ShellMoving;

    m_shellDirection =
        direction >= 0 ? 1 : -1;
}

Koopa::State Koopa::GetState() const
{
    return m_state;
}

bool Koopa::IsWalking() const
{
    return m_state == State::Walking;
}

bool Koopa::IsShellIdle() const
{
    return m_state == State::ShellIdle;
}

bool Koopa::IsShellMoving() const
{
    return m_state == State::ShellMoving;
}

void Koopa::Fling()
{
    m_flung = true;
    m_state = State::Walking;

    m_velocity = {
        0.f,
        -500.f
    };

    m_sprite.setScale(
        m_sprite.getScale().x,
        -std::abs(m_sprite.getScale().y)
    );
}

bool Koopa::IsFlung() const {
    return m_flung;
}
