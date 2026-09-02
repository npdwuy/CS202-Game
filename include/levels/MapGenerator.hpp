#pragma once
#include <string>

class MapGenerator {
public:
    static void generateMap(int level, const std::string& outputPath);
    static void generateSubLevel(const std::string& outputPath);
    static void generateSubLevel(const std::string& difficulty, const std::string& outputPath);
};
