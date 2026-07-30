#pragma once

#include <string>

struct SaveData {
    int version = 1;
    int currentLevel = 1;
    int score = 0;
    int remainingLives = 3;
    std::string selectedCharacter = "Mario";
    bool hasPlayerPosition = false;
    float playerX = 0.f;
    float playerY = 0.f;
    std::string powerUpState = "None";
};
