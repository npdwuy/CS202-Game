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
    auto& settings = GameManager::getInstance().getSettings();

<<<<<<< HEAD
    if (sf::Keyboard::isKeyPressed(settings.getKeyBinding("MoveLeft"))
        || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        moveLeft();
    }
    if (sf::Keyboard::isKeyPressed(settings.getKeyBinding("MoveRight"))
        || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        moveRight();
    }
    const bool jumpPressed =
        sf::Keyboard::isKeyPressed(settings.getKeyBinding("MoveUp")) ||
        sf::Keyboard::isKeyPressed(settings.getKeyBinding("Action")) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
=======
    const bool moveLeftPressed =
        sf::Keyboard::isKeyPressed(
            settings.getKeyBinding("MoveLeft")
        ) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left);

    const bool moveRightPressed =
        sf::Keyboard::isKeyPressed(
            settings.getKeyBinding("MoveRight")
        ) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right);

    const bool jumpPressed =
        sf::Keyboard::isKeyPressed(
            settings.getKeyBinding("MoveUp")
        ) ||
        sf::Keyboard::isKeyPressed(
            settings.getKeyBinding("Action")
        ) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up);

    if (moveLeftPressed)
    {
        moveLeft();
    }

    if (moveRightPressed)
    {
        moveRight();
    }
>>>>>>> 9d9b00d (feat(player): support arrow key movement controls)
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
    moveCharacter(timePerFrame);


    State newState = State::Stand;
    if(hitRoof_ == true)newState = State::HitRoof;
    else if (velocity_.y < 0.0f) {
        newState = State::Jump;
    } else if (velocity_.y > 0.0f) {
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
    animationTime_+=timePerFrame.asSeconds();
    movedThisFrame_ = false;

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
}
