#pragma once
#include "Character.hpp"
#include "SFML/Window/Keyboard.hpp"

class Player : public Character {
public:
    static constexpr float CollisionWidth = 32.f;
    static constexpr float CollisionHeight = 70.f;
protected:
    float coyoteTimer_ = 0.0f;
    int currentFrame_ = 0;
    float speed_ = 280.0f;
    // Input buffer for rapid tap handling
    float moveLeftTimer_ = 0.0f;
    float moveRightTimer_ = 0.0f;
    static constexpr float MoveInputWindow = 0.12f; // seconds
    float speedMultiplier_ = 1.0f;
    float jumpPower_ = 670.0f;
    float animationTime_ = 0.0f;
    float jumpBufferTimer_ = 0.0f;
    bool movedThisFrame_ = false;
    bool jumpedThisFrame_ = false;
    bool jumpHeld_ = false;

    bool fireMario_ = false;
    bool mushroom_ = false;
    std::string label_;

    void performJump();

    enum class State { Stand, Walk, Jump, Fall, HitRoof, TransitionStand };

    State currentState = State::Stand;

public:
    Player(sf:: Vector2f position, std::string label, float speed, float jumpPower)
        : Character(position, CollisionWidth, CollisionHeight),
          speed_(speed), jumpPower_(jumpPower),
          label_(std::move(label)) {}

    std:: string name() const override{
        return label_;
    }

    void up2Fire(){
        fireMario_ = true;
    }
    void getMushroom(){
        mushroom_ = true;
        width_ = width_ * 1.2f;
        height_ = height_ * 1.2f;
    }

    void update(sf::Time timePerFrame) override;

    void moveLeft();
    void moveRight();
    void jump();
    bool consumeJumpEvent();
    void setSpeedMultiplier(float multiplier);
    float speedMultiplier() const;

};
