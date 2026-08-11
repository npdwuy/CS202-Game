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

    // Grow/Shrink animation states
    bool isGrowing_ = false;
    float growAnimTime_ = 0.0f;
    static constexpr float GrowAnimDuration = 1.2f;

    bool isShrinking_ = false;
    float shrinkAnimTime_ = 0.0f;
    static constexpr float ShrinkAnimDuration = 1.2f;

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
        if (!mushroom_) {
            mushroom_ = true;
            float old_width = width_;
            float old_height = height_;
            width_ = width_ * 1.5f;
            height_ = height_ * 1.5f;
            jumpPower_ = jumpPower_ * 1.25f;
            position_.x = position_.x + (old_width / 2.0f) - (width_ / 2.0f);
            position_.y = position_.y + old_height - height_;
            isGrowing_ = true;
            growAnimTime_ = 0.0f;
            isShrinking_ = false; // Cancel shrink if it was playing
        }
    }
    void getMushroom(){
        if (!mushroom_) {
            mushroom_ = true;
            float old_width = width_;
            float old_height = height_;
            width_ = width_ * 1.5f;
            height_ = height_ * 1.5f;
            jumpPower_ = jumpPower_ * 1.25f;
            position_.x = position_.x + (old_width / 2.0f) - (width_ / 2.0f);
            position_.y = position_.y + old_height - height_;
            isGrowing_ = true;
            growAnimTime_ = 0.0f;
            isShrinking_ = false; // Cancel shrink if it was playing
        }
    }
    void shrinkPlayer(){
        if (mushroom_) {
            mushroom_ = false;
            float old_width = width_;
            float old_height = height_;
            width_ = width_ / 1.5f;
            height_ = height_ / 1.5f;
            jumpPower_ = jumpPower_ / 1.25f;
            position_.x = position_.x + (old_width / 2.0f) - (width_ / 2.0f);
            position_.y = position_.y + old_height - height_;
            isShrinking_ = true;
            shrinkAnimTime_ = 0.0f;
            isGrowing_ = false; // Cancel grow if it was playing
        }
        fireMario_ = false; // Lose fire mario status when taking damage
    }

    bool isGrowing() const { return isGrowing_; }
    float growAnimTime() const { return growAnimTime_; }

    bool isShrinking() const { return isShrinking_; }
    float shrinkAnimTime() const { return shrinkAnimTime_; }

    void update(sf::Time timePerFrame) override;

    void moveLeft();
    void moveRight();
    void jump();
    bool consumeJumpEvent();
    void setSpeedMultiplier(float multiplier);
    float speedMultiplier() const;

};
