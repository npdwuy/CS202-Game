#pragma once

#include <string>

enum class GameEventType {
    CoinCollected,
    PowerUpCollected,
    EnemyDefeated,
    PlayerDamaged,
    LevelCompleted
};

struct GameEvent {
    GameEventType type;
    int value = 0;
    std::string data;
};