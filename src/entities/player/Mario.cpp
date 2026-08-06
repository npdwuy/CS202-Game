#include "entities/player/Mario.hpp"
#include <cmath> 

Mario::Mario(sf::Vector2f position): Player(position, "Mario", 292.0f, 830.0f){
    sf::Image image;
    if (!image.loadFromFile("assets/sprites/player/mario.png")) {
        throw std::runtime_error("Failed to load mario image.");
    }
    
    image.createMaskFromColor(sf::Color::White);


    if (!texture_.loadFromImage(image)) {
        throw std::runtime_error("Failed to load texture from image.");
    }

    sprite_.setTexture(texture_);
    
    // Đặt khung hình mặc định ban đầu tránh bị lỗi tàng hình
    sprite_.setTextureRect(sf::IntRect(144, 25, 20, 32)); 
    sprite_.setOrigin(10.f, 32.f);
    sprite_.setScale(1.5f, 1.5f);
    sprite_.setPosition(position.x + width() / 2.0f, position.y + height());
}

void Mario::update(sf::Time timePerFrame) {
    Player::update(timePerFrame);

    // Cú pháp: sf::IntRect(Tọa_độ_X, Tọa_độ_Y, Chiều_Rộng, Chiều_Cao)
    if(fireMario_){
        baseImage_ = 736;
    }    

    // 1. Đứng yên (Lấy khung hình "Stand" chuẩn)
    static const std::vector<sf::IntRect> framesStand = {
         sf::IntRect(160  , 15, 35, 50),
    };
    
    // 2. Đi bộ (Lấy 8 khung hình ở hàng "Walk(8)")
    static const std::vector<sf::IntRect> framesWalk = {
        sf::IntRect(13  , 85, 40, 50),
        sf::IntRect(302  , 85, 40, 50),
        sf::IntRect(54  , 85, 40, 50),
        sf::IntRect(93  , 85, 40, 50),
        sf::IntRect(137  , 85, 40, 50),
        sf::IntRect(178  , 85, 40, 50),
        sf::IntRect(302  , 85, 40, 50),
        sf::IntRect(219  , 85, 40, 50),
        sf::IntRect(259  , 85, 40, 50),
        sf::IntRect(302  , 85, 40, 50)
    };
    
    // 3. Nhảy lên (Lấy khung ở hàng "Jump(2)")
    static const std::vector<sf::IntRect> framesJump = {
        sf::IntRect(22  , 155, 40, 55),
        sf::IntRect(65  , 155, 40, 55)
    };
    
    // 4. Rơi xuống (Lấy khung ở hàng "Fall(3)")
    static const std::vector<sf::IntRect> framesFall = {
        sf::IntRect(115  , 155, 40, 55),
        sf::IntRect(118  , 155, 40, 55),
        sf::IntRect(200  , 155, 40, 55)
    };

    // 5. Nhảy đụng trần
    static const std::vector<sf::IntRect> framesHitRoof = {
        sf::IntRect(250  , 155, 42, 42),
        sf::IntRect(300  , 155, 42, 42)
    };
    
    // 6. Chuyển dần sang đứng yên

    static const std::vector<sf::IntRect> framesTransitionStand = {
        sf::IntRect(24  , 15, 35, 50),
        sf::IntRect(67  , 15, 35, 50),
        sf::IntRect(108  , 12, 35, 50)
    };


    const std::vector<sf::IntRect>* currentAnim = &framesStand;
    if (currentState == State::Walk) currentAnim = &framesWalk;
    else if (currentState == State::Jump) currentAnim = &framesJump;
    else if (currentState == State::Fall) currentAnim = &framesFall;
    else if(currentState == State::HitRoof) currentAnim = &framesHitRoof;
    else if(currentState == State::TransitionStand)currentAnim = &framesTransitionStand;

    const float frameDuration = 0.10f; 
    // animationTime_ += timePerFrame.asSeconds();

    if (animationTime_ >= frameDuration) {
        animationTime_ -= frameDuration;
        if(currentState == State::Jump){
            currentFrame_ = std::min(currentFrame_ + 1, static_cast<int>(currentAnim->size() - 1));
        }
        else
            currentFrame_ = (currentFrame_ + 1) % currentAnim->size();
        // Nhảy đùng trần 2 frame -> rơi xuống 
        if(currentState == State::HitRoof && currentFrame_ == 1)hitRoof_ = false;
    }

    sf::IntRect currentRect = (*currentAnim)[currentFrame_];
    if(fireMario_){
        currentRect.left += baseImage_;
    }
    sprite_.setTextureRect(currentRect);

    // Đặt Origin ở điểm giữa cạnh dưới của hình ảnh
    sprite_.setOrigin(currentRect.width / 2.0f, static_cast<float>(currentRect.height));

    // Lật mặt và xử lý Scale
    float scaleAbs = 1.5f; // Độ to của nhân vật
    if(mushroom_){
        scaleAbs = 1.8f;
    }
    if (velocity_.x < 0.0f) {
        sprite_.setScale(-scaleAbs, scaleAbs);
    } else if (velocity_.x > 0.0f) {
        sprite_.setScale(scaleAbs, scaleAbs);
    }
    
    // Vẽ sprite tại điểm giữa cạnh dưới của khung va chạm (collision box)
    sprite_.setPosition(position_.x + width() / 2.0f, position_.y + height());
}

void Mario::Render(sf::RenderWindow& window) const {
    window.draw(sprite_);
}
