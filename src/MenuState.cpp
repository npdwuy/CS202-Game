#include "MenuState.hpp"
#include "PlayState.hpp"
#include <iostream>

MenuState::MenuState() {
    // Initialize menu UI assets, buttons, and background here
}

void MenuState::Input(const sf::Event &event) {
    // Handle single-press event triggers
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Enter) {
            std::cout << "Transitioning to PlayState...\n";
            m_transitionToPlay = true;
        }
    }
}

void MenuState::Update(sf::Time timePerFrame) {
    // Update menu dynamic animations or effects here
}

void MenuState::Render(sf::RenderWindow &window) {
    // Member A handles drawing Main Menu UI layout here
}

bool MenuState::hasNextState() const {
    return m_transitionToPlay;
}

std::unique_ptr<GameState> MenuState::getNextState() {
    return std::make_unique<PlayState>();
}