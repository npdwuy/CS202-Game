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
    text.setOutlineThickness(1.5f);
}

}

GameHud::GameHud(const sf::Font& font) {
    m_topBar.setFillColor(sf::Color(15, 20, 35, 170));

    m_statusPanel.setFillColor(sf::Color(24, 31, 52, 215));
    m_statusPanel.setOutlineColor(sf::Color(255, 218, 92, 205));
    m_statusPanel.setOutlineThickness(2.f);

    styleText(m_scoreText, font, 20U, sf::Color(255, 239, 137));
    styleText(m_infoText, font, 20U, sf::Color::White);
    styleText(m_powerText, font, 18U, sf::Color(178, 231, 255));
    styleText(m_controlsText, font, 14U, sf::Color(210, 218, 232));
    styleText(m_statusText, font, 28U, sf::Color(255, 239, 137));
    styleText(m_timeText, font, 20U, sf::Color::White);

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
    if (m_displayedScore != static_cast<float>(m_data.score)) {
        float difference = m_data.score - m_displayedScore;
        m_displayedScore += difference * std::min(1.f, seconds * 15.f);
        if (std::abs(m_data.score - m_displayedScore) < 0.5f) {
            m_displayedScore = static_cast<float>(m_data.score);
        }
    }

    m_statusTimeRemaining = std::max(
        0.f,
        m_statusTimeRemaining - seconds
    );
    m_scorePulseRemaining = std::max(
        0.f,
        m_scorePulseRemaining - seconds
    );

    // Score text highlight pulse if score increases
    if (m_scorePulseRemaining > 0.f) {
        const float pulse = m_scorePulseRemaining / 0.32f;
        m_scoreText.setFillColor(sf::Color(
            255,
            static_cast<sf::Uint8>(180 + 59.f * (1.f - pulse)),
            static_cast<sf::Uint8>(100 + 37.f * (1.f - pulse))
        ));
    } else {
        m_scoreText.setFillColor(sf::Color(255, 239, 137));
    }

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
    const float bottom = viewCenter.y + viewSize.y * 0.5f;

    m_topBar.setSize({viewSize.x, 60.f});
    m_topBar.setPosition(left, top);

    m_scoreText.setPosition(left + 90.f, top + 14.f);
    m_infoText.setPosition(left + 420.f, top + 14.f);

    // Right-align time text
    sf::FloatRect timeBounds = m_timeText.getLocalBounds();
    m_timeText.setOrigin(timeBounds.left + timeBounds.width, 0.f);
    m_timeText.setPosition(right - 40.f, top + 14.f);

    // Right-align power text, placing it safely to the left of time text
    sf::FloatRect powerBounds = m_powerText.getLocalBounds();
    m_powerText.setOrigin(powerBounds.left + powerBounds.width, 0.f);
    m_powerText.setPosition(right - 220.f, top + 14.f);

    // Controls text moved to bottom-right corner to keep header clean
    const sf::FloatRect controlsBounds = m_controlsText.getLocalBounds();
    m_controlsText.setOrigin(controlsBounds.width, controlsBounds.height);
    m_controlsText.setPosition(right - 24.f, bottom - 20.f);

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
    target.draw(m_topBar);
    target.draw(m_scoreText);
    target.draw(m_infoText);
    target.draw(m_powerText);
    target.draw(m_controlsText);
    target.draw(m_timeText);

    if (m_statusTimeRemaining > 0.f) {
        target.draw(m_statusPanel);
        target.draw(m_statusText);
    }
}

void GameHud::refreshText() {
    m_scoreText.setString("SCORE  " + formatScore(static_cast<int>(std::round(m_displayedScore))));
    m_infoText.setString(
        "WORLD  " + std::to_string(m_data.level) +
        "    LIVES  x" + std::to_string(m_data.lives) +
        "    COINS  x" + std::to_string(m_data.coins)
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
    m_timeText.setString("TIME  " + std::to_string(std::max(0, displayTime)));
}

std::string GameHud::formatScore(int score) {
    std::ostringstream output;
    output << std::setfill('0') << std::setw(7) << std::max(0, score);
    return output.str();
}
