#include "Entity.hpp"

class Character: public Entity{
protected:
    int facing_ = 1;
    bool onGround_ = false;
public:
    
    Character(sf:: Vector2f position, float width, float height) : Entity(position, width, height){}

    bool onGround() const{
        return onGround_;
    }

    void setOnGround(bool onGround){
        onGround_ = onGround;
    }

    int facing() const{
        return facing_;
    }

    void setFacing(int facing){
        facing_ = facing < 0 ? -1 : 1;
    }
};