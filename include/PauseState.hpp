#pragma once

#include "GameState.hpp"
#include "ui/Button.hpp"
#include "ui/Panel.hpp"
#include <memory>
#include <vector>

class PauseState : public GameState {
public:
    PauseState();
    ~PauseState() override = default;

    void Input(const sf::Event &event) override;
    void Update(sf::Time timePerFrame) override;
    void Render(sf::RenderWindow &window) override;

    bool isOverlay() const override { return true; }

private:
    void initUI();

private:
    sf::Font m_font;

    sf::RectangleShape m_dimOverlay;
    std::unique_ptr<Panel> m_panelBackground;
    
    sf::Text m_titleText;
    
    std::unique_ptr<Button> m_resumeButton;
    std::unique_ptr<Button> m_optionsButton;
    std::unique_ptr<Button> m_menuButton;
    std::unique_ptr<Button> m_exitButton;
};
