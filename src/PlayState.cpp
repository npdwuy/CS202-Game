#include "PlayState.hpp"

#include "entities/enemies/Goomba.hpp"
#include "entities/items/Coin.hpp"

PlayState::PlayState()
{
    m_enemies.push_back(
        std::make_unique<Goomba>(
            sf::Vector2f(300.f, 850.f),
            120.f,
            250.f,
            900.f
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
    (void)event;
}

void PlayState::Update(sf::Time timePerFrame)
{
    (void)timePerFrame;
}

void PlayState::Render(sf::RenderWindow& window)
{
    (void)window;
}