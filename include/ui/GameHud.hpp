#pragma once

#include <SFML/Graphics.hpp>

#include <string>

struct GameHudData {
    int level = 1;
    int score = 0;
    int lives = 3;
    std::string powerUp = "None";
    float invincibilitySeconds = 0.f;
    float speedBoostSeconds = 0.f;
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
    int m_displayedScore = 0;
    float m_statusTimeRemaining = 0.f;

    sf::RectangleShape m_scoreShadow;
    sf::RectangleShape m_scorePanel;
    sf::RectangleShape m_scoreAccent;
    sf::RectangleShape m_infoPanel;
    sf::RectangleShape m_statusPanel;
    sf::Text m_scoreLabel;
    sf::Text m_scoreText;
    sf::Text m_infoText;
    sf::Text m_powerText;
    sf::Text m_statusText;
};
