#include "factories/LevelObjectFactory.hpp"

#include "entities/enemies/BossEnemy.hpp"
#include "entities/enemies/Goomba.hpp"
#include "entities/enemies/Koopa.hpp"
#include "entities/items/Coin.hpp"
#include "entities/items/PowerUpPickup.hpp"
#include "entities/strategies/PatrolStrategy.hpp"

#if __has_include("entities/items/Mushroom.hpp")
#include "entities/items/Mushroom.hpp"
#endif

#include <algorithm>
#include <stdexcept>
#include <string>

std::unique_ptr<Enemy> LevelObjectFactory::createEnemy(
    char symbol,
    sf::Vector2f position,
    float tileSize,
    float levelWidth
) const {
    const float minimumX = std::max(0.f, position.x - tileSize * 2.f);
    const float maximumX = std::min(
        std::max(0.f, levelWidth - tileSize),
        position.x + tileSize * 2.f
    );

    switch (symbol) {
        case 'G':
            return std::make_unique<Goomba>(
                position,
                70.f,
                std::make_unique<PatrolStrategy>(minimumX, maximumX)
            );
        case 'K':
            return std::make_unique<Koopa>(
                position,
                55.f,
                std::make_unique<PatrolStrategy>(minimumX, maximumX)
            );
        case 'B':
            return std::make_unique<BossEnemy>(
                sf::Vector2f(position.x, position.y - tileSize * 0.5f),
                minimumX,
                maximumX,
                45.f
            );
        default:
            throw std::invalid_argument(
                "Unsupported enemy map symbol: " + std::string(1, symbol)
            );
    }
}

std::unique_ptr<Item> LevelObjectFactory::createItem(
    char symbol,
    sf::Vector2f position
) const {
    switch (symbol) {
        case 'C':
            return std::make_unique<Coin>(position, 100);
        case 'M':
#if __has_include("entities/items/Mushroom.hpp")
            return std::make_unique<Mushroom>(position);
#else
            return std::make_unique<PowerUpPickup>(
                position,
                PowerUpKind::Mushroom
            );
#endif
        case 'F':
            return std::make_unique<PowerUpPickup>(
                position,
                PowerUpKind::FireFlower
            );
        default:
            throw std::invalid_argument(
                "Unsupported item map symbol: " + std::string(1, symbol)
            );
    }
}

bool LevelObjectFactory::isEnemySymbol(char symbol) {
    return symbol == 'G' || symbol == 'K' || symbol == 'B';
}

bool LevelObjectFactory::isItemSymbol(char symbol) {
    return symbol == 'C' || symbol == 'M' || symbol == 'F';
}
