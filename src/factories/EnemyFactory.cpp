#include "factories/EnemyFactory.hpp"

#include "entities/enemies/FlyingEnemy.hpp"
#include "entities/enemies/Goomba.hpp"
#include "entities/enemies/Koopa.hpp"

#include "entities/strategies/FlyingStrategy.hpp"
#include "entities/strategies/PatrolStrategy.hpp"

#include <stdexcept>

std::unique_ptr<Enemy> EnemyFactory::Create(
    char symbol,
    sf::Vector2f position
)
{
    switch (symbol)
    {
        case 'G':
            return std::make_unique<Goomba>(
                position,
                120.f,
                std::make_unique<PatrolStrategy>(
                    position.x - 100.f,
                    position.x + 400.f
                )
            );

        case 'K':
            return std::make_unique<Koopa>(
                position,
                80.f,
                std::make_unique<PatrolStrategy>(
                    position.x - 100.f,
                    position.x + 400.f
                )
            );

        case 'E':
            return std::make_unique<FlyingEnemy>(
                position,
                100.f,
                std::make_unique<FlyingStrategy>(
                    position.y - 150.f,
                    position.y + 300.f
                )
            );

        default:
            throw std::invalid_argument(
                "Unsupported enemy symbol."
            );
    }
}