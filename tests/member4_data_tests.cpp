#include "levels/LevelLoader.hpp"
#include "persistence/LoadManager.hpp"
#include "persistence/SaveManager.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace {

bool require(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
    }
    return condition;
}

}

int main() {
    bool passed = true;

    for (int levelNumber = 1; levelNumber <= 3; ++levelNumber) {
        const LevelData level = LevelLoader::loadFromFile(
            "levels/level" + std::to_string(levelNumber) + ".txt"
        );

        passed &= require(level.widthInTiles() == 40U, "level width is 40");
        passed &= require(level.heightInTiles() == 20U, "level height is 20");
        passed &= require(!level.spawnRequests.empty(), "level has spawns");
        passed &= require(level.tileSize == 48U, "tile size is 48");
    }

    const std::filesystem::path invalidLevelPath =
        std::filesystem::temp_directory_path() /
        "cs202_member4_invalid_level_test.txt";
    {
        std::ofstream invalidLevel(invalidLevelPath, std::ios::trunc);
        invalidLevel
            << "@name=Invalid\n"
            << "@difficulty=Test\n"
            << "@tile_size=48\n"
            << "@map\n"
            << "P?X\n"
            << "###\n";
    }

    bool invalidLevelRejected = false;
    try {
        static_cast<void>(
            LevelLoader::loadFromFile(invalidLevelPath.string())
        );
    } catch (const std::exception&) {
        invalidLevelRejected = true;
    }
    passed &= require(
        invalidLevelRejected,
        "unsupported level symbols are rejected"
    );

    const std::filesystem::path savePath =
        std::filesystem::temp_directory_path() / "cs202_member4_save_test.txt";

    SaveData expected;
    expected.currentLevel = 2;
    expected.score = 2400;
    expected.remainingLives = 2;
    expected.selectedCharacter = "Luigi";
    expected.hasPlayerPosition = true;
    expected.playerX = 144.f;
    expected.playerY = 288.f;
    expected.powerUpState = "FireFlower";

    passed &= require(
        SaveManager::save(expected, savePath.string()),
        "valid save data is written"
    );

    const std::optional<SaveData> loaded =
        LoadManager::load(savePath.string());
    passed &= require(loaded.has_value(), "saved data can be loaded");
    if (loaded) {
        passed &= require(
            loaded->currentLevel == expected.currentLevel,
            "current level round-trips"
        );
        passed &= require(
            loaded->score == expected.score,
            "score round-trips"
        );
        passed &= require(
            loaded->remainingLives == expected.remainingLives,
            "lives round-trip"
        );
        passed &= require(
            loaded->selectedCharacter == expected.selectedCharacter,
            "character round-trips"
        );
        passed &= require(
            loaded->powerUpState == expected.powerUpState,
            "power-up round-trips"
        );
    }

    {
        std::ofstream corrupted(savePath, std::ios::trunc);
        corrupted
            << "version=999\n"
            << "currentLevel=1\n"
            << "score=0\n"
            << "remainingLives=3\n"
            << "selectedCharacter=Mario\n"
            << "hasPlayerPosition=false\n"
            << "playerX=0\n"
            << "playerY=0\n"
            << "powerUpState=None\n";
    }
    passed &= require(
        !LoadManager::load(savePath.string()).has_value(),
        "invalid save version is rejected"
    );

    std::error_code removeError;
    std::filesystem::remove(invalidLevelPath, removeError);
    removeError.clear();
    std::filesystem::remove(savePath, removeError);

    if (passed) {
        std::cout << "All Member 4 data tests passed.\n";
        return 0;
    }

    return 1;
}
