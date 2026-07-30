#include "factories/ItemFactory.hpp"

#include "entities/items/Coin.hpp"
#include "entities/items/FireFlower.hpp"
#include "entities/items/Mushroom.hpp"

#include <stdexcept>

std::unique_ptr<Item> ItemFactory::Create(
    char symbol,
    sf::Vector2f position
)
{
    switch (symbol)
    {
        case 'C':
            return std::make_unique<Coin>(
                position,
                100
            );

        case 'M':
            return std::make_unique<Mushroom>(
                position
            );

        case 'F':
            return std::make_unique<FireFlower>(
                position
            );

        default:
            throw std::invalid_argument(
                "Unsupported item symbol."
            );
    }
}