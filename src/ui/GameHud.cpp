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
    styleText(m_controlsText, font, 14U, sf::Color(210, 218, 232));
    styleText(m_statusText, font, 28U, sf::Color(255, 239, 137));
    styleText(m_timeLabel, font, 19U, sf::Color(158, 220, 255));
    styleText(m_timeText, font, 39U, sf::Color::White);
    m_timeLabel.setString("TIME");

    m_scoreLabel.setString("SCORE");
    m_controlsText.setString("F5 SAVE    F9 LOAD    ESC PAUSE");
    refreshText();
}

void GameHud::setData(const GameHudData& data) {
    if (data.score > m_data.score) {
        m_scorePulseRemaining = 0.32f;
    }
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
    m_scorePulseRemaining = std::max(
        0.f,
        m_scorePulseRemaining - seconds
    );

    const float pulse = m_scorePulseRemaining > 0.f
        ? m_scorePulseRemaining / 0.32f
        : 0.f;
    m_scorePanel.setOutlineThickness(3.f + pulse * 2.f);
    m_scorePanel.setOutlineColor(sf::Color(
        255,
        static_cast<sf::Uint8>(218 + 37.f * pulse),
        static_cast<sf::Uint8>(92 + 60.f * pulse),
        210
    ));

    // Timer warning pulse when <= 30 seconds
    if (m_data.timeRemaining > 0.f && m_data.timeRemaining <= 30.f) {
        m_timerWarningPulse += seconds * 6.f;
        const float alpha = (std::sin(m_timerWarningPulse) + 1.f) * 0.5f;
        const auto red = static_cast<sf::Uint8>(255);
        const auto green = static_cast<sf::Uint8>(60.f + 80.f * alpha);
        const auto blue = static_cast<sf::Uint8>(60.f + 80.f * alpha);
        m_timeText.setFillColor(sf::Color(red, green, blue));
    } else {
        m_timerWarningPulse = 0.f;
        m_timeText.setFillColor(sf::Color::White);
    }

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
    const sf::FloatRect controlsBounds = m_controlsText.getLocalBounds();
    m_controlsText.setOrigin(controlsBounds.width, 0.f);
    m_controlsText.setPosition(right - 24.f, top + 137.f);

    // Timer: centered horizontally at top
    const float centerX = viewCenter.x;
    sf::FloatRect timeLabelBounds = m_timeLabel.getLocalBounds();
    m_timeLabel.setOrigin(timeLabelBounds.left + timeLabelBounds.width / 2.f, 0.f);
    m_timeLabel.setPosition(centerX, top + 30.f);

    sf::FloatRect timeTextBounds = m_timeText.getLocalBounds();
    m_timeText.setOrigin(timeTextBounds.left + timeTextBounds.width / 2.f, 0.f);
    m_timeText.setPosition(centerX, top + 55.f);

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
    target.draw(m_controlsText);
    target.draw(m_timeLabel);
    target.draw(m_timeText);

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

    if (m_data.invincibilitySeconds > 0.f) {
        m_powerText.setFillColor(sf::Color(255, 236, 116));
    } else if (m_data.speedBoostSeconds > 0.f) {
        m_powerText.setFillColor(sf::Color(115, 238, 255));
    } else {
        m_powerText.setFillColor(sf::Color(178, 231, 255));
    }

    const int displayTime = static_cast<int>(std::ceil(m_data.timeRemaining));
    m_timeText.setString(std::to_string(std::max(0, displayTime)));
}

std::string GameHud::formatScore(int score) {
    std::ostringstream output;
    output << std::setfill('0') << std::setw(7) << std::max(0, score);
    return output.str();
}
