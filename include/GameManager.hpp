#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "GameState.hpp"
#include "SettingsManager.hpp"

class GameManager {
public:
    static GameManager& getInstance();

    // Disable copy constructor and assignment operator
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    void pushState(std::unique_ptr<GameState> state);
    void popState();
    void changeState(std::unique_ptr<GameState> state);

    void run();
    void quit();

    sf::RenderWindow& getWindow();
    SettingsManager& getSettings();
    const sf::View& getGameView() const;

private:
    GameManager();
    ~GameManager() = default;

    void processInput();
    void update(sf::Time timePerFrame);
    void render();
    void updateView(unsigned int windowWidth, unsigned int windowHeight);

private:
    sf::RenderWindow m_window;
    sf::View m_gameView;
    std::vector<std::unique_ptr<GameState>> m_states;
    SettingsManager m_settings;
    bool m_isRunning = true;
};

