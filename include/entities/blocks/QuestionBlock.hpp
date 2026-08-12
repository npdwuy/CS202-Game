#pragma once

#include <SFML/Graphics.hpp>

class QuestionBlock {
public:
    QuestionBlock(int row, int col, sf::Vector2f position, float tileSize);

    void Update(sf::Time timePerFrame);
    void Render(sf::RenderWindow& window) const;

    void Hit();
    bool IsEmpty() const;

    int GetRow() const { return m_row; }
    int GetCol() const { return m_col; }

private:
    int m_row;
    int m_col;
    sf::Sprite m_sprite;
    bool m_isEmpty;
    
    // Bouncing animation state
    sf::Vector2f m_basePosition;
    bool m_isBouncing;
    float m_bounceTimer;
    float m_bounceOffset;
};
