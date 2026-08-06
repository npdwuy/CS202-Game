#pragma once

#include "GameState.hpp"
#include <SFML/Graphics.hpp>

class LevelCompleteState : public GameState {
public:
    LevelCompleteState();
    ~LevelCompleteState() override = default;

    void Input(const sf::Event& event) override;
    void Update(sf::Time timePerFrame) override;
    void Render(sf::RenderWindow& window) override;
    
    bool isOverlay() const override { return true; }

private:
    sf::RectangleShape m_dimOverlay;
    sf::Text m_victoryText;
};
