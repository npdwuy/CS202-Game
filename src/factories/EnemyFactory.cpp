#include "factories/EnemyFactory.hpp"

#include "entities/enemies/BossEnemy.hpp"
#include "entities/enemies/FlyingEnemy.hpp"
#include "entities/enemies/Goomba.hpp"
#include "entities/enemies/Koopa.hpp"

#include "entities/strategies/FlyingStrategy.hpp"
#include "entities/strategies/PatrolStrategy.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>

std::unique_ptr<Enemy> EnemyFactory::Create(
    char symbol,
    sf::Vector2f position,
    float tileSize,
    float levelWidth
)
{
    const float minimumX = std::max(
        0.f,
        position.x - tileSize * 2.f
    );

    const float maximumX = std::min(
        std::max(0.f, levelWidth - tileSize),
        position.x + tileSize * 2.f
    );

    switch (symbol)
    {
        case 'G':
            return std::make_unique<Goomba>(
                position,
                70.f,
                std::make_unique<PatrolStrategy>(
                    minimumX,
                    maximumX
                )
            );

        case 'K':
            return std::make_unique<Koopa>(
                position,
                55.f,
                std::make_unique<PatrolStrategy>(
                    minimumX,
                    maximumX
                )
            );

        case 'E':
            return std::make_unique<FlyingEnemy>(
                position,
                80.f,
                std::make_unique<FlyingStrategy>(
                    std::max(0.f, position.y - tileSize * 3.f),
                    position.y + tileSize * 4.f
                )
            );

        case 'B':
            return std::make_unique<BossEnemy>(
                sf::Vector2f(
                    position.x,
                    position.y - tileSize * 0.5f
                ),
                minimumX,
                maximumX,
                45.f
            );

        default:
            throw std::invalid_argument(
                "Unsupported enemy map symbol: " +
                std::string(1, symbol)
            );
    }
}