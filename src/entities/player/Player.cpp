#include "entities/player/Player.hpp"
#include "GameManager.hpp"
#include <algorithm>
#include <cmath>

void Player:: moveLeft(){
    velocity_.x = -speed_ * speedMultiplier_;
    facing_ = -1;
    movedThisFrame_ = true;
}

void Player:: moveRight(){
    velocity_.x = speed_ * speedMultiplier_;
    facing_ = 1;
    movedThisFrame_ = true;
}

void Player:: jump(){
    jumpBufferTimer_ = 0.14f;
}

void Player:: performJump(){
    velocity_.y = -jumpPower_;
    onGround_ = false;
    jumpBufferTimer_ = 0.0f;
    jumpedThisFrame_ = true;
}

bool Player::consumeJumpEvent(){
    const bool jumped = jumpedThisFrame_;
    jumpedThisFrame_ = false;
    return jumped;
}

void Player::setSpeedMultiplier(float multiplier) {
    speedMultiplier_ = std::clamp(multiplier, 0.5f, 2.0f);
}

float Player::speedMultiplier() const {
    return speedMultiplier_;
}



void Player:: update(sf::Time timePerFrame){
    if (isDead_) {
        deathAnimTime_ += timePerFrame.asSeconds();
        if (deathAnimTime_ > DeathHopDelay) {
            deathVelocityY_ += 1500.f * timePerFrame.asSeconds();
            position_.y += deathVelocityY_ * timePerFrame.asSeconds();
        }
        animationTime_ += timePerFrame.asSeconds();
        return; // skip input, normal physics, and bounding box logic
    }

    if (isTransforming()) {
        if (isGrowing_) {
            growAnimTime_ += timePerFrame.asSeconds();
            if (growAnimTime_ >= GrowAnimDuration) {
                isGrowing_ = false;
                growAnimTime_ = 0.0f;
            }
        }
        if (isShrinking_) {
            shrinkAnimTime_ += timePerFrame.asSeconds();
            if (shrinkAnimTime_ >= ShrinkAnimDuration) {
                isShrinking_ = false;
                shrinkAnimTime_ = 0.0f;
            }
        }
        if (isColorChanging_) {
            colorChangeAnimTime_ += timePerFrame.asSeconds();
            if (colorChangeAnimTime_ >= ColorChangeAnimDuration) {
                isColorChanging_ = false;
                colorChangeAnimTime_ = 0.0f;
            }
        }
        return; // Skip input and physics while transforming
    }

    auto& settings = GameManager::getInstance().getSettings();

    bool jumpPressed = false;
    if (inputEnabled_) {
        if (sf::Keyboard::isKeyPressed(settings.getKeyBinding("MoveLeft"))
            || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            moveLeft();
        }

        if (sf::Keyboard::isKeyPressed(settings.getKeyBinding("MoveRight"))
            || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            moveRight();
        }

        jumpPressed =
            sf::Keyboard::isKeyPressed(settings.getKeyBinding("MoveUp")) ||
            sf::Keyboard::isKeyPressed(settings.getKeyBinding("Action")) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Space) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
    }
    
    if (jumpPressed && !jumpHeld_) {
        jump();
    }
    jumpHeld_ = jumpPressed;
    
    if(onGround_){
        coyoteTimer_ = 0.12f;
    }
    else{
        coyoteTimer_ = std::max(0.0f, coyoteTimer_ - timePerFrame.asSeconds());
    }

    if(jumpBufferTimer_ > 0.0f && (onGround_ || coyoteTimer_ > 0.0f)){
        performJump();
    }

    if (inputEnabled_) {
        if(!movedThisFrame_){
            constexpr float ReferenceFrameSeconds = 1.f / 30.f;
            constexpr float ReferenceDamping = 0.78f;
            const float damping = std::pow(
                ReferenceDamping,
                timePerFrame.asSeconds() / ReferenceFrameSeconds
            );
            velocity_.x *= damping;
            if(std::abs(velocity_.x) < 7.0f){
                velocity_.x = 0.0f;
            }
        }
        
        jumpBufferTimer_ = std::max(0.0f, jumpBufferTimer_ - timePerFrame.asSeconds());
        const float gravityScale = velocity_.y > 0.0f ? 1.0f : 0.88f ;
        velocity_.y = std::min(980.0f, velocity_.y + 1850.0f * gravityScale * timePerFrame.asSeconds());
    } else {
        if (currentState != State::PoleSlide && currentState != State::AutoWalk) {
            // Apply gravity but no friction (PlayState controls velocity.x)
            const float gravityScale = velocity_.y > 0.0f ? 1.0f : 0.88f ;
            velocity_.y = std::min(980.0f, velocity_.y + 1850.0f * gravityScale * timePerFrame.asSeconds());
        }
    }

    moveCharacter(timePerFrame);

    if (inputEnabled_) {
        State newState = State::Stand;
        if(hitRoof_ == true)newState = State::HitRoof;
        else if (velocity_.y < 0.0f && !onGround_) {
            newState = State::Jump;
        } else if (velocity_.y > 0.0f && !onGround_) {
            newState = State::Fall;
        } else if (std::abs(velocity_.x) > 0.1f) { 
            if(movedThisFrame_)
                newState = State::Walk;
            else newState = State::TransitionStand;
        }

        if (newState != currentState) {
            currentState = newState;
            currentFrame_ = 0;
            animationTime_ = 0.f;
        }
    }
    
    animationTime_+=timePerFrame.asSeconds();
    movedThisFrame_ = false;
}
