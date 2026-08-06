#include "ui/GameHud.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace {

void styleText(
    sf::Text& text,
    const sf::Font& font,
    unsigned int size,
    sf::Color color
) {
    text.setFont(font);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setOutlineColor(sf::Color(18, 20, 32, 220));
    text.setOutlineThickness(2.f);
}

}

GameHud::GameHud(const sf::Font& font) {
    m_scoreShadow.setSize({310.f, 104.f});
    m_scoreShadow.setFillColor(sf::Color(12, 15, 28, 105));

    m_scorePanel.setSize({310.f, 104.f});
    m_scorePanel.setFillColor(sf::Color(24, 31, 52, 225));
    m_scorePanel.setOutlineColor(sf::Color(255, 218, 92, 210));
    m_scorePanel.setOutlineThickness(3.f);

    m_scoreAccent.setSize({9.f, 104.f});
    m_scoreAccent.setFillColor(sf::Color(255, 184, 48));

    m_infoPanel.setSize({520.f, 104.f});
    m_infoPanel.setFillColor(sf::Color(24, 31, 52, 205));
    m_infoPanel.setOutlineColor(sf::Color(126, 210, 255, 180));
    m_infoPanel.setOutlineThickness(2.f);

    m_statusPanel.setFillColor(sf::Color(24, 31, 52, 215));
    m_statusPanel.setOutlineColor(sf::Color(255, 218, 92, 205));
    m_statusPanel.setOutlineThickness(2.f);

    styleText(m_scoreLabel, font, 19U, sf::Color(158, 220, 255));
    styleText(m_scoreText, font, 39U, sf::Color(255, 239, 137));
    styleText(m_infoText, font, 23U, sf::Color::White);
    styleText(m_powerText, font, 18U, sf::Color(178, 231, 255));
    styleText(m_statusText, font, 28U, sf::Color(255, 239, 137));

    m_scoreLabel.setString("SCORE");
    refreshText();
}

void GameHud::setData(const GameHudData& data) {
    m_data = data;
    m_data.score = std::max(0, m_data.score);
    m_data.lives = std::max(0, m_data.lives);
    refreshText();
}

void GameHud::update(sf::Time deltaTime) {
    const float seconds = std::max(0.f, deltaTime.asSeconds());
    if (m_displayedScore != m_data.score) {
        const int difference = m_data.score - m_displayedScore;
        const int step = std::max(
            1,
            static_cast<int>(std::ceil(std::abs(difference) *
                                       std::min(1.f, seconds * 9.f)))
        );
        m_displayedScore += std::clamp(difference, -step, step);
    }

    m_statusTimeRemaining = std::max(
        0.f,
        m_statusTimeRemaining - seconds
    );
    refreshText();
}

void GameHud::layout(const sf::View& screenView) {
    const sf::Vector2f viewSize = screenView.getSize();
    const sf::Vector2f viewCenter = screenView.getCenter();
    const float left = viewCenter.x - viewSize.x * 0.5f;
    const float right = viewCenter.x + viewSize.x * 0.5f;
    const float top = viewCenter.y - viewSize.y * 0.5f;

    m_scoreShadow.setPosition(left + 31.f, top + 31.f);
    m_scorePanel.setPosition(left + 24.f, top + 24.f);
    m_scoreAccent.setPosition(left + 24.f, top + 24.f);
    m_scoreLabel.setPosition(left + 50.f, top + 34.f);
    m_scoreText.setPosition(left + 48.f, top + 57.f);

    m_infoPanel.setPosition(right - 544.f, top + 24.f);
    m_infoText.setPosition(right - 518.f, top + 38.f);
    m_powerText.setPosition(right - 518.f, top + 77.f);

    const sf::FloatRect textBounds = m_statusText.getLocalBounds();
    m_statusText.setOrigin(
        textBounds.left + textBounds.width * 0.5f,
        textBounds.top + textBounds.height * 0.5f
    );
    m_statusText.setPosition(viewCenter.x, top + 173.f);

    const float panelWidth = textBounds.width + 54.f;
    m_statusPanel.setSize({panelWidth, 58.f});
    m_statusPanel.setOrigin(panelWidth * 0.5f, 29.f);
    m_statusPanel.setPosition(viewCenter.x, top + 173.f);
}

void GameHud::showStatus(const std::string& message, float duration) {
    m_statusText.setString(message);
    m_statusTimeRemaining = std::max(0.f, duration);
}

void GameHud::render(sf::RenderTarget& target) const {
    target.draw(m_scoreShadow);
    target.draw(m_scorePanel);
    target.draw(m_scoreAccent);
    target.draw(m_infoPanel);
    target.draw(m_scoreLabel);
    target.draw(m_scoreText);
    target.draw(m_infoText);
    target.draw(m_powerText);

    if (m_statusTimeRemaining > 0.f) {
        target.draw(m_statusPanel);
        target.draw(m_statusText);
    }
}

void GameHud::refreshText() {
    m_scoreText.setString(formatScore(m_displayedScore));
    m_infoText.setString(
        "WORLD  " + std::to_string(m_data.level) +
        "       LIVES  x" + std::to_string(m_data.lives)
    );

    std::string power = "POWER  " + m_data.powerUp;
    if (m_data.invincibilitySeconds > 0.f) {
        power += "   STAR " + std::to_string(
            static_cast<int>(std::ceil(m_data.invincibilitySeconds))
        ) + "s";
    }
    if (m_data.speedBoostSeconds > 0.f) {
        power += "   SPEED " + std::to_string(
            static_cast<int>(std::ceil(m_data.speedBoostSeconds))
        ) + "s";
    }
    m_powerText.setString(power);
}

std::string GameHud::formatScore(int score) {
    std::ostringstream output;
    output << std::setfill('0') << std::setw(7) << std::max(0, score);
    return output.str();
}
