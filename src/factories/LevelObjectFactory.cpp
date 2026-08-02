#include "factories/LevelObjectFactory.hpp"

#include "factories/EnemyFactory.hpp"

#include "entities/items/Coin.hpp"
#include "entities/items/PowerUpPickup.hpp"

#if __has_include("entities/items/Mushroom.hpp")
#include "entities/items/Mushroom.hpp"
#endif

#include <stdexcept>
#include <string>

std::unique_ptr<Enemy> LevelObjectFactory::createEnemy(
    char symbol,
    sf::Vector2f position,
    float tileSize,
    float levelWidth
) const
{
    return EnemyFactory::Create(
        symbol,
        position,
        tileSize,
        levelWidth
    );
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

bool LevelObjectFactory::isEnemySymbol(char symbol)
{
    return symbol == 'G'
        || symbol == 'K'
        || symbol == 'E'
        || symbol == 'B';
}

bool LevelObjectFactory::isItemSymbol(char symbol) {
    return symbol == 'C' || symbol == 'M' || symbol == 'F';
}
