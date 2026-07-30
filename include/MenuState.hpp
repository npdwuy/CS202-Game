#pragma once
#include "GameState.hpp"
#include "ui/Button.hpp"
#include <memory>

class MenuState : public GameState {
public:
    MenuState();
    ~MenuState() override = default;

    void Input(const sf::Event &event) override;
    void Update(sf::Time timePerFrame) override;
    void Render(sf::RenderWindow &window) override;

private:
    sf::Texture m_backgroundTexture;
    sf::Texture m_buttonTexture;
    sf::Sprite m_backgroundSprite;
    sf::Font m_font;

    std::unique_ptr<Button> m_playButton;
    std::unique_ptr<Button> m_aboutButton;
    std::unique_ptr<Button> m_optionsButton;
    std::unique_ptr<Button> m_exitButton;
};