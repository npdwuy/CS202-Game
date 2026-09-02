#include "persistence/LoadManager.hpp"

#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>

namespace {

bool isSupportedCharacter(const std::string& character) {
    return character == "Mario" || character == "Luigi";
}

bool isSupportedPowerUp(const std::string& powerUp) {
    return (
        powerUp == "None" ||
        powerUp == "Mushroom" ||
        powerUp == "FireFlower"
    );
}

bool parseBoolean(const std::string& value) {
    if (value == "true") {
        return true;
    }
    if (value == "false") {
        return false;
    }
    throw std::runtime_error("Expected true or false, received: " + value);
}

}

std::optional<SaveData> LoadManager::load(const std::string& path) {
    std::ifstream input(path);
    if (!input.is_open()) {
        return std::nullopt;
    }

    std::map<std::string, std::string> values;
    std::string line;

    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line.empty() || line.front() == ';') {
            continue;
        }

        const std::size_t equalPosition = line.find('=');
        if (equalPosition == std::string::npos) {
            std::cerr << "Ignoring malformed save line: " << line << '\n';
            continue;
        }

        values[line.substr(0, equalPosition)] = line.substr(equalPosition + 1);
    }

    try {
        SaveData data;
        data.version = std::stoi(values.at("version"));
        data.currentLevel = std::stoi(values.at("currentLevel"));
        
        auto highestLevelIt = values.find("highestUnlockedLevel");
        if (highestLevelIt != values.end()) {
            data.highestUnlockedLevel = std::stoi(highestLevelIt->second);
        } else {
            data.highestUnlockedLevel = 1;
        }

        data.score = std::stoi(values.at("score"));
        data.remainingLives = std::stoi(values.at("remainingLives"));
        data.selectedCharacter = values.at("selectedCharacter");
        data.hasPlayerPosition = parseBoolean(values.at("hasPlayerPosition"));
        data.playerX = std::stof(values.at("playerX"));
        data.playerY = std::stof(values.at("playerY"));
        data.powerUpState = values.at("powerUpState");

        auto timeIt = values.find("remainingTime");
        if (timeIt != values.end()) {
            data.remainingTime = std::stof(timeIt->second);
        } else {
            data.remainingTime = 400.f;
        }

        auto coinsIt = values.find("coins");
        if (coinsIt != values.end()) {
            data.coins = std::stoi(coinsIt->second);
        } else {
            data.coins = 0;
        }

        if (data.version != 1) {
            throw std::runtime_error("Unsupported save version.");
        }
        if (data.currentLevel < 1 || data.currentLevel > 5) {
            throw std::runtime_error("Saved level is outside the valid range.");
        }
        if (data.highestUnlockedLevel < 1 || data.highestUnlockedLevel > 3) {
            throw std::runtime_error("Saved highest unlocked level is outside the valid range.");
        }
        if (data.score < 0 || data.remainingLives < 0) {
            throw std::runtime_error("Score and lives cannot be negative.");
        }
        if (!isSupportedCharacter(data.selectedCharacter)) {
            throw std::runtime_error("Unsupported saved character.");
        }
        if (!isSupportedPowerUp(data.powerUpState)) {
            throw std::runtime_error("Unsupported saved power-up.");
        }
        if (!std::isfinite(data.playerX) || !std::isfinite(data.playerY)) {
            throw std::runtime_error("Saved player position is not finite.");
        }
        if (!std::isfinite(data.remainingTime) || data.remainingTime < 0.f) {
            throw std::runtime_error("Saved remaining time is invalid.");
        }
        if (data.coins < 0) {
            throw std::runtime_error("Saved coins is invalid.");
        }

        return data;
    } catch (const std::exception& error) {
        std::cerr << "Failed to load save file '" << path
                  << "': " << error.what() << '\n';
        return std::nullopt;
    }
}
