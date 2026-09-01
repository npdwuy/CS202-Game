#include "factories/EnemyFactory.hpp"

#include "entities/enemies/BossEnemy.hpp"
#include "entities/enemies/FlyingEnemy.hpp"
#include "entities/enemies/Goomba.hpp"
#include "entities/enemies/HammerBro.hpp"
#include "entities/enemies/Koopa.hpp"
#include "levels/TileMap.hpp"

#include "entities/strategies/ChaseStrategy.hpp"
#include "entities/strategies/FlyingStrategy.hpp"
#include "entities/strategies/PatrolStrategy.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>

std::unique_ptr<Enemy> EnemyFactory::Create(
    char symbol,
    sf::Vector2f position,
    float tileSize,
    float levelWidth,
    const TileMap& tileMap
)
{
    float minimumX = std::max(0.f, position.x - tileSize * 2.f);
    float maximumX = std::min(std::max(0.f, levelWidth - tileSize), position.x + tileSize * 2.f);

    if (symbol == 'G' || symbol == 'K' || symbol == 'H' || symbol == 'h') {
        // Scan left and right to find exact platform bounds
        float leftScan = position.x;
        const float footY = position.y + tileSize + 2.f;
        const float bodyY = position.y + tileSize * 0.5f;
        const float step = 10.f;

        // Quét sang trái
        while (leftScan >= 0.f) {
            float nextLeft = leftScan - step;
            if (tileMap.isSolidAt(sf::Vector2f(nextLeft, bodyY)) || 
                !tileMap.isSolidAt(sf::Vector2f(nextLeft, footY))) {
                break;
            }
            leftScan = nextLeft;
        }
        minimumX = leftScan;

        // Quét sang phải (tính từ mép phải của quái vật)
        float rightScan = position.x + tileSize;
        while (rightScan <= levelWidth) {
            float nextRight = rightScan + step;
            if (tileMap.isSolidAt(sf::Vector2f(nextRight, bodyY)) || 
                !tileMap.isSolidAt(sf::Vector2f(nextRight, footY))) {
                break;
            }
            rightScan = nextRight;
        }
        maximumX = std::max(minimumX, rightScan - tileSize);
    }

    switch (symbol)
    {
        case 'G':
            return std::make_unique<Goomba>(
                position,
                70.f,
                std::make_unique<ChaseStrategy>(
                    minimumX,
                    maximumX,
                    280.f,  // aggro radius: 280 px ≈ ~5.8 tiles
                    true    // Goomba sprite sheet faces LEFT by default
                )
            );

        case 'K':
            return std::make_unique<Koopa>(
                position,
                55.f,
                std::make_unique<ChaseStrategy>(
                    minimumX,
                    maximumX,
                    320.f,  // Koopa has slightly longer aggro range
                    false   // Koopa sprite sheet faces RIGHT by default
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

        case 'Z':
            return std::make_unique<BossEnemy>(
                sf::Vector2f(
                    position.x + tileSize * 0.5f, 
                    position.y + tileSize         
                ),
                minimumX, 
                maximumX, 
                25.f      
            );

        case 'H':
            return std::make_unique<HammerBro>(
                position,
                50.f,
                std::make_unique<ChaseStrategy>(
                    minimumX,
                    maximumX,
                    280.f,  // aggro radius
                    true    // HammerBro sprite faces LEFT by default
                ),
                false // Small HammerBro
            );

        case 'h':
            return std::make_unique<HammerBro>(
                position,
                50.f,
                std::make_unique<ChaseStrategy>(
                    minimumX,
                    maximumX,
                    280.f,  // aggro radius
                    true    // HammerBro sprite faces LEFT by default
                ),
                true // Big HammerBro
            );

        default:
            throw std::invalid_argument(
                "Unsupported enemy map symbol: " +
                std::string(1, symbol)
            );
    }
}