#pragma once

#include <SFML/Graphics.hpp>
#include "GameState.hpp"
#include "factories/LevelObjectFactory.hpp"
#include "levels/TileMap.hpp"
#include "persistence/SaveData.hpp"
#include "entities/Enemy.hpp"
#include "entities/Item.hpp"
#include "entities/player/Player.hpp"
#include "events/GameEventListener.hpp"
#include "ui/GameHud.hpp"
#include "camera/PlayerCamera.hpp"
#include "ui/Button.hpp"
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
    bool handlePlayerFall();
    void handleLevelExit();
    void handlePlayerDamage();
    void updateTimedPowerUps(sf::Time timePerFrame);
    void updateLevelTimer(sf::Time timePerFrame);
    void resetTransientEffects();
    void loseLife();
    void updateHud();
    void updateCamera(sf::Time timePerFrame);
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
    GameHud m_hud;
    PlayerCamera m_camera;
    float m_invincibilityTimeRemaining = 0.f;
    float m_speedBoostTimeRemaining = 0.f;
    float m_damageCooldown = 0.f;
    bool m_playerDamagePending = false;
    float m_timeRemaining = 400.f;

    std::unique_ptr<Button> m_menuButton;
    sf::Texture m_buttonTexture;

struct BackgroundLayer
{
    std::shared_ptr<sf::Texture> texture;
    sf::Sprite sprite;

    float parallaxFactor = 0.f;
    float driftAmplitude = 0.f;
    float driftSpeed = 0.f;
};

std::vector<BackgroundLayer> m_backgroundLayers;

float m_backgroundAnimationTime = 0.f;

void loadBackgroundLayers();

void addBackgroundLayer(
    const std::string& path,
    float parallaxFactor,
    float driftAmplitude,
    float driftSpeed,
    bool required
);

void updateBackgroundLayers(
    sf::Time timePerFrame,
    const sf::View& view
);

void renderBackgroundLayers(
    sf::RenderWindow& window
);
};
