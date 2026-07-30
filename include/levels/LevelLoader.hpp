#pragma once

#include "levels/LevelData.hpp"

#include <string>

class LevelLoader {
public:
    static LevelData loadFromFile(const std::string& path);
    static bool isSupportedSymbol(char symbol);
};
