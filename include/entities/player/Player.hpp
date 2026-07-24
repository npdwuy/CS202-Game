#pragma once
#include "Character.hpp"
#include "SFML/Window/Keyboard.hpp"

class Player : public Character {
protected:
    float coyoteTimer_ = 0.0f;
    int currentFrame_ = 0;
    float speed_ = 280.0f;
    float jumpPower_ = 660.0f;
    float animationTime_ = 0.0f;
    bool movedThisFrame_ = false;
    float jumpBufferTimer_ = 0.0f;
    std::string label_;
    
    void performJump();
public:
    Player(sf:: Vector2f position, std::string label, float speed, float jumpPower)
        : Character(position, 27.0f, 30.0f),
          speed_(speed), jumpPower_(jumpPower),
          label_(std::move(label)) {}

    std:: string name() const override{
        return label_;
    }

    // void Render(sf::RenderWindow& window) const override = 0;
    void update(sf::Time timePerFrame) override;

    void moveLeft();
    void moveRight();
    void jump();

};