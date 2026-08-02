#pragma once

#include "events/GameEvent.hpp"

class GameEventListener {
public:
    virtual ~GameEventListener() = default;

    virtual void OnGameEvent(
        const GameEvent& event
    ) = 0;
};