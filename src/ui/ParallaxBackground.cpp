#include "ui/ParallaxBackground.hpp"
#include <cmath>

ParallaxBackground::ParallaxBackground() {
}

bool ParallaxBackground::load(const std::string& texturePath) {
    m_texture = std::make_shared<sf::Texture>();
    if (!m_texture->loadFromFile(texturePath)) {
        return false;
    }

    // Prepare normal sprite
    m_spriteNormal.setTexture(*m_texture);
    
    // Scale to fit screen height (1080p)
    float scaleY = 1080.f / static_cast<float>(m_texture->getSize().y);
    m_spriteNormal.setScale(scaleY, scaleY);
    m_scaledWidth = static_cast<float>(m_texture->getSize().x) * scaleY;

    // Prepare flipped sprite for seamless mirroring
    m_spriteFlipped.setTexture(*m_texture);
    m_spriteFlipped.setScale(-scaleY, scaleY); // Flip horizontally
    m_spriteFlipped.setOrigin(static_cast<float>(m_texture->getSize().x), 0.f);

    return true;
}

void ParallaxBackground::update(const sf::View& camera, float parallaxFactor) {
    if (!m_texture) return;

    sf::Vector2f camCenter = camera.getCenter();
    sf::Vector2f camSize = camera.getSize();
    
    // Calculate the top-left corner of the camera view
    float camLeft = camCenter.x - camSize.x / 2.f;
    float camTop = camCenter.y - camSize.y / 2.f;

    // The base position if parallax didn't exist
    float parallaxBaseX = camLeft * parallaxFactor;
    
    // Find how many full patterns (2 sprites = normal + flipped = 2 * m_scaledWidth) have passed
    float patternWidth = m_scaledWidth * 2.f;
    float offset = std::fmod(parallaxBaseX, patternWidth);
    
    if (offset < 0) offset += patternWidth;

    // We anchor the drawing so it perfectly covers the camera view.
    // The first sprite starts at (camLeft - offset)
    float startX = camLeft - offset;
    float startY = camTop; // Stick to the top of the camera! This removes the black border.

    m_spriteNormal.setPosition(startX, startY);
    m_spriteFlipped.setPosition(startX + m_scaledWidth, startY);
}

void ParallaxBackground::render(sf::RenderTarget& target) const {
    if (!m_texture) return;
    
    // We draw enough copies to cover a 1920x1080 screen (at least 2 patterns needed depending on scaled width)
    // A pattern is normal + flipped.
    
    // We already positioned the first pattern at (startX, startY) in update().
    // We just draw it, and then draw another pattern to the right to guarantee full coverage.
    float startX = m_spriteNormal.getPosition().x;
    float startY = m_spriteNormal.getPosition().y;
    float patternWidth = m_scaledWidth * 2.f;

    // We can draw 3 patterns to be completely safe (left edge could be slightly off-screen)
    for (int i = 0; i < 3; ++i) {
        sf::Sprite normal = m_spriteNormal;
        sf::Sprite flipped = m_spriteFlipped;
        
        normal.setPosition(startX + i * patternWidth, startY);
        flipped.setPosition(startX + m_scaledWidth + i * patternWidth, startY);
        
        target.draw(normal);
        target.draw(flipped);
    }
}
