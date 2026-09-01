#include "levels/LevelLoader.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <stdexcept>
#include <string>

namespace {

std::string trim(std::string value) {
    const auto first = std::find_if_not(
        value.begin(),
        value.end(),
        [](unsigned char ch) { return std::isspace(ch) != 0; }
    );

    const auto last = std::find_if_not(
        value.rbegin(),
        value.rend(),
        [](unsigned char ch) { return std::isspace(ch) != 0; }
    ).base();

    if (first >= last) {
        return {};
    }

    return std::string(first, last);
}

void parseMetadata(LevelData& level, const std::string& line) {
    const std::size_t equalPosition = line.find('=');
    if (equalPosition == std::string::npos) {
        throw std::runtime_error("Invalid level metadata line: " + line);
    }

    const std::string key = trim(line.substr(1, equalPosition - 1));
    const std::string value = trim(line.substr(equalPosition + 1));

    if (key == "name") {
        if (value.empty()) {
            throw std::runtime_error("Level name cannot be empty.");
        }
        level.name = value;
    } else if (key == "difficulty") {
        if (value.empty()) {
            throw std::runtime_error("Level difficulty cannot be empty.");
        }
        level.difficulty = value;
    } else if (key == "tile_size") {
        try {
            const int parsedSize = std::stoi(value);
            if (parsedSize < 16 || parsedSize > 128) {
                throw std::runtime_error(
                    "Level tile_size must be between 16 and 128."
                );
            }
            level.tileSize = static_cast<unsigned int>(parsedSize);
        } catch (const std::invalid_argument&) {
            throw std::runtime_error("Invalid tile_size value: " + value);
        } catch (const std::out_of_range&) {
            throw std::runtime_error("tile_size value is out of range: " + value);
        }
    } else {
        throw std::runtime_error("Unknown level metadata key: " + key);
    }
}

}

LevelData LevelLoader::loadFromFile(const std::string& path) {
    std::ifstream input(path);
    if (!input.is_open()) {
        throw std::runtime_error("Failed to open level file: " + path);
    }

    LevelData level;
    bool readingMap = false;
    std::string line;

    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (!readingMap) {
            const std::string stripped = trim(line);
            if (stripped.empty() || stripped.front() == ';') {
                continue;
            }

            if (stripped == "@map") {
                readingMap = true;
                continue;
            }

            if (stripped.front() != '@') {
                throw std::runtime_error(
                    "Expected metadata or @map before level rows in: " + path
                );
            }

            parseMetadata(level, stripped);
            continue;
        }

        if (line.empty()) {
            continue;
        }

        level.rows.push_back(line);
    }

    if (!readingMap || level.rows.empty()) {
        throw std::runtime_error("Level file contains no @map data: " + path);
    }

    const std::size_t expectedWidth = level.rows.front().size();
    if (expectedWidth == 0U) {
        throw std::runtime_error("Level map width cannot be zero: " + path);
    }

    int playerCount = 0;
    int exitCount = 0;

    for (std::size_t row = 0; row < level.rows.size(); ++row) {
        if (level.rows[row].size() != expectedWidth) {
            throw std::runtime_error(
                "All map rows must have the same width in: " + path
            );
        }

        for (std::size_t column = 0; column < expectedWidth; ++column) {
            const char symbol = level.rows[row][column];
            if (!isSupportedSymbol(symbol)) {
                throw std::runtime_error(
                    "Unsupported map symbol '" + std::string(1, symbol) +
                    "' in: " + path
                );
            }

            const sf::Vector2f position{
                static_cast<float>(column * level.tileSize),
                static_cast<float>(row * level.tileSize)
            };

            if (symbol == 'P') {
                ++playerCount;
                level.playerStart = position;
            } else if (symbol == 'X') {
                ++exitCount;
                level.exitPosition = position;
            } else if (symbol != '#' && symbol != '.' && symbol != '?' && symbol != '!') {
                level.spawnRequests.push_back({symbol, position});
            }
        }
    }

    if (playerCount != 1) {
        throw std::runtime_error(
            "A level must contain exactly one player start symbol P: " + path
        );
    }

    if (exitCount > 1) {
        throw std::runtime_error(
            "A level must contain at most one exit symbol X: " + path
        );
    }
    level.hasExit = (exitCount == 1);

    return level;
}

bool LevelLoader::isSupportedSymbol(char symbol) {
    const std::string supportedSymbols = "#.PCMFGKEBZXLSV?!TD=Hh";
    return supportedSymbols.find(symbol) != std::string::npos;
}
