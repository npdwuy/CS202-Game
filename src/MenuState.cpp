#include "MenuState.hpp"
#include "PlayState.hpp"
#include <iostream>
#include <stdexcept>

MenuState::MenuState() {
    if (!m_backgroundTexture.loadFromFile("assets/sprites/menu_bg.png")) {
        throw std::runtime_error("Failed to load assets/sprites/menu_bg.png");
    }
    m_backgroundSprite.setTexture(m_backgroundTexture);

    if (!m_buttonTexture.loadFromFile("assets/sprites/button/btn_transparent.png")) {
        throw std::runtime_error("Failed to load assets/sprites/button/btn_transparent.png");
    }

    if (!m_font.loadFromFile("assets/fonts/ro-spritendo-font/RoSpritendoSemiboldBeta-vmVwZ.otf")) {
        throw std::runtime_error("Failed to load assets/fonts/ro-spritendo-font/RoSpritendoSemiboldBeta-vmVwZ.otf");
    }

    // Position buttons in the middle of the screen (assuming 1920x1080 resolution)
    // Custom button aspect ratio is ~2.48. A size of 350x140 matches this aspect ratio perfectly.
    m_playButton = std::make_unique<Button>("PLAY", m_font, m_buttonTexture, sf::Vector2f(960.f, 540.f), sf::Vector2f(350.f, 140.f), 40);
    m_exitButton = std::make_unique<Button>("EXIT", m_font, m_buttonTexture, sf::Vector2f(960.f, 700.f), sf::Vector2f(350.f, 140.f), 40);

    // Customize button colors (White keeps the original texture colors; hover uses a warm tint/dimming)
    m_playButton->setColors(sf::Color::White, sf::Color(255, 230, 200, 255), sf::Color(245, 222, 179));
    m_exitButton->setColors(sf::Color::White, sf::Color(255, 210, 210, 255), sf::Color(245, 222, 179));
}

void MenuState::Input(const sf::Event &event) {
    // 1. Mouse movement to update hover status
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
        if (m_playButton) m_playButton->update(mousePos);
        if (m_exitButton) m_exitButton->update(mousePos);
    }

    // 2. Mouse click to select option
    if (event.type == sf::Event::MouseButtonReleased) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        if (m_playButton && m_playButton->isClicked(event, mousePos)) {
            std::cout << "Transitioning to PlayState...\n";
            m_transitionToPlay = true;
        }
        if (m_exitButton && m_exitButton->isClicked(event, mousePos)) {
            std::cout << "Exiting game from Main Menu...\n";
            m_exitGame = true;
        }
    }

    // 3. Keyboard input triggers
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Enter) {
            std::cout << "Transitioning to PlayState...\n";
            m_transitionToPlay = true;
        } else if (event.key.code == sf::Keyboard::Escape) {
            std::cout << "Exiting game from Main Menu...\n";
            m_exitGame = true;
        }
    }
}

void MenuState::Update(sf::Time timePerFrame) {
    // Update menu animations or effects if any
}

void MenuState::Render(sf::RenderWindow &window) {
    // Scale background dynamically to window size
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u textureSize = m_backgroundTexture.getSize();
    if (textureSize.x > 0 && textureSize.y > 0) {
        m_backgroundSprite.setScale(
            static_cast<float>(windowSize.x) / textureSize.x,
            static_cast<float>(windowSize.y) / textureSize.y
        );
    }

    window.draw(m_backgroundSprite);

    if (m_playButton) m_playButton->render(window);
    if (m_exitButton) m_exitButton->render(window);
}

bool MenuState::hasNextState() const {
    return m_transitionToPlay || m_exitGame;
}

std::unique_ptr<GameState> MenuState::getNextState() {
    if (m_transitionToPlay) {
        return std::make_unique<PlayState>();
    }
    return nullptr; // nullptr represents exit
}