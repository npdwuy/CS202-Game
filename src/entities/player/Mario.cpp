#include "entities/player/Mario.hpp"
#include <cmath> 

Mario::Mario(sf::Vector2f position): Player(position, "Mario", 292.0f, 720.0f){
    if (!texture_.loadFromFile("assets/sprites/player/mario.png")) {
        throw std::runtime_error("Failed to load mario sprite.");
    }
    sprite_.setTexture(texture_);
    
    // Đặt khung hình mặc định ban đầu tránh bị lỗi tàng hình
    sprite_.setTextureRect(sf::IntRect(144, 25, 20, 32)); 
    sprite_.setOrigin(0.f, 0.f);
    sprite_.setScale(3.f, 3.f);
    sprite_.setPosition(position);
}

void Mario::update(sf::Time timePerFrame) {
    Player::update(timePerFrame);

    // Cú pháp: sf::IntRect(Tọa_độ_X, Tọa_độ_Y, Chiều_Rộng, Chiều_Cao)
    

    // 1. Đứng yên (Lấy khung hình "Stand" chuẩn)
    std::vector<sf::IntRect> framesStand = {
        sf::IntRect(144, 25, 20, 32)
    };
    
    // 2. Đi bộ (Lấy 8 khung hình ở hàng "Walk(8)")
    std::vector<sf::IntRect> framesWalk = {
        sf::IntRect(8, 70, 20, 30),
        sf::IntRect(34, 70, 20, 30),
        sf::IntRect(62, 70, 20, 30),
        sf::IntRect(88, 70, 20, 30),
        sf::IntRect(114, 70, 20, 30),
        sf::IntRect(140, 70, 20, 30),
        sf::IntRect(166, 70, 20, 30),
        sf::IntRect(192, 70, 20, 30)
    };
    
    // 3. Nhảy lên (Lấy khung ở hàng "Jump(2)")
    std::vector<sf::IntRect> framesJump = {
        sf::IntRect(8, 120, 22, 32), 
        sf::IntRect(35, 120, 22, 32)
    };
    
    // 4. Rơi xuống (Lấy khung ở hàng "Fall(3)")
    std::vector<sf::IntRect> framesFall = {
        sf::IntRect(65, 120, 24, 30),
        sf::IntRect(92, 120, 24, 30)
    };

    // 5. Nhảy đụng trần
    std::vector<sf::IntRect> framesHitRoof = {
        sf::IntRect(92, 120, 24, 30)
    };

    // --- KẾT THÚC BẢNG TỌA ĐỘ ---

    std::vector<sf::IntRect>* currentAnim = &framesStand;
    if (currentState == State::Walk) currentAnim = &framesWalk;
    else if (currentState == State::Jump) currentAnim = &framesJump;
    else if (currentState == State::Fall) currentAnim = &framesFall;
    else if(currentState == State::HitRoof) currentAnim = &framesHitRoof;

    const float frameDuration = 0.10f; 
    // animationTime_ += timePerFrame.asSeconds();

    if (animationTime_ >= frameDuration) {
        animationTime_ -= frameDuration;
        currentFrame_ = (currentFrame_ + 1) % currentAnim->size();
        // Nhảy đùng trần 2 frame -> rơi xuống 
        if(currentState == State::HitRoof && currentFrame_ == 2)hitRoof_ = false;
    }

    sf::IntRect currentRect = (*currentAnim)[currentFrame_];
    sprite_.setTextureRect(currentRect);

    // Lật mặt và xử lý Scale
    const float scaleAbs = 3.0f; // Độ to của nhân vật (3x)
    if (velocity_.x < 0.0f) {
        sprite_.setOrigin(static_cast<float>(currentRect.width), 0.f);
        sprite_.setScale(-scaleAbs, scaleAbs);
    } else if (velocity_.x > 0.0f) {
        sprite_.setOrigin(0.f, 0.f);
        sprite_.setScale(scaleAbs, scaleAbs);
    }
    
    sprite_.setPosition(position_);
}

void Mario::Render(sf::RenderWindow& window) const {
    window.draw(sprite_);
}