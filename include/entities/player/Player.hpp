#pragma once
#include "Character.hpp"
#include "SFML/Window/Keyboard.hpp"

class Player : public Character {
public:
    static constexpr float CollisionWidth = 38.f;
    static constexpr float CollisionHeight = 45.f;
protected:
    bool inputEnabled_ = true;
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
    bool invincible_ = false;
    std::string label_;

    // Grow/Shrink animation states
    bool isGrowing_ = false;
    float growAnimTime_ = 0.0f;
    static constexpr float GrowAnimDuration = 1.0f;

    bool isShrinking_ = false;
    float shrinkAnimTime_ = 0.0f;
    static constexpr float ShrinkAnimDuration = 1.0f;

    void performJump();

public:
    enum class State { Stand, Walk, Jump, Fall, HitRoof, TransitionStand, PoleSlide, AutoWalk };

protected:
    State currentState = State::Stand;

public:
    void setInputEnabled(bool enabled) { inputEnabled_ = enabled; }
    void forceState(State state) { 
        if (currentState != state) {
            currentState = state; 
            currentFrame_ = 0;
            animationTime_ = 0.f;
        }
    }

    Player(sf:: Vector2f position, std::string label, float speed, float jumpPower)
        : Character(position, CollisionWidth, CollisionHeight),
          speed_(speed), jumpPower_(jumpPower),
          label_(std::move(label)) {}
          
    bool isSuper() const { return mushroom_ || fireMario_; }
    bool isInvincible() const { return invincible_; }
    void setInvincible(bool invincible) { invincible_ = invincible; }

    std:: string name() const override{
        return label_;
    }

    void up2Fire(){
        fireMario_ = true;
        if (!mushroom_) {
            mushroom_ = true;
            jumpPower_ = jumpPower_ * 1.25f;
            isGrowing_ = true;
            growAnimTime_ = 0.0f;
            isShrinking_ = false; // Cancel shrink if it was playing
        }
    }
    void getMushroom(){
        if (!mushroom_) {
            mushroom_ = true;
            jumpPower_ = jumpPower_ * 1.25f;
            isGrowing_ = true;
            growAnimTime_ = 0.0f;
            isShrinking_ = false; // Cancel shrink if it was playing
        }
    }
    void shrinkPlayer(){
        if (mushroom_) {
            mushroom_ = false;
            jumpPower_ = jumpPower_ / 1.25f;
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
