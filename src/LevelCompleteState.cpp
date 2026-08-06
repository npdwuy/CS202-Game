#include "LevelCompleteState.hpp"
#include "GameManager.hpp"
#include "PlayState.hpp"
#include "MenuState.hpp"
#include "resources/ResourceManager.hpp"

LevelCompleteState::LevelCompleteState() {
    m_dimOverlay.setSize(sf::Vector2f(1920.f, 1080.f));
    m_dimOverlay.setFillColor(sf::Color(0, 0, 0, 150));

    const sf::Font& font = ResourceManager::getInstance().getFont(
        "assets/fonts/ro-spritendo-font/RoSpritendoSemiboldBeta-vmVwZ.otf"
    );

    m_victoryText.setFont(font);
    m_victoryText.setString("YOU WIN! Press Enter for menu or R to restart");
    m_victoryText.setCharacterSize(34);
    m_victoryText.setFillColor(sf::Color(255, 235, 120));
    m_victoryText.setOutlineColor(sf::Color::Black);
    m_victoryText.setOutlineThickness(3.f);

    const sf::FloatRect textBounds = m_victoryText.getLocalBounds();
    m_victoryText.setOrigin(
        textBounds.left + textBounds.width / 2.f,
        textBounds.top + textBounds.height / 2.f
    );
    m_victoryText.setPosition(960.f, 540.f);
}

void LevelCompleteState::Input(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::R) {
            GameManager::getInstance().changeState(std::make_unique<PlayState>(false));
        } else if (event.key.code == sf::Keyboard::Enter) {
            GameManager::getInstance().changeState(std::make_unique<MenuState>());
        }
    }
}

void LevelCompleteState::Update(sf::Time timePerFrame) {
    // Nothing to update
}

void LevelCompleteState::Render(sf::RenderWindow& window) {
    window.draw(m_dimOverlay);
    window.draw(m_victoryText);
}
