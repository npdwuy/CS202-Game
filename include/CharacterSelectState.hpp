#pragma once

#include "GameState.hpp"
#include "ui/Button.hpp"

#include <SFML/Graphics.hpp>
#include <memory>

class CharacterSelectState : public GameState {
public:
    CharacterSelectState();
    ~CharacterSelectState() override = default;

    void Input(const sf::Event& event) override;
    void Update(sf::Time timePerFrame) override;
    void Render(sf::RenderWindow& window) override;

private:
    void confirmSelection();

    sf::Font    m_font;
    sf::Texture m_marioTexture;
    sf::Texture m_luigiTexture;
    sf::Sprite  m_marioSprite;
    sf::Sprite  m_luigiSprite;

    sf::Text m_titleText;
    sf::Text m_marioLabel;
    sf::Text m_luigiLabel;
    sf::Text m_hintText;

    sf::RectangleShape m_highlight;  // Yellow box around selected character

    int m_selected = 0;   // 0 = Mario, 1 = Luigi

    std::unique_ptr<Button> m_confirmButton;
};
