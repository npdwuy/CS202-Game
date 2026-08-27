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

    bool isDamageTransforming_ = false;
    float damageTransformAnimTime_ = 0.0f;

    bool invincible_ = false;
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

    bool isPendingTransformation_ = false;
    PowerUpState pendingPowerUpState_ = PowerUpState::Small;

    // Death state variables
    bool isDead_ = false;
    float deathAnimTime_ = 0.0f;
    float deathVelocityY_ = -500.f;
    static constexpr float DeathHopDelay = 0.5f;

    // Jump power anchoring (prevents compounding)
    float baseJumpPower_ = 670.0f;
    static constexpr float BigJumpBonus = 207.5f;

    float throwTimer_ = 0.0f;

    // Boss knockback timer
    float bossKnockbackTimer_ = 0.0f;
    float bossKnockbackDuration_ = 0.45f;
    float bossKnockbackDir_ = 0.0f;

    void performJump();

public:
    void triggerThrow() {
        throwTimer_ = 0.15f;
    }

    void triggerBossKnockback(float pushDirection, float duration = 0.7f) {
        bossKnockbackTimer_ = duration;
        bossKnockbackDuration_ = duration;
        bossKnockbackDir_ = pushDirection;
        velocity_.x = pushDirection * 750.f;
        velocity_.y = -620.f;
        onGround_ = false;
        currentState = State::Jump;
    }

    bool isBossKnockbackActive() const { return bossKnockbackTimer_ > 0.0f; }

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
          
    bool isSuper() const { return powerUp_ != PowerUpState::Small; }
    bool isInvincible() const { return invincible_; }
    void setInvincible(bool invincible) { invincible_ = invincible; }

    std:: string name() const override{
        return label_;
    }

    void recalcJumpPower() {
        jumpPower_ = baseJumpPower_ + (powerUp_ != PowerUpState::Small ? BigJumpBonus : 0.f);
    }

    void up2Fire() {
        if (powerUp_ == PowerUpState::Small) {
            pendingPowerUpState_ = PowerUpState::Fire;
        } else if (powerUp_ == PowerUpState::Big) {
            pendingPowerUpState_ = PowerUpState::Fire;
        }
        isPendingTransformation_ = true;
    }

    void getMushroom() {
        if (powerUp_ == PowerUpState::Small) {
            pendingPowerUpState_ = PowerUpState::Big;
            isPendingTransformation_ = true;
        }
    }

    void applyPendingPowerUp() {
        if (pendingPowerUpState_ == PowerUpState::Big && powerUp_ == PowerUpState::Small) {
            powerUp_ = PowerUpState::Big;
            recalcJumpPower();
            isGrowing_ = true;
            growAnimTime_ = 0.f;
            isShrinking_ = false;
        } else if (pendingPowerUpState_ == PowerUpState::Fire) {
            if (powerUp_ == PowerUpState::Small) {
                powerUp_ = PowerUpState::Fire;
                recalcJumpPower();
                isGrowing_ = true;
                growAnimTime_ = 0.f;
                isShrinking_ = false;
            } else {
                powerUp_ = PowerUpState::Fire;
                isColorChanging_ = true;
                colorChangeAnimTime_ = 0.f;
            }
        }
    }

    void shrinkPlayer() {
        isDamageTransforming_ = true;
        damageTransformAnimTime_ = 0.0f;
        
        if (powerUp_ == PowerUpState::Fire) {
            powerUp_ = PowerUpState::Big;
            isColorChanging_ = true; // Use this to flash colors
            colorChangeAnimTime_ = 0.0f;
        } else if (powerUp_ == PowerUpState::Big) {
            powerUp_ = PowerUpState::Small;
            recalcJumpPower();
            isShrinking_ = true; // Use this to shrink scale
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
        return isGrowing_ || isColorChanging_ || isDamageTransforming_;
    }
    
    bool isPendingTransformation() const { return isPendingTransformation_; }
    bool isDamageShrinking() const { return isShrinking_; }

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
