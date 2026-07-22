#pragma once

#include "GameState.hpp"
#include "entities/Enemy.hpp"
#include "entities/Item.hpp"

#include <memory>
#include <vector>

class PlayState : public GameState {
public:
    PlayState();
    ~PlayState() override = default;

    void Input(const sf::Event& event) override;
    void Update(sf::Time timePerFrame) override;
    void Render(sf::RenderWindow& window) override;

private:
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::vector<std::unique_ptr<Item>> m_items;
};