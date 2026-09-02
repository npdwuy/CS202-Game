#include "entities/player/Mario.hpp"
#include "entities/player/PlayerSpriteLoader.hpp"
#include <cmath> 

namespace {
// HSV to RGB conversion for rainbow effect
// h: 0-360, s: 0-1, v: 0-1
sf::Color hsvToRgb(float h, float s, float v) {
    h = std::fmod(h, 360.f);
    if (h < 0.f) h += 360.f;
    
    float c = v * s;
    float x = c * (1.f - std::fabs(std::fmod(h / 60.f, 2.f) - 1.f));
    float m = v - c;
    
    float r = 0.f, g = 0.f, b = 0.f;
    if (h < 60.f)       { r = c; g = x; b = 0.f; }
    else if (h < 120.f) { r = x; g = c; b = 0.f; }
    else if (h < 180.f) { r = 0.f; g = c; b = x; }
    else if (h < 240.f) { r = 0.f; g = x; b = c; }
    else if (h < 300.f) { r = x; g = 0.f; b = c; }
    else                { r = c; g = 0.f; b = x; }
    
    return sf::Color(
        static_cast<sf::Uint8>((r + m) * 255.f),
        static_cast<sf::Uint8>((g + m) * 255.f),
        static_cast<sf::Uint8>((b + m) * 255.f)
    );
}
} // anonymous namespace

Mario::Mario(sf::Vector2f position): Player(position, "Mario", 292.0f, 830.0f){
    sf::Image image = PlayerSpriteLoader::loadCompositedSpriteSheet(PlayerSpriteLoader::CharacterType::Mario);

    if (!texture_.loadFromImage(image)) {
        throw std::runtime_error("Failed to load texture from image.");
    }

    sprite_.setTexture(texture_);
    
    // Đặt khung hình mặc định ban đầu tránh bị lỗi tàng hình
    sprite_.setTextureRect(sf::IntRect(160, 15, 35, 47)); 
    sprite_.setOrigin(17.5f, 47.f);
    sprite_.setScale(1.5f, 1.5f);
    sprite_.setPosition(position.x + width() / 2.0f, position.y + height());
}

