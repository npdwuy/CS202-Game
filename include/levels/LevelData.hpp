#pragma once

#include <SFML/System/Vector2.hpp>

#include <cstddef>
#include <string>
#include <vector>

struct LevelSpawnRequest {
    char symbol = '.';
    sf::Vector2f position{};
};

struct LevelData {
    std::string name = "Untitled Level";
    std::string difficulty = "Unknown";
    unsigned int tileSize = 48;
    std::vector<std::string> rows;
    std::vector<LevelSpawnRequest> spawnRequests;
    sf::Vector2f playerStart{};
    sf::Vector2f exitPosition{};

    std::size_t widthInTiles() const {
        return rows.empty() ? 0U : rows.front().size();
    }

    std::size_t heightInTiles() const {
        return rows.size();
    }

    sf::Vector2f worldSize() const {
        return {
            static_cast<float>(widthInTiles() * tileSize),
            static_cast<float>(heightInTiles() * tileSize)
        };
    }
};
