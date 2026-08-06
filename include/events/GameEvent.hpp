#pragma once

#include <string>

enum class GameEventType {
    CoinCollected,
    PowerUpCollected,
    ExtraLifeCollected,
    InvincibilityCollected,
    SpeedBoostCollected,
    EnemyDefeated,
    PlayerDamaged,
    PlayerFell,
    LevelCompleted
};

struct GameEvent {
    GameEventType type;
    int value = 0;
    std::string data;
};
