#include "entities/enemies/Koopa.hpp"

#include "levels/TileMap.hpp"
#include "resources/ResourceManager.hpp"

#include <algorithm>
#include <cmath>
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
      m_active(true),
      m_state(State::Walking),
      m_shellSpeed(450.f),
      m_shellDirection(1),
      m_shellVerticalVelocity(0.f),
      m_shellKickDelay(0.f),
      m_flung(false),
      m_velocity(0.f, 0.f)
{
    if (!m_movementStrategy)
    {
        throw std::invalid_argument(
            "Koopa requires a movement strategy."
        );
    }

    m_sprite.setTexture(
        ResourceManager::getInstance().getTexture(
            "assets/sprites/enemies/koopa_walk.png"
        )
    );

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

    const float dt = timePerFrame.asSeconds();

    // =========================================================
    // SHELL KICK DELAY
    // =========================================================
    if (m_shellKickDelay > 0.f)
    {
        m_shellKickDelay -= dt;

        if (m_shellKickDelay < 0.f)
        {
            m_shellKickDelay = 0.f;
        }
    }

    // =========================================================
    // FLUNG STATE
    // =========================================================
    if (m_flung)
    {
        m_velocity.y += 2000.f * dt;

        m_sprite.move(
            m_velocity * dt
        );

        if (m_sprite.getPosition().y > 2000.f)
        {
            m_active = false;
        }

        return;
    }

    // =========================================================
    // IDLE SHELL
    //
    // Horizontal movement is disabled.
    // Gravity is handled later by UpdateShellPhysics().
    // =========================================================
    if (m_state == State::ShellIdle)
    {
        return;
    }

    // =========================================================
    // MOVING SHELL
    //
    // Horizontal movement + gravity are handled by
    // UpdateShellPhysics().
    //
    // This section only handles shell animation.
    // =========================================================
    if (m_state == State::ShellMoving)
    {
        m_animationTime += dt;

        const float shellFrameDuration = 0.08f;

        if (m_animationTime >= shellFrameDuration)
        {
            m_animationTime = 0.f;

            m_currentFrame =
                (m_currentFrame + 1) % 4;

            m_sprite.setTextureRect(
                sf::IntRect(
                    m_currentFrame * 48,
                    0,
                    48,
                    48
                )
            );
        }

        return;
    }

    // =========================================================
    // NORMAL WALKING KOOPA
    // =========================================================
    m_animationTime += dt;

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

void Koopa::UpdateShellPhysics(
    sf::Time timePerFrame,
    const TileMap& tileMap
)
{
    if (
        !m_active ||
        m_flung ||
        (
            m_state != State::ShellIdle &&
            m_state != State::ShellMoving
        )
    )
    {
        return;
    }

    const float dt = timePerFrame.asSeconds();

    if (dt <= 0.f)
    {
        return;
    }

    constexpr float gravity = 2000.f;
    constexpr float terminalVelocity = 1800.f;

    // =========================================================
    // HORIZONTAL SHELL MOVEMENT
    // =========================================================
    if (m_state == State::ShellMoving)
    {
        const float dx =
            m_shellSpeed *
            static_cast<float>(m_shellDirection) *
            dt;

        const sf::FloatRect currentBounds =
            m_sprite.getGlobalBounds();

        sf::FloatRect nextHorizontal(
            currentBounds.left + dx,
            currentBounds.top + 5.f,
            currentBounds.width,
            std::max(
                1.f,
                currentBounds.height - 10.f
            )
        );

        const sf::FloatRect world =
            tileMap.worldBounds();

        const bool hitsWorldEdge =
            nextHorizontal.left < world.left ||
            nextHorizontal.left +
                    nextHorizontal.width >
                world.left + world.width;

        const bool hitsWall =
            tileMap.intersectsSolid(
                nextHorizontal
            );

        if (hitsWorldEdge || hitsWall)
        {
            // Shell hits a wall -> reverse direction.
            m_shellDirection *= -1;
        }
        else
        {
            m_sprite.move(
                dx,
                0.f
            );
        }
    }

    // =========================================================
    // CHECK FOR GROUND
    // =========================================================
    sf::FloatRect bounds =
        m_sprite.getGlobalBounds();

    const float footY =
        bounds.top +
        bounds.height +
        2.f;

    const float leftFoot =
        bounds.left + 5.f;

    const float centerFoot =
        bounds.left +
        bounds.width * 0.5f;

    const float rightFoot =
        bounds.left +
        bounds.width - 5.f;

    const bool hasGround =
        tileMap.isSolidAt(
            sf::Vector2f(
                leftFoot,
                footY
            )
        ) ||
        tileMap.isSolidAt(
            sf::Vector2f(
                centerFoot,
                footY
            )
        ) ||
        tileMap.isSolidAt(
            sf::Vector2f(
                rightFoot,
                footY
            )
        );

    // =========================================================
    // GRAVITY
    // =========================================================
    if (
        hasGround &&
        m_shellVerticalVelocity >= 0.f
    )
    {
        m_shellVerticalVelocity = 0.f;
    }
    else
    {
        m_shellVerticalVelocity +=
            gravity * dt;

        m_shellVerticalVelocity =
            std::min(
                m_shellVerticalVelocity,
                terminalVelocity
            );
    }

    // =========================================================
    // VERTICAL MOVEMENT
    // =========================================================
    if (m_shellVerticalVelocity != 0.f)
    {
        const float dy =
            m_shellVerticalVelocity * dt;

        bounds =
            m_sprite.getGlobalBounds();

        sf::FloatRect nextVertical(
            bounds.left + 4.f,
            bounds.top + dy,
            std::max(
                1.f,
                bounds.width - 8.f
            ),
            bounds.height
        );

        if (
            dy > 0.f &&
            tileMap.intersectsSolid(nextVertical)
        )
        {
            // -------------------------------------------------
            // Shell is falling onto a platform.
            //
            // Find the maximum safe vertical movement so the
            // shell lands directly above the tile instead of
            // entering it.
            // -------------------------------------------------
            float safeMove = 0.f;
            float collisionMove = dy;

            for (int i = 0; i < 10; ++i)
            {
                const float middle =
                    (safeMove + collisionMove)
                    * 0.5f;

                sf::FloatRect testBounds(
                    bounds.left + 4.f,
                    bounds.top + middle,
                    std::max(
                        1.f,
                        bounds.width - 8.f
                    ),
                    bounds.height
                );

                if (
                    tileMap.intersectsSolid(
                        testBounds
                    )
                )
                {
                    collisionMove = middle;
                }
                else
                {
                    safeMove = middle;
                }
            }

            m_sprite.move(
                0.f,
                safeMove
            );

            m_shellVerticalVelocity = 0.f;
        }
        else if (
            dy < 0.f &&
            tileMap.intersectsSolid(nextVertical)
        )
        {
            // Currently not normally used by the shell,
            // but prevents upward motion through blocks.
            m_shellVerticalVelocity = 0.f;
        }
        else
        {
            // No ground / obstacle -> continue falling.
            m_sprite.move(
                0.f,
                dy
            );
        }
    }

    // =========================================================
    // FALL OUT OF LEVEL
    // =========================================================
    if (
        m_sprite.getPosition().y >
        tileMap.worldBounds().height + 200.f
    )
    {
        m_active = false;
    }
}

void Koopa::Render(
    sf::RenderWindow& window
) const
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

// =============================================================
// ENTER SHELL
// =============================================================
void Koopa::EnterShell()
{
    if (
        !m_active ||
        m_state != State::Walking
    )
    {
        return;
    }

    m_state = State::ShellIdle;

    m_animationTime = 0.f;
    m_currentFrame = 0;

    // Prevent the same stomp from immediately
    // kicking the shell.
    m_shellKickDelay = 0.35f;

    m_shellVerticalVelocity = 0.f;

    m_sprite.setTexture(
        ResourceManager::getInstance().getTexture(
            "assets/sprites/enemies/koopa_shell.png"
        ),
        true
    );

    m_sprite.setTextureRect(
        sf::IntRect(
            0,
            0,
            48,
            48
        )
    );
}

// =============================================================
// KICK SHELL
// =============================================================
void Koopa::KickShell(int direction)
{
    if (
        !m_active ||
        !CanKickShell()
    )
    {
        return;
    }

    m_state = State::ShellMoving;

    m_shellDirection =
        direction >= 0
            ? 1
            : -1;

    m_shellVerticalVelocity = 0.f;

    m_animationTime = 0.f;
    m_currentFrame = 0;

    m_sprite.setTexture(
        ResourceManager::getInstance().getTexture(
            "assets/sprites/enemies/koopa_shell_move.png"
        ),
        true
    );

    m_sprite.setTextureRect(
        sf::IntRect(
            0,
            0,
            48,
            48
        )
    );
}

void Koopa::SetPlayerPosition(sf::Vector2f pos) {
    if (m_movementStrategy) {
        m_movementStrategy->setPlayerPosition(pos);
    }
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

bool Koopa::CanKickShell() const
{
    return m_state == State::ShellIdle && m_shellKickDelay <= 0.f;
}

// =============================================================
// FLING
// =============================================================
void Koopa::Fling()
{
    m_flung = true;
    m_state = State::Walking;
    m_shellVerticalVelocity = 0.f;
    m_velocity = {0.f, -500.f};

    m_sprite.setScale(
        m_sprite.getScale().x,
        -std::abs(m_sprite.getScale().y)
    );
}

bool Koopa::IsFlung() const
{
    return m_flung;
}
