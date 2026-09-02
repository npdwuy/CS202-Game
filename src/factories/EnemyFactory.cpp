#include "factories/EnemyFactory.hpp"

#include "entities/enemies/BossEnemy.hpp"
#include "entities/enemies/FlyingEnemy.hpp"
#include "entities/enemies/Goomba.hpp"
#include "entities/enemies/HammerBro.hpp"
#include "entities/enemies/Koopa.hpp"
#include "levels/TileMap.hpp"

#include "entities/strategies/BossChaseStrategy.hpp"
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
    float enemyWidth = tileSize;
    float enemyHeight = tileSize;
    if (symbol == 'B') {
        enemyWidth = 85.f * 1.6f;
        enemyHeight = 65.f * 1.6f;
    } else if (symbol == 'H') {
        enemyWidth = 51.f * 0.7f;
        enemyHeight = 75.f * 0.7f;
    } else if (symbol == 'h') {
        enemyWidth = 75.f * 0.8f;
        enemyHeight = 97.f * 0.8f;
    }

    float minimumX = std::max(0.f, position.x - tileSize * 5.f);
    float maximumX = std::min(std::max(0.f, levelWidth - enemyWidth), position.x + enemyWidth + tileSize * 5.f);

    if (symbol == 'G' || symbol == 'K' || symbol == 'H' || symbol == 'h' || symbol == 'B') {
        // Multi-point vertical scan: foot (ground probe), bottom body, middle body, and head
        const float footY = position.y + tileSize + 2.f;
        const float bottomBodyY = position.y + tileSize - 4.f;
        const float midY = position.y + tileSize - enemyHeight * 0.5f;
        const float topY = position.y + tileSize - enemyHeight + 4.f;
        const float step = 8.f;

        auto isBlockedAt = [&](float x) {
            return tileMap.isSolidAt(sf::Vector2f(x, bottomBodyY)) ||
                   tileMap.isSolidAt(sf::Vector2f(x, midY)) ||
                   tileMap.isSolidAt(sf::Vector2f(x, topY));
        };

        auto hasGroundAt = [&](float x) {
            return tileMap.isSolidAt(sf::Vector2f(x, footY));
        };

        // Quét sang trái
        float leftScan = position.x;
        while (leftScan >= 0.f) {
            float nextLeft = leftScan - step;
            if (isBlockedAt(nextLeft) || !hasGroundAt(nextLeft)) {
                break;
            }
            leftScan = nextLeft;
        }
        minimumX = leftScan;

        // Quét sang phải (tính từ mép phải của quái vật)
        float rightScan = position.x + enemyWidth;
        while (rightScan <= levelWidth) {
            float nextRight = rightScan + step;
            if (isBlockedAt(nextRight) || !hasGroundAt(nextRight)) {
                break;
            }
            rightScan = nextRight;
        }
        maximumX = std::max(minimumX + enemyWidth, rightScan);
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

        case 'B':
            return std::make_unique<BossEnemy>(
                sf::Vector2f(
                    position.x + tileSize * 0.5f,
                    position.y + tileSize
                ),
                25.f,
                std::make_unique<BossChaseStrategy>(
                    minimumX,
                    maximumX
                )
            );

        case 'H':
            return std::make_unique<HammerBro>(
                sf::Vector2f(
                    position.x + tileSize * 0.5f,
                    position.y + tileSize
                ),
                30.f,  // Small HammerBro - tốc độ vừa
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
                sf::Vector2f(
                    position.x + tileSize * 0.5f,
                    position.y + tileSize
                ),
                18.f,  // Big HammerBro - tốc độ chậm hơn
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