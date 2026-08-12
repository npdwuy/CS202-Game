#include "entities/blocks/QuestionBlock.hpp"
#include "resources/ResourceManager.hpp"

#include <cmath>

QuestionBlock::QuestionBlock(int row, int col, sf::Vector2f position, float tileSize)
    : m_row(row),
      m_col(col),
      m_isEmpty(false),
      m_basePosition(position),
      m_isBouncing(false),
      m_bounceTimer(0.f),
      m_bounceOffset(0.f)
{
    m_sprite.setTexture(ResourceManager::getInstance().getTexture(
        "assets/sprites/tilesets/WU_Field_plain_D.png"
    ));
    m_sprite.setTextureRect(sf::IntRect(2 * 64, 0, 64, 64)); // row 0, col 2
    
    // Scale sprite to match tileSize (source tile size is 64x64)
    float scale = tileSize / 64.f;
    m_sprite.setScale(scale, scale);
    
    m_sprite.setPosition(position);
}

void QuestionBlock::Update(sf::Time timePerFrame) {
    if (m_isBouncing) {
        m_bounceTimer += timePerFrame.asSeconds();
        const float bounceDuration = 0.2f;
        const float bounceHeight = 16.f;
        
        if (m_bounceTimer >= bounceDuration) {
            m_isBouncing = false;
            m_bounceTimer = 0.f;
            m_bounceOffset = 0.f;
            m_isEmpty = true;
            m_sprite.setTextureRect(sf::IntRect(3 * 64, 0, 64, 64)); // row 0, col 3
        } else {
            // Parabolic bounce
            float t = m_bounceTimer / bounceDuration;
            m_bounceOffset = -bounceHeight * std::sin(t * 3.14159f);
        }
        
        m_sprite.setPosition(m_basePosition.x, m_basePosition.y + m_bounceOffset);
    }
}

void QuestionBlock::Render(sf::RenderWindow& window) const {
    window.draw(m_sprite);
}

void QuestionBlock::Hit() {
    if (m_isEmpty || m_isBouncing) {
        return;
    }
    m_isBouncing = true;
    m_bounceTimer = 0.f;
}

bool QuestionBlock::IsEmpty() const {
    return m_isEmpty;
}
