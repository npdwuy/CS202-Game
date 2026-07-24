#include "entities/player/Player.hpp"


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
}

void Player:: update(sf::Time timePerFrame){

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        moveLeft();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        moveRight();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)
        ||sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
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
    movedThisFrame_ = false;
    jumpBufferTimer_ = std::max(0.0f, jumpBufferTimer_ - timePerFrame.asSeconds());
    const float gravityScale = velocity_.y > 0.0f ? 0.88f : 1.0f;
    velocity_.y = std::min(980.0f, velocity_.y + 1850.0f * gravityScale * timePerFrame.asSeconds());
    moveCharacter(timePerFrame);
    animationTime_+=timePerFrame.asSeconds();
}