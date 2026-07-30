#pragma once

#include "persistence/SaveData.hpp"

#include <string>

class SaveManager {
public:
    static bool save(
        const SaveData& data,
        const std::string& path = "savegame.txt"
    );
};
