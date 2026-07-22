#include "Player.hpp"

void Player:: draw()const{

}

void Player:: update() {

}

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