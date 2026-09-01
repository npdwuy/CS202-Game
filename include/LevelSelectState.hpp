#pragma once

#include "GameState.hpp"
#include "ui/Button.hpp"
#include "levels/TileMap.hpp"
#include "ui/ParallaxBackground.hpp"

#include <SFML/Graphics.hpp>
#include <memory>
#include <array>

class LevelSelectState : public GameState {
public:
    LevelSelectState();
    ~LevelSelectState() override = default;

    void Input(const sf::Event& event) override;
    void Update(sf::Time timePerFrame) override;
    void Render(sf::RenderWindow& window) override;

private:
    void selectLevel(int level);

    sf::Font m_font;

    // Scrolling map background
    TileMap  m_bgMap;
    sf::View m_bgCamera;
    ParallaxBackground m_parallaxBg;
    float    m_scrollX = 0.f;
    static constexpr float ScrollSpeed = 40.f;

    sf::Text m_titleText;

    // 3 level buttons + back button
    std::array<std::unique_ptr<Button>, 3> m_levelButtons;
    std::unique_ptr<Button> m_generateButton;
    std::unique_ptr<Button> m_backButton;

    // Level info texts
    std::array<sf::Text, 3> m_difficultyText;

    sf::RectangleShape m_overlay; // semi-transparent backdrop
};
