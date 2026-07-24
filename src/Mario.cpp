#include "Mario.hpp"

Mario::Mario(sf::Vector2f position): Player(position, "Mario", 292.0f, 720.0f){
    if (!texture_.loadFromFile(
            "assets/sprites/player/mario.png"))
    {
        throw std::runtime_error(
            "Failed to load mario(walk) sprite."
        );
    }
    sprite_.setTexture(texture_);
}

void Mario::update(sf::Time timePerFrame){
    Character::update(timePerFrame);

    const float frameDuration = 0.18f;

    if (animationTime_ >= frameDuration){

        if(velocity_.y){
                if(velocity_.y < 0.0f){
                animationTime_ = 0.f;
                currentFrame_ = (currentFrame_ + 1) % 8;

                sprite_.setTextureRect(
                    sf::IntRect(currentFrame_ * 16, 0, 16, 16)
                );
            }   
            if(velocity_.y > 0.0f){
                animationTime_ = 0.f;
                currentFrame_ = (currentFrame_ + 1) % 8;

                sprite_.setTextureRect(
                    sf::IntRect(currentFrame_ * 16, 0, 16, 16)
                );
            }
        }
        else if(velocity_.x){
            animationTime_ = 0.f;
            currentFrame_ = (currentFrame_ + 1) % 8;

            sprite_.setTextureRect(
                sf::IntRect(currentFrame_ * 16, 0, 16, 16)
            );
        }
        else{
            animationTime_ = 0.f;
            currentFrame_ = (currentFrame_ + 1) % 8;

            sprite_.setTextureRect(
                sf::IntRect(currentFrame_ * 16, 0, 16, 16)
            );
        }
    }

}

void Mario::draw(sf::RenderWindow& window)const{
    window.draw(sprite_);
}