#include "PlayState.hpp"

#include "entities/enemies/Goomba.hpp"
#include "entities/enemies/Koopa.hpp"
#include "entities/items/Coin.hpp"
#include "entities/strategies/PatrolStrategy.hpp"

#include <algorithm>

PlayState::PlayState()
{
    m_enemies.push_back(
        std::make_unique<Goomba>(
            sf::Vector2f(300.f, 850.f),
            120.f,
            std::make_unique<PatrolStrategy>(
                250.f,
                900.f
            )
        )
    );

    m_enemies.push_back(
        std::make_unique<Koopa>(
            sf::Vector2f(700.f, 850.f),
            80.f,
            std::make_unique<PatrolStrategy>(
                600.f,
                1150.f
            )
        )
    );

    m_items.push_back(
        std::make_unique<Coin>(
            sf::Vector2f(700.f, 700.f),
            1
        )
    );
}

void PlayState::Input(const sf::Event& event)
{
    if (event.type != sf::Event::KeyPressed)
    {
        return;
    }

    if (event.key.code == sf::Keyboard::C && !m_items.empty())
    {
        m_items.front()->Collect();
    }
    else if (event.key.code == sf::Keyboard::K && !m_enemies.empty())
    {
        m_enemies.front()->Deactivate();
    }
}

void PlayState::Update(sf::Time timePerFrame)
{
    for (auto& enemy : m_enemies)
    {
        enemy->Update(timePerFrame);
    }

    for (auto& item : m_items)
    {
        item->Update(timePerFrame);
    }

    m_enemies.erase(
        std::remove_if(
            m_enemies.begin(),
            m_enemies.end(),
            [](const std::unique_ptr<Enemy>& enemy)
            {
                return !enemy->IsActive();
            }
        ),
        m_enemies.end()
    );

    m_items.erase(
        std::remove_if(
            m_items.begin(),
            m_items.end(),
            [](const std::unique_ptr<Item>& item)
            {
                return item->IsCollected();
            }
        ),
        m_items.end()
    );
}

void PlayState::Render(sf::RenderWindow& window)
{
    for (const auto& enemy : m_enemies)
    {
        enemy->Render(window);
    }

    for (const auto& item : m_items)
    {
        item->Render(window);
    }
}