#include "entities/player/Luigi.hpp"
#include "entities/player/PlayerSpriteLoader.hpp"
#include <cmath>

namespace {
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

// Luigi stats:
//   Mario:  speed=292, jumpPower=830
//   Luigi:  speed=200 (slower -20%), jumpPower=1040.75 (jumps +15% higher)
Luigi::Luigi(sf::Vector2f position)
    : Player(position, "Luigi", 200.0f, 1040.75f)
{
    sf::Image image = PlayerSpriteLoader::loadCompositedSpriteSheet(PlayerSpriteLoader::CharacterType::Luigi);

    if (!texture_.loadFromImage(image)) {
        throw std::runtime_error("Failed to create Luigi texture from image.");
    }

    sprite_.setTexture(texture_);
    sprite_.setTextureRect(sf::IntRect(160, 15, 35, 47));
    sprite_.setOrigin(17.5f, 47.f);
    sprite_.setScale(1.2f, 1.65f); // ốm đi 20% (1.5 * 0.8), cao hơn 10% (1.5 * 1.10)
    sprite_.setPosition(position.x + width() / 2.0f, position.y + height());
}

void Luigi::update(sf::Time timePerFrame) {
    Player::update(timePerFrame);

    // ── Animation frames (same layout as Mario sheet) ────────────────────────
    static const std::vector<sf::IntRect> framesStand          = { sf::IntRect(160, 15, 35, 47) };
    static const std::vector<sf::IntRect> framesWalk           = {
        sf::IntRect(13,  85, 40, 50), sf::IntRect(302, 85, 40, 50),
        sf::IntRect(54,  85, 40, 50), sf::IntRect(93,  85, 40, 50),
        sf::IntRect(137, 85, 40, 50), sf::IntRect(178, 85, 40, 50),
        sf::IntRect(302, 85, 40, 50), sf::IntRect(219, 85, 40, 50),
        sf::IntRect(259, 85, 40, 50), sf::IntRect(302, 85, 40, 50)
    };
    static const std::vector<sf::IntRect> framesJump           = {
        sf::IntRect(22,  155, 40, 52), sf::IntRect(65, 155, 40, 52)
    };
    static const std::vector<sf::IntRect> framesFall           = {
        sf::IntRect(115, 155, 40, 52), sf::IntRect(118, 155, 40, 52),
        sf::IntRect(200, 155, 40, 52)
    };
    static const std::vector<sf::IntRect> framesHitRoof        = {
        sf::IntRect(250, 155, 42, 39), sf::IntRect(300, 155, 42, 39)
    };
    static const std::vector<sf::IntRect> framesTransitionStand = {
        sf::IntRect(24, 15, 35, 47), sf::IntRect(67, 15, 35, 47),
        sf::IntRect(108, 12, 35, 50)
    };
    
    // Cầm rùa đứng yên
    static const std::vector<sf::IntRect> framesHold = {
        sf::IntRect(18, 295, 32, 57)
    };
    
    // Cầm rùa chạy
    static const std::vector<sf::IntRect> framesHoldWalk = {
        sf::IntRect(6,   365, 33, 53),
        sf::IntRect(47,  365, 41, 48),
        sf::IntRect(95,  365, 38, 49),
        sf::IntRect(145, 365, 34, 52),
        sf::IntRect(193, 365, 32, 53),
        sf::IntRect(234, 365, 43, 48),
        sf::IntRect(282, 365, 40, 48),
        sf::IntRect(332, 365, 34, 52)
    };

    static const std::vector<sf::IntRect> framesPoleSlide      = { sf::IntRect(22, 155, 40, 52) };
    static const std::vector<sf::IntRect> framesDead           = {
        sf::IntRect(561, 85, 40, 50), sf::IntRect(603, 85, 40, 50),
        sf::IntRect(645, 85, 40, 50), sf::IntRect(686, 85, 40, 50)
    };

    const std::vector<sf::IntRect>* currentAnim = &framesStand;
    if      (currentState == State::Dead)                                   currentAnim = &framesDead;
    else if (currentState == State::Walk || currentState == State::AutoWalk) currentAnim = &framesWalk;
    else if (currentState == State::Jump)                                   currentAnim = &framesJump;
    else if (currentState == State::Fall)                                   currentAnim = &framesFall;
    else if (currentState == State::HitRoof)                                currentAnim = &framesHitRoof;
    else if (currentState == State::TransitionStand)                        currentAnim = &framesTransitionStand;
    else if (currentState == State::PoleSlide)                              currentAnim = &framesPoleSlide;

    if (isCarrying() && currentState != State::Dead) {
        if (currentState == State::Walk || currentState == State::AutoWalk) {
            currentAnim = &framesHoldWalk;
        } else {
            currentAnim = &framesHold;
        }
    }

    const float frameDuration = (currentState == State::Dead) ? 0.07f : 0.10f;

    if (animationTime_ >= frameDuration) {
        animationTime_ -= frameDuration;
        if (currentState == State::Jump)
            currentFrame_ = std::min(currentFrame_ + 1, static_cast<int>(currentAnim->size() - 1));
        else
            currentFrame_ = (currentFrame_ + 1) % currentAnim->size();
        if (currentState == State::HitRoof && currentFrame_ == 1) hitRoof_ = false;
    }
    if (currentFrame_ >= static_cast<int>(currentAnim->size())) currentFrame_ = 0;

    sf::IntRect currentRect = (*currentAnim)[currentFrame_];

    // Throw override
    if (throwTimer_ > 0.f) {
        throwTimer_ -= timePerFrame.asSeconds();
        currentRect = (throwTimer_ > 0.075f)
            ? sf::IntRect(93, 85, 40, 50)
            : sf::IntRect(137, 85, 40, 50);
    }

    static constexpr int FireOffset = 736;
    if (isFireMario()) baseImage_ = FireOffset; else baseImage_ = 0;

    if (isColorChanging()) {
        int flashFrame = static_cast<int>(colorChangeAnimTime() / 0.05f);
        baseImage_ = (flashFrame % 2 == 0) ? FireOffset : 0;
    }

    if (currentState != State::Dead) currentRect.left += baseImage_;
    sprite_.setTextureRect(currentRect);
    sprite_.setOrigin(currentRect.width / 2.0f, static_cast<float>(currentRect.height));

    // Scale
    float baseScale = 1.425f;
    float scaleAbs  = baseScale;

    if (isGrowing()) {
        // Simple grow: interpolate from 1.0 to 1.275 over GrowAnimDuration
        float t  = std::min(growAnimTime() / GrowAnimDuration, 1.f);
        scaleAbs = baseScale * (1.0f + t * 0.275f);
    } else if (isShrinking()) {
        float t  = std::min(shrinkAnimTime() / ShrinkAnimDuration, 1.f);
        scaleAbs = baseScale * (1.275f - t * 0.275f);
    } else if (isBig()) {
        scaleAbs = baseScale * 1.275f;
    }

    float scaleX = scaleAbs * 0.80f; // ốm đi 20%
    float scaleY = scaleAbs * 1.10f; // cao hơn 10%

    float oldWidth  = width_;
    float oldHeight = height_;
    width_  = 35.0f * scaleX * 0.90f;
    float topPadding = 2.0f;
    height_ = (47.0f - topPadding) * scaleY;
    position_.x += (oldWidth - width_) / 2.0f;
    position_.y += oldHeight - height_;

    if (facing() < 0)
        sprite_.setScale(-scaleX, scaleY);
    else
        sprite_.setScale( scaleX, scaleY);

    sprite_.setPosition(position_.x + width() / 2.0f, position_.y + height());
}

void Luigi::Render(sf::RenderWindow& window) const {
    if (invincible_) {
        // Rainbow color cycling effect - 720 degrees per second (2 full cycles/sec)
        float hue = std::fmod(animationTime_ * 720.f, 360.f);
        sf::Color rainbow = hsvToRgb(hue, 0.85f, 1.0f);
        const_cast<sf::Sprite&>(sprite_).setColor(rainbow);
        window.draw(sprite_);
        const_cast<sf::Sprite&>(sprite_).setColor(sf::Color::White);
    } else {
        window.draw(sprite_);
    }
}
