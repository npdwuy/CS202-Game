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

    std::string label_;

public:
    enum class PowerUpState { Small, Big, Fire };

protected:
    PowerUpState powerUp_ = PowerUpState::Small;

    // Grow/Shrink animation states
    bool isGrowing_ = false;
    float growAnimTime_ = 0.0f;
    static constexpr float GrowAnimDuration = 1.0f;

    bool isShrinking_ = false;
    float shrinkAnimTime_ = 0.0f;
    static constexpr float ShrinkAnimDuration = 1.0f;

    // Color-change animation state (Big <-> Fire transitions)
    bool isColorChanging_ = false;
    float colorChangeAnimTime_ = 0.0f;
    static constexpr float ColorChangeAnimDuration = 0.6f;

    // Death state variables
    bool isDead_ = false;
    float deathAnimTime_ = 0.0f;
    float deathVelocityY_ = -500.f;
    static constexpr float DeathHopDelay = 0.5f;

    // Jump power anchoring (prevents compounding)
    float baseJumpPower_ = 670.0f;
    static constexpr float BigJumpBonus = 207.5f;

    void performJump();

public:
    enum class State { Stand, Walk, Jump, Fall, HitRoof, TransitionStand, Dead, PoleSlide, AutoWalk };

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
          speed_(speed), jumpPower_(jumpPower), baseJumpPower_(jumpPower),
          label_(std::move(label)) {}
          
    bool isSuper() const { return mushroom_ || fireMario_; }

    std:: string name() const override{
        return label_;
    }

    void recalcJumpPower() {
        jumpPower_ = baseJumpPower_ + (powerUp_ != PowerUpState::Small ? BigJumpBonus : 0.f);
    }

    void up2Fire() {
        if (powerUp_ == PowerUpState::Small) {
            powerUp_ = PowerUpState::Fire;
            recalcJumpPower();
            isGrowing_ = true;
            growAnimTime_ = 0.0f;
            isShrinking_ = false;
        } else if (powerUp_ == PowerUpState::Big) {
            powerUp_ = PowerUpState::Fire;
            isColorChanging_ = true;
            colorChangeAnimTime_ = 0.0f;
        }
    }

    void getMushroom() {
        if (powerUp_ == PowerUpState::Small) {
            powerUp_ = PowerUpState::Big;
            recalcJumpPower();
            isGrowing_ = true;
            growAnimTime_ = 0.0f;
            isShrinking_ = false;
        }
    }

    void shrinkPlayer() {
        if (powerUp_ == PowerUpState::Fire) {
            powerUp_ = PowerUpState::Big;
            isColorChanging_ = true;
            colorChangeAnimTime_ = 0.0f;
        } else if (powerUp_ == PowerUpState::Big) {
            powerUp_ = PowerUpState::Small;
            recalcJumpPower();
            isShrinking_ = true;
            shrinkAnimTime_ = 0.0f;
            isGrowing_ = false;
        }
    }

    bool isBig() const { return powerUp_ != PowerUpState::Small; }
    bool isFireMario() const { return powerUp_ == PowerUpState::Fire; }
    PowerUpState powerUpState() const { return powerUp_; }
    
    void die() {
        isDead_ = true;
        currentState = State::Dead;
        currentFrame_ = 0;
        animationTime_ = 0.f;
        powerUp_ = PowerUpState::Small;
        velocity_ = {0.f, 0.f};
    }
    
    bool isDead() const { return isDead_; }
    
    bool isTransforming() const {
        return isGrowing_ || isShrinking_ || isColorChanging_;
    }

    bool isGrowing() const { return isGrowing_; }
    float growAnimTime() const { return growAnimTime_; }

    bool isShrinking() const { return isShrinking_; }
    float shrinkAnimTime() const { return shrinkAnimTime_; }
    
    bool isColorChanging() const { return isColorChanging_; }
    float colorChangeAnimTime() const { return colorChangeAnimTime_; }

    void update(sf::Time timePerFrame) override;

    void moveLeft();
    void moveRight();
    void jump();
    bool consumeJumpEvent();
    void setSpeedMultiplier(float multiplier);
    float speedMultiplier() const;

};
