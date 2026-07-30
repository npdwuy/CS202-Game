#pragma once

#include "GameState.hpp"
#include "ui/Button.hpp"
#include "ui/Slider.hpp"
#include <memory>
#include <map>
#include <vector>

class OptionsState : public GameState {
public:
    OptionsState();
    ~OptionsState() override = default;

    void Input(const sf::Event &event) override;
    void Update(sf::Time timePerFrame) override;
    void Render(sf::RenderWindow &window) override;

    bool isOverlay() const override { return true; }

private:
    void initUI();
    void updateButtonLabels();

private:
    sf::Font m_font;
    sf::Texture m_buttonTexture;

    // Panel visuals
    sf::RectangleShape m_dimOverlay;
    sf::RectangleShape m_panelBackground;
    
    // Headers and Labels
    sf::Text m_titleText;
    std::vector<sf::Text> m_sectionHeaders;
    std::vector<std::pair<sf::Text, std::string>> m_controlLabels; // Label text + action key

    // Controls
    std::unique_ptr<Button> m_difficultyButton;
    std::unique_ptr<Slider> m_sfxSlider;
    std::unique_ptr<Slider> m_bgmSlider;
    
    std::map<std::string, std::unique_ptr<Button>> m_bindButtons;
    std::unique_ptr<Button> m_resetButton;
    std::unique_ptr<Button> m_backButton;

    // Control rebinding tracking
    std::string m_rebindingAction;
};
