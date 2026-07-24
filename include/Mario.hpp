#include "Player.hpp"

class Mario: public Player {
private:
    sf::Texture texture_;
    sf::Sprite sprite_;
    
public:
    explicit Mario(sf::Vector2f position): Player(position, "Mario", 292.0f, 720.0f){}
    void draw(sf::RenderWindow& window) const override;
    void update(sf::Time timePerFrame)override;
};