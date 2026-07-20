#pragma once
#include "GameState.hpp"

class PlayState : public GameState {
public:
    PlayState();
    ~PlayState() override = default;

    void Input(const sf::Event &event) override;
    void Update(sf::Time timePerFrame) override;
    void Render(sf::RenderWindow &window) override;
};