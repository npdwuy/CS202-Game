#include "Character.hpp"

class Player : public Character {
private:
    float speed_;
    float jumpPower_;
    bool movedThisFrame_ = false;
    bool jumpBufferTimer_ = false;
    std::string label_;
    void performJump();
public:
    Player(sf:: Vector2f position, float width, float height, float speed, float jumpPower, std::string label)
        : Character(position, width, height),
          speed_(speed), jumpPower_(jumpPower),
          label_(std::move(label)) {}

    void draw() const override;
    void update() override;
    std:: string name() const override{
        return label_;
    }

    void moveLeft();
    void moveRight();
    void jump();
    
};