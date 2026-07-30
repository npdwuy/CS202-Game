#pragma once

enum class ItemEffectType {
    AddScore,
    GrowPlayer,
    EnableFirePower
};

struct ItemEffect {
    ItemEffectType type;
    int amount;
};