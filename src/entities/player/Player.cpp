#include "entities/player/Player.hpp"

#include <algorithm>
#include <cmath>

void Player:: moveLeft(){
    velocity_.x = - speed_;
    facing_ = -1;
    movedThisFrame_ = true;
}

void Player:: moveRight(){
    velocity_.x = speed_;
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

#include "GameManager.hpp"

void Player:: update(sf::Time timePerFrame){
    auto& settings = GameManager::getInstance().getSettings();

    if (sf::Keyboard::isKeyPressed(settings.getKeyBinding("MoveLeft"))) {
        moveLeft();
    }
    if (sf::Keyboard::isKeyPressed(settings.getKeyBinding("MoveRight"))) {
        moveRight();
    }
    if (sf::Keyboard::isKeyPressed(settings.getKeyBinding("MoveUp")) || 
        sf::Keyboard::isKeyPressed(settings.getKeyBinding("Action"))) {
        jump();
    }
    
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
        velocity_.x *=0.78f;
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
}
