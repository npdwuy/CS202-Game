#pragma once

#include <SFML/Graphics.hpp>

#include <string>

struct GameHudData {
    int level = 1;
    int score = 0;
    int lives = 10;
    std::string powerUp = "None";
    float invincibilitySeconds = 0.f;
    float speedBoostSeconds = 0.f;
    float timeRemaining = 400.f;
    int coins = 0;
};

class GameHud {
public:
    explicit GameHud(const sf::Font& font);

    void setData(const GameHudData& data);
    void update(sf::Time deltaTime);
    void layout(const sf::View& screenView);
    void showStatus(const std::string& message, float duration = 2.f);
    void render(sf::RenderTarget& target) const;

private:
    void refreshText();
    static std::string formatScore(int score);

    GameHudData m_data;
    float m_displayedScore = 0.f;
    float m_statusTimeRemaining = 0.f;
    float m_scorePulseRemaining = 0.f;

    sf::RectangleShape m_topBar;
    sf::RectangleShape m_statusPanel;
    sf::Text m_scoreText;
    sf::Text m_infoText;
    sf::Text m_powerText;
    sf::Text m_controlsText;
    sf::Text m_statusText;
    sf::Text m_timeText;
    float m_timerWarningPulse = 0.f;
};
