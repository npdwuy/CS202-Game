#pragma once

#include "events/GameEvent.hpp"
#include "events/GameEventListener.hpp"

#include <vector>

class GameEventManager {
public:
    static GameEventManager& GetInstance();

    GameEventManager(const GameEventManager&) = delete;
    GameEventManager& operator=(
        const GameEventManager&
    ) = delete;

    void AddListener(GameEventListener* listener);
    void RemoveListener(GameEventListener* listener);

    void Notify(const GameEvent& event);

private:
    GameEventManager() = default;

    std::vector<GameEventListener*> m_listeners;
};