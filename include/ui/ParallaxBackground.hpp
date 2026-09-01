#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

class ParallaxBackground {
public:
    ParallaxBackground();
    ~ParallaxBackground() = default;

    bool load(const std::string& texturePath);

    void update(const sf::View& camera, float parallaxFactor = 0.5f);

    void render(sf::RenderTarget& target) const;

private:
    std::shared_ptr<sf::Texture> m_texture;
    sf::Sprite m_spriteNormal;
    sf::Sprite m_spriteFlipped;

    float m_scaledWidth = 0.f;
};
