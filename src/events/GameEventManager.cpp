#include "events/GameEventManager.hpp"

#include <algorithm>

GameEventManager& GameEventManager::GetInstance()
{
    static GameEventManager instance;
    return instance;
}

void GameEventManager::AddListener(
    GameEventListener* listener
)
{
    if (listener == nullptr)
    {
        return;
    }

    const auto found = std::find(
        m_listeners.begin(),
        m_listeners.end(),
        listener
    );

    if (found == m_listeners.end())
    {
        m_listeners.push_back(listener);
    }
}

void GameEventManager::RemoveListener(
    GameEventListener* listener
)
{
    m_listeners.erase(
        std::remove(
            m_listeners.begin(),
            m_listeners.end(),
            listener
        ),
        m_listeners.end()
    );
}

void GameEventManager::Notify(
    const GameEvent& event
)
{
    const auto listeners = m_listeners;

    for (GameEventListener* listener : listeners)
    {
        if (listener != nullptr)
        {
            listener->OnGameEvent(event);
        }
    }
}