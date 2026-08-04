#include "GameOverState.hpp"
#include "GameManager.hpp"
#include "PlayState.hpp"
#include "resources/ResourceManager.hpp"

GameOverState::GameOverState() {
    m_dimOverlay.setSize(sf::Vector2f(1920.f, 1080.f));
    m_dimOverlay.setFillColor(sf::Color(0, 0, 0, 150));

    const sf::Font& font = ResourceManager::getInstance().getFont(
        "assets/fonts/ro-spritendo-font/RoSpritendoSemiboldBeta-vmVwZ.otf"
    );

    m_gameOverText.setFont(font);
    m_gameOverText.setString("GAME OVER - Press R to restart");
    m_gameOverText.setCharacterSize(34);
    m_gameOverText.setFillColor(sf::Color(255, 235, 120));
    m_gameOverText.setOutlineColor(sf::Color::Black);
    m_gameOverText.setOutlineThickness(3.f);

    const sf::FloatRect textBounds = m_gameOverText.getLocalBounds();
    m_gameOverText.setOrigin(
        textBounds.left + textBounds.width / 2.f,
        textBounds.top + textBounds.height / 2.f
    );
    m_gameOverText.setPosition(960.f, 540.f);
}

void GameOverState::Input(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::R) {
            GameManager::getInstance().changeState(std::make_unique<PlayState>(false));
        }
    }
}

void GameOverState::Update(sf::Time timePerFrame) {
    // Nothing to update
}

void GameOverState::Render(sf::RenderWindow& window) {
    window.draw(m_dimOverlay);
    window.draw(m_gameOverText);
}
