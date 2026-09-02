#pragma once
#include "GameState.hpp"
#include "ui/Button.hpp"
#include "levels/TileMap.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <array>

class GenerateLevelState : public GameState {
public:
    GenerateLevelState();
    ~GenerateLevelState() override = default;

    void Input(const sf::Event& event) override;
    void Update(sf::Time timePerFrame) override;
    void Render(sf::RenderWindow& window) override;

private:
    void generateAndPlay(int difficulty);

    sf::Font m_font;
    TileMap  m_bgMap;
    sf::View m_bgCamera;
    float    m_scrollX = 0.f;

    sf::Text m_titleText;

    std::array<std::unique_ptr<Button>, 3> m_diffButtons;
    std::unique_ptr<Button> m_backButton;
};
