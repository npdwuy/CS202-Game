#include "persistence/SaveManager.hpp"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <system_error>

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

bool isValid(const SaveData& data) {
    return (
        data.version == 1 &&
        data.currentLevel >= 1 &&
        data.currentLevel <= 3 &&
        data.score >= 0 &&
        data.remainingLives >= 0 &&
        isSupportedCharacter(data.selectedCharacter) &&
        isSupportedPowerUp(data.powerUpState) &&
        std::isfinite(data.playerX) &&
        std::isfinite(data.playerY)
    );
}

}

bool SaveManager::save(const SaveData& data, const std::string& path) {
    if (!isValid(data)) {
        std::cerr << "Refusing to save invalid game data.\n";
        return false;
    }

    const std::filesystem::path savePath(path);
    if (!savePath.parent_path().empty()) {
        std::error_code directoryError;
        std::filesystem::create_directories(
            savePath.parent_path(),
            directoryError
        );
        if (directoryError) {
            std::cerr << "Failed to create save directory: "
                      << directoryError.message() << '\n';
            return false;
        }
    }

    std::filesystem::path temporaryPath = savePath;
    temporaryPath += ".tmp";

    std::ofstream output(temporaryPath, std::ios::trunc);
    if (!output.is_open()) {
        std::cerr << "Failed to open temporary save file: "
                  << temporaryPath.string() << '\n';
        return false;
    }

    output << "version=" << data.version << '\n';
    output << "currentLevel=" << data.currentLevel << '\n';
    output << "score=" << data.score << '\n';
    output << "remainingLives=" << data.remainingLives << '\n';
    output << "selectedCharacter=" << data.selectedCharacter << '\n';
    output << "hasPlayerPosition="
           << (data.hasPlayerPosition ? "true" : "false") << '\n';
    output << "playerX=" << data.playerX << '\n';
    output << "playerY=" << data.playerY << '\n';
    output << "powerUpState=" << data.powerUpState << '\n';
    output.flush();

    if (!output.good()) {
        std::cerr << "Failed while writing save data.\n";
        output.close();
        std::error_code removeError;
        std::filesystem::remove(temporaryPath, removeError);
        return false;
    }

    output.close();

    std::filesystem::path backupPath = savePath;
    backupPath += ".bak";

    std::error_code fileError;
    std::filesystem::remove(backupPath, fileError);
    fileError.clear();

    const bool hadExistingSave = std::filesystem::exists(savePath, fileError);
    if (fileError) {
        std::cerr << "Failed to inspect existing save file: "
                  << fileError.message() << '\n';
        std::filesystem::remove(temporaryPath, fileError);
        return false;
    }

    if (hadExistingSave) {
        std::filesystem::rename(savePath, backupPath, fileError);
        if (fileError) {
            std::cerr << "Failed to preserve existing save file: "
                      << fileError.message() << '\n';
            std::error_code removeError;
            std::filesystem::remove(temporaryPath, removeError);
            return false;
        }
    }

    std::filesystem::rename(temporaryPath, savePath, fileError);

    if (fileError) {
        std::cerr << "Failed to finalize save file: "
                  << fileError.message() << '\n';

        if (hadExistingSave) {
            std::error_code restoreError;
            std::filesystem::rename(backupPath, savePath, restoreError);
            if (restoreError) {
                std::cerr << "Failed to restore the previous save file: "
                          << restoreError.message() << '\n';
            }
        }

        std::error_code removeError;
        std::filesystem::remove(temporaryPath, removeError);
        return false;
    }

    if (hadExistingSave) {
        std::error_code removeError;
        std::filesystem::remove(backupPath, removeError);
    }

    return true;
}
