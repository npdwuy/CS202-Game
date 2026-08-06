#pragma once

enum class ItemEffectType {
    AddScore,
    GrowPlayer,
    EnableFirePower,
    ExtraLife,
    Invincibility,
    SpeedBoost
};

struct ItemEffect {
    ItemEffectType type;
    int amount;
};
