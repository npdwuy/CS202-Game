#pragma once
#include "GameState.hpp"
#include "ui/Button.hpp"
#include <memory>

class MenuState : public GameState {
public:
    MenuState(); // Constructor nếu cần khởi tạo tài nguyên
    ~MenuState() override = default;

    void Input(const sf::Event &event) override;
    void Update(sf::Time timePerFrame) override;
    void Render(sf::RenderWindow &window) override;

    bool hasNextState() const override;
    std::unique_ptr<GameState> getNextState() override;

private:
    bool m_transitionToPlay = false;
    bool m_exitGame = false;

    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;
    sf::Font m_font;

    std::unique_ptr<Button> m_playButton;
    std::unique_ptr<Button> m_exitButton;
};