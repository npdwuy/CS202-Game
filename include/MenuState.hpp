#pragma once

#include "GameState.hpp"
#include "levels/TileMap.hpp"
#include "ui/Button.hpp"

#include <SFML/Graphics.hpp>
#include <memory>

class MenuState : public GameState {
public:
    MenuState();
    ~MenuState() override = default;

    void Input(const sf::Event& event) override;
    void Update(sf::Time timePerFrame) override;
    void Render(sf::RenderWindow& window) override;

private:
    sf::Font m_font;

    // Scrolling map background
    TileMap  m_bgMap;
    sf::View m_bgCamera;
    float    m_scrollX = 0.f;
    static constexpr float ScrollSpeed = 50.f;  // px/s

    std::shared_ptr<sf::Texture> m_bgTexture;
    sf::Sprite m_bgSprite;

    // 4 menu buttons (PLAY, LOAD, OPTIONS, EXIT)
    std::unique_ptr<Button> m_playButton;
    std::unique_ptr<Button> m_loadButton;
    std::unique_ptr<Button> m_optionsButton;
    std::unique_ptr<Button> m_exitButton;
};
