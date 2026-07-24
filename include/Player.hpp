#include "Character.hpp"

class Player : public Character {
protected:
    int coyoteTimer_ = 0.0f;
    int currentFrame_ = 0;
    float speed_ = 280.0f;
    float jumpPower_ = 660.0f;
    float animationTime_ = 0.0f;
    bool movedThisFrame_ = false;
    bool jumpBufferTimer_ = false;
    std::string label_;
    void performJump();
public:
    Player(sf:: Vector2f position, std::string label, float speed, float jumpPower)
        : Character(position, 27.0f, 30.0f),
          speed_(speed), jumpPower_(jumpPower),
          label_(std::move(label)) {}

    std:: string name() const override{
        return label_;
    }

    void update(sf::Time timePerFrame) override;

    void moveLeft();
    void moveRight();
    void jump();

};