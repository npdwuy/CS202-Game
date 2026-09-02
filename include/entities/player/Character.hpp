#pragma once
#include "Entity.hpp"
#include <functional>
#include <utility>

class Character: public Entity{
public:
    using CollisionResolver = std::function<void(Character&, sf::Time)>;

protected:
    int facing_ = 1;
    bool onGround_ = false;
    bool hitRoof_ = false;
    CollisionResolver collisionResolver_;
public:
    
    Character(sf:: Vector2f position, float width, float height) : Entity(position, width, height){}

    bool onGround() const{
        return onGround_;
    }

    void setOnGround(bool onGround){
        onGround_ = onGround;
    }

    void setHitRoof(bool hitRoof){
        hitRoof_ = hitRoof;
    }

    void setCollisionResolver(CollisionResolver resolver){
        collisionResolver_ = std::move(resolver);
    }

    void clearCollisionResolver(){
        collisionResolver_ = nullptr;
    }

    int facing() const{
        return facing_;
    }

    void setFacing(int facing){
        facing_ = facing < 0 ? -1 : 1;
    }

    void moveCharacter(sf::Time dt){
        if (collisionResolver_) {
            collisionResolver_(*this, dt);
            return;
        }
        position_ += velocity_ * dt.asSeconds();
    }

};