void Mario::update(sf::Time timePerFrame) {
    Player::update(timePerFrame);

    // Cú pháp: sf::IntRect(Tọa_độ_X, Tọa_độ_Y, Chiều_Rộng, Chiều_Cao)

    // 1. Đứng yên (Lấy khung hình "Stand" chuẩn)
    static const std::vector<sf::IntRect> framesStand = {
         sf::IntRect(160  , 15, 35, 47),
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
        sf::IntRect(22  , 155, 40, 52),
        sf::IntRect(65  , 155, 40, 52)
    };
    
    // 4. Rơi xuống (Lấy khung ở hàng "Fall(3)")
    static const std::vector<sf::IntRect> framesFall = {
        sf::IntRect(115  , 155, 40, 52),
        sf::IntRect(118  , 155, 40, 52),
        sf::IntRect(200  , 155, 40, 52)
    };

    // 5. Nhảy đụng trần
    static const std::vector<sf::IntRect> framesHitRoof = {
        sf::IntRect(250  , 155, 42, 39),
        sf::IntRect(300  , 155, 42, 39)
    };
    
    // 6. Chuyển dần sang đứng yên
    static const std::vector<sf::IntRect> framesTransitionStand = {
        sf::IntRect(24  , 15, 35, 47),
        sf::IntRect(67  , 15, 35, 47),
        sf::IntRect(110 , 15, 35, 47)
    };
    
    // 7. Cầm rùa đứng yên
    static const std::vector<sf::IntRect> framesHold = {
        sf::IntRect(21, 295, 32, 53)
    };
    
    // 8. Cầm rùa chạy
    static const std::vector<sf::IntRect> framesHoldWalk = {
        sf::IntRect(0,   365, 32, 53),
        sf::IntRect(40,  365, 41, 48),
        sf::IntRect(88,  365, 38, 49),
        sf::IntRect(138, 365, 34, 52),
        sf::IntRect(186, 365, 32, 53),
        sf::IntRect(227, 365, 43, 48),
        sf::IntRect(275, 365, 40, 48),
        sf::IntRect(325, 365, 34, 52)
    };

    static const std::vector<sf::IntRect> framesPoleSlide = {
        sf::IntRect(22, 155, 40, 52) // Jump frame looks like holding pole
    };


    // 7. Chết (Flashing and spinning)
    static const std::vector<sf::IntRect> framesDead = {
        sf::IntRect(561, 85, 40, 50),
        sf::IntRect(603, 85, 40, 50),
        sf::IntRect(645, 85, 40, 50),
        sf::IntRect(686, 85, 40, 50)
    };

    const std::vector<sf::IntRect>* currentAnim = &framesStand;
    if (currentState == State::Dead) currentAnim = &framesDead;
    else if (currentState == State::Walk || currentState == State::AutoWalk) currentAnim = &framesWalk;
    else if (currentState == State::Jump) currentAnim = &framesJump;
    else if (currentState == State::Fall) currentAnim = &framesFall;
    else if(currentState == State::HitRoof) currentAnim = &framesHitRoof;
    else if(currentState == State::TransitionStand) currentAnim = &framesTransitionStand;
    else if(currentState == State::PoleSlide) currentAnim = &framesPoleSlide;

    if (isCarrying() && currentState != State::Dead) {
        if (currentState == State::Walk || currentState == State::AutoWalk) {
            currentAnim = &framesHoldWalk;
        } else {
            currentAnim = &framesHold;
        }
    }

    const float frameDuration = (currentState == State::Dead) ? 0.07f : 0.10f;
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

    if (currentFrame_ >= currentAnim->size()) {
        currentFrame_ = 0;
    }
    sf::IntRect currentRect = (*currentAnim)[currentFrame_];
    
    // Xử lý đè frame ném lửa nếu đang ném
    if (throwTimer_ > 0.f) {
        throwTimer_ -= timePerFrame.asSeconds();
        // Animation 2 khung hình để trông mượt hơn
        if (throwTimer_ > 0.075f) {
            currentRect = sf::IntRect(93, 85, 40, 50);  // Giơ tay
        } else {
            currentRect = sf::IntRect(137, 85, 40, 50); // Bung tay
        }
    }
    
    static constexpr int FireMarioSpriteOffsetX = 736;
    
    if (isFireMario()) {
        baseImage_ = FireMarioSpriteOffsetX;
    } else {
        baseImage_ = 0;
    }
    
    if (isColorChanging()) {
        // Flash between normal and fire sprite at ~10Hz
        int flashFrame = static_cast<int>(colorChangeAnimTime() / 0.05f);
        if (flashFrame % 2 == 0) {
            baseImage_ = FireMarioSpriteOffsetX;
        } else {
            baseImage_ = 0;
        }
    }

    if (currentState != State::Dead) {
        currentRect.left += baseImage_;
    }
    sprite_.setTextureRect(currentRect);

    // Đặt Origin ở điểm giữa cạnh dưới của hình ảnh
    sprite_.setOrigin(currentRect.width / 2.0f, static_cast<float>(currentRect.height));

    // Lật mặt và xử lý Scale
    float baseScale = 1.425f;
    float scaleAbs = baseScale; // Độ to của nhân vật

    if (isGrowing()) {
        struct Keyframe {
            float time;
            float multiplier;
        };
        static const std::vector<Keyframe> growKeyframes = {
            {0.00f, 1.00f},
            {0.05f, 1.05f},
            {0.25f, 1.05f},
            {0.30f, 1.15f},
            {0.50f, 1.15f},
            {0.55f, 1.22f},
            {0.75f, 1.22f},
            {0.80f, 1.275f},
            {1.00f, 1.275f}
        };
        
        float currentMult = 1.275f;
        float animTime = growAnimTime();
        for (size_t i = 0; i < growKeyframes.size() - 1; ++i) {
            if (animTime >= growKeyframes[i].time && animTime <= growKeyframes[i+1].time) {
                float t = (animTime - growKeyframes[i].time) / (growKeyframes[i+1].time - growKeyframes[i].time);
                currentMult = growKeyframes[i].multiplier + t * (growKeyframes[i+1].multiplier - growKeyframes[i].multiplier);
                break;
            }
        }
        scaleAbs = baseScale * currentMult;
    } else if (isShrinking()) {
        struct Keyframe {
            float time;
            float multiplier;
        };
        static const std::vector<Keyframe> shrinkKeyframes = {
            {0.00f, 1.275f},
            {0.05f, 1.22f},
            {0.25f, 1.22f},
            {0.30f, 1.15f},
            {0.50f, 1.15f},
            {0.55f, 1.05f},
            {0.75f, 1.05f},
            {0.80f, 1.00f},
            {1.00f, 1.00f}
        };
        
        float currentMult = 1.00f;
        float animTime = shrinkAnimTime();
        for (size_t i = 0; i < shrinkKeyframes.size() - 1; ++i) {
            if (animTime >= shrinkKeyframes[i].time && animTime <= shrinkKeyframes[i+1].time) {
                float t = (animTime - shrinkKeyframes[i].time) / (shrinkKeyframes[i+1].time - shrinkKeyframes[i].time);
                currentMult = shrinkKeyframes[i].multiplier + t * (shrinkKeyframes[i+1].multiplier - shrinkKeyframes[i].multiplier);
                break;
            }
        }
        scaleAbs = baseScale * currentMult;
    } else if (isBig()) {
        scaleAbs = baseScale * 1.275f;
    }

    // Dynamic bounding box adjustment to prevent spilling (uses constant reference sizes to prevent jitter)
    float oldWidth = width_;
    float oldHeight = height_;
    
    // Keep horizontal collision width invariant to prevent floating over gaps when big
    width_ = CollisionWidth; 
    
    float topPadding = 2.0f; // Transparent pixels at the top of the sprite frames
    height_ = (47.0f - topPadding) * scaleAbs;
    
    position_.x += (oldWidth - width_) / 2.0f;
    position_.y += oldHeight - height_;

    if (facing() < 0) {
        sprite_.setScale(-scaleAbs, scaleAbs);
    } else {
        sprite_.setScale(scaleAbs, scaleAbs);
    }
    
    // Vẽ sprite tại điểm giữa cạnh dưới của khung va chạm (collision box)
    sprite_.setPosition(position_.x + width() / 2.0f, position_.y + height());
}

void Mario::Render(sf::RenderWindow& window) const {
    if (invincible_) {
        // Rainbow color cycling effect - 720 degrees per second (2 full cycles/sec)
        float hue = std::fmod(animationTime_ * 720.f, 360.f);
        sf::Color rainbow = hsvToRgb(hue, 0.85f, 1.0f);
        // Use mutable cast since setColor doesn't change logical state
        const_cast<sf::Sprite&>(sprite_).setColor(rainbow);
        window.draw(sprite_);
        const_cast<sf::Sprite&>(sprite_).setColor(sf::Color::White);
    } else {
        window.draw(sprite_);
    }
}