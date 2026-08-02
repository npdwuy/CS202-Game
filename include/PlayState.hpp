#pragma once

#include "GameState.hpp"
#include "factories/LevelObjectFactory.hpp"
#include "levels/TileMap.hpp"
#include "persistence/SaveData.hpp"
#include "entities/Enemy.hpp"
#include "entities/Item.hpp"
#include "entities/player/Player.hpp"
#include "events/GameEventListener.hpp"

#include <memory>
#include <string>
#include <vector>

class PlayState
    : public GameState,
      public GameEventListener
{
public:
    explicit PlayState(bool loadSavedGame = false);
    ~PlayState() override;

    void Input(const sf::Event& event) override;
    void Update(sf::Time timePerFrame) override;
    void Render(sf::RenderWindow& window) override;
    void OnGameEvent(const GameEvent& event) override;

private:
    void loadLevel(int levelNumber, bool restoreSavedPosition);
    void createLevelObjects();
    void saveGame();
    void loadGame();
    void handleItemCollisions();
    bool handleEnemyCollisions();
    void handlePlayerFall();
    void handleLevelExit();
    void loseLife();
    void restartGame();
    void updateHud();
    void showStatus(const std::string& message, float duration = 2.f);
    sf::FloatRect playerBounds() const;
    bool hasActiveBoss() const;
    static std::string levelPath(int levelNumber);

    TileMap m_tileMap;
    LevelObjectFactory m_objectFactory;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::vector<std::unique_ptr<Item>> m_items;
    std::unique_ptr<Player> m_player;

    SaveData m_saveData;
    const sf::Font* m_hudFont = nullptr;
    sf::Text m_hudText;
    sf::Text m_statusText;
    float m_statusTimeRemaining = 0.f;
    bool m_gameOver = false;
    bool m_victory = false;
};
