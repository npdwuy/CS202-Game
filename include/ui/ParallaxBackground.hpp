#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

class ParallaxBackground {
public:
    ParallaxBackground();
    ~ParallaxBackground() = default;

    // Load texture from file and prepare sprites
    bool load(const std::string& texturePath);

    // Update the position of the background based on the camera view
    // parallaxFactor determines how fast it scrolls (0 = static, 1 = moves with camera)
    void update(const sf::View& camera, float parallaxFactor = 0.5f);

    // Render the seamless background
    void render(sf::RenderTarget& target) const;

private:
    std::shared_ptr<sf::Texture> m_texture;
    sf::Sprite m_spriteNormal;
    sf::Sprite m_spriteFlipped;

    float m_scaledWidth = 0.f;
};
