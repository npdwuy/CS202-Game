#include "factories/LevelObjectFactory.hpp"

#include "factories/EnemyFactory.hpp"

#include "factories/ItemFactory.hpp"

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
) const
{
    return ItemFactory::Create(
        symbol,
        position
    );
}

bool LevelObjectFactory::isEnemySymbol(char symbol)
{
    return symbol == 'G'
        || symbol == 'K'
        || symbol == 'E'
        || symbol == 'Z';
}

bool LevelObjectFactory::isItemSymbol(char symbol) {
    return symbol == 'C' || symbol == 'M' || symbol == 'F'
        || symbol == 'L' || symbol == 'S' || symbol == 'V';
}
