#include "entities/player/Luigi.hpp"
#include <cmath>

// Luigi stats:
//   Mario:  speed=292, jumpPower=830
//   Luigi:  speed=200 (slower -20%), jumpPower=1040.75 (jumps +15% higher)
Luigi::Luigi(sf::Vector2f position)
    : Player(position, "Luigi", 200.0f, 1040.75f)
{
    sf::Image image;

    // Prefer a dedicated Luigi sheet; fall back to Mario sheet tinted green.
    bool loaded = image.loadFromFile("assets/sprites/player/luigi.png");
    bool dedicatedLoaded = loaded;
    if (!loaded) {
        loaded = image.loadFromFile("assets/sprites/player/mario.png");
    }
    if (!loaded) {
        throw std::runtime_error("Failed to load Luigi (or fallback Mario) sprite sheet.");
    }

    image.createMaskFromColor(sf::Color::White);

    if (!texture_.loadFromImage(image)) {
        throw std::runtime_error("Failed to create Luigi texture from image.");
    }

    sprite_.setTexture(texture_);
    sprite_.setTextureRect(sf::IntRect(160, 15, 35, 47));
    sprite_.setOrigin(17.5f, 47.f);
    sprite_.setScale(1.2f, 1.65f); // ốm đi 20% (1.5 * 0.8), cao hơn 10% (1.5 * 1.10)
    sprite_.setPosition(position.x + width() / 2.0f, position.y + height());

    // Tint green only if using Mario fallback sheet
    if (!dedicatedLoaded) {
        sprite_.setColor(sf::Color(140, 230, 140));
    }
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
    window.draw(sprite_);
}
