#pragma once

#include "persistence/SaveData.hpp"

#include <optional>
#include <string>

class LoadManager {
public:
    static std::optional<SaveData> load(
        const std::string& path = "savegame.txt"
    );
};
