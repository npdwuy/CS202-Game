#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

class GameState {
public:
    virtual ~GameState() = default;
    virtual void Input(const sf::Event &event) = 0;
    virtual void Update(sf::Time timePerFrame) = 0;
    virtual void Render(sf::RenderWindow &window) = 0;

    virtual bool isOverlay() const { return false; }
    virtual void onPause() {}
    virtual void onResume() {}
};