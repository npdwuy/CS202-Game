#include "ui/ParallaxBackground.hpp"
#include <cmath>

ParallaxBackground::ParallaxBackground() {}

bool ParallaxBackground::load(const std::string& texturePath) {
    m_texture = std::make_shared<sf::Texture>();
    if (!m_texture->loadFromFile(texturePath)) {
        return false;
    }
    m_texture->setSmooth(true);

    m_spriteNormal.setTexture(*m_texture);
    
    constexpr float TargetScreenHeight = 1080.f;
    const float scaleY = TargetScreenHeight / static_cast<float>(m_texture->getSize().y);
    m_spriteNormal.setScale(scaleY, scaleY);
    m_scaledWidth = static_cast<float>(m_texture->getSize().x) * scaleY;

    m_spriteFlipped.setTexture(*m_texture);
    m_spriteFlipped.setScale(-scaleY, scaleY);
    m_spriteFlipped.setOrigin(static_cast<float>(m_texture->getSize().x), 0.f);

    return true;
}

void ParallaxBackground::update(const sf::View& camera, float parallaxFactor) {
    if (!m_texture) return;

    const sf::Vector2f camCenter = camera.getCenter();
    const sf::Vector2f camSize = camera.getSize();
    
    const float camLeft = camCenter.x - camSize.x / 2.f;
    const float camTop = camCenter.y - camSize.y / 2.f;

    const float parallaxBaseX = camLeft * parallaxFactor;
    
    const float patternWidth = m_scaledWidth * 2.f;
    float cameraScrollOffset = std::fmod(parallaxBaseX, patternWidth);
    if (cameraScrollOffset < 0) cameraScrollOffset += patternWidth;

    const float startX = camLeft - cameraScrollOffset;
    const float startY = camTop;

    m_spriteNormal.setPosition(startX, startY);
    m_spriteFlipped.setPosition(startX + m_scaledWidth, startY);
}

void ParallaxBackground::render(sf::RenderTarget& target) const {
    if (!m_texture) return;
    
    const float startX = m_spriteNormal.getPosition().x;
    const float startY = m_spriteNormal.getPosition().y;
    const float patternWidth = m_scaledWidth * 2.f;

    constexpr int RequiredDrawPatterns = 3;
    for (int i = 0; i < RequiredDrawPatterns; ++i) {
        sf::Sprite normal = m_spriteNormal;
        sf::Sprite flipped = m_spriteFlipped;
        
        normal.setPosition(startX + i * patternWidth, startY);
        flipped.setPosition(startX + m_scaledWidth + i * patternWidth, startY);
        
        target.draw(normal);
        target.draw(flipped);
    }
}
