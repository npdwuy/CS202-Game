#pragma once
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <string>

class Entity{
protected:
    sf::Vector2f position_;
    sf::Vector2f velocity_;
    float width_;
    float height_;
    bool active_ = true;
public:
    Entity(sf:: Vector2f position, float width = 1.0f, float height = 1.0f) : position_(position), width_(width), height_(height){}

    virtual ~Entity() = default;
    virtual std::string name()const = 0;
    virtual void Render(sf::RenderWindow& window) const = 0;
    virtual void update(sf::Time timePerFrame) = 0;

    sf::Vector2f position()const{
        return position_;
    }
    sf::Vector2f velocity()const{
        return velocity_;
    }
    bool active()const{
        return active_;
    }
    void deactive(){
        active_ = false;
    }
    void setPosition(sf::Vector2f position){
        position_= position;
    }
    void setVelocity(sf::Vector2f velocity){
        velocity_ = velocity;
    }
    float width() const{
        return width_;
    }
    float height() const{
        return height_;
    }
};
