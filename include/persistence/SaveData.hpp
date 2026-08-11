#pragma once

#include <string>

struct SaveData {
    int version = 1;
    int currentLevel = 1;
    int score = 0;
    int remainingLives = 10;
    std::string selectedCharacter = "Mario";
    bool hasPlayerPosition = false;
    float playerX = 0.f;
    float playerY = 0.f;
    std::string powerUpState = "None";
    float remainingTime = 400.f;
    int coins = 0;
};
