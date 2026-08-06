#pragma once
#include "Player.hpp"

class Mario: public Player {
private:
    sf::Texture texture_;
    sf::Sprite sprite_;
    int baseImage_ = 0;
    bool fireMario_ = false;
    bool mushroom_ = false;
public:
    explicit Mario(sf::Vector2f position);
    void Render(sf::RenderWindow& window) const override;
    void update(sf::Time timePerFrame)override;
    void up2Fire(){
        fireMario_ = true;
    }
    void getMushroom(){
        mushroom_ = true;
    }
};