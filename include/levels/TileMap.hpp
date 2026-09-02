#pragma once

#include "levels/LevelData.hpp"

#include <SFML/Graphics.hpp>

#include "levels/renderer/TileRenderer.hpp"
#include <string>
#include <memory>
#include <set>

#include <functional>

class Character;

struct BlockDebris {
    sf::Sprite sprite;
    sf::RectangleShape shape;
    bool useSprite = false;
    sf::Vector2f position;
    sf::Vector2f velocity;
    float rotation = 0.f;
    float rotationSpeed = 0.f;
    float lifeTime = 0.f;
};

struct CrackedBlock {
    int row = 0;
    int col = 0;
    float bounceTimer = 0.f;        // Thời gian bounce animation còn lại
    float bounceOffset = 0.f;       // Offset Y hiện tại (âm = lên)
    bool isBouncing = true;         // Đang trong giai đoạn bounce?
    std::shared_ptr<sf::Texture> crackTexture;
    sf::Sprite crackSprite;
    static constexpr float BounceDuration = 0.3f;
    static constexpr float BounceHeight = 6.f;
};

#include "entities/blocks/QuestionBlock.hpp"

// ... existing forward declarations

class TileMap {
public:
    void load(const std::string& path);
    void restoreTileRows(const std::vector<std::string>& tileRows);
    void render(sf::RenderWindow& window) const;
    void renderForegroundPipes(sf::RenderWindow& window) const;
    
    void setDrawSky(bool draw) {
        if (m_renderer) {
            m_renderer->setDrawSky(draw);
        }
    }
    void setDrawHills(bool draw) {
        if (m_renderer) {
            m_renderer->setDrawHills(draw);
        }
    }
    
    void update(sf::Time dt);
    void resolveCollision(Character& character, sf::Time timePerFrame, std::function<void(int row, int col)> onHitRoof = nullptr) const;
    void breakBlock(int row, int col);
    // Hit a question block. Returns true if it was a '?' block and has been changed to '!'.
    bool hitQuestionBlock(int row, int col);
    
    // Hit a block once. Returns true if block is destroyed (2nd hit), false if just cracked (1st hit).
    bool hitBlock(int row, int col);

    const LevelData& data() const;
    sf::FloatRect exitBounds() const;
    sf::FloatRect worldBounds() const;
    bool isSolidAt(sf::Vector2f worldPosition) const;
    bool intersectsSolid(const sf::FloatRect& bounds) const;

    bool updateFlagAnimation(sf::Time dt, float speed);
    float getPoleTopY() const;
    float getPoleBottomY() const;
    float getPoleX() const;


private:
    void rebuildGeometry();
    CrackedBlock makeCrackedBlock(int row, int col, float tileSize);

    LevelData m_data;
    sf::VertexArray m_backgroundVertices{sf::Quads};
    sf::VertexArray m_sceneryVertices{sf::Quads};
    sf::VertexArray m_tileVertices{sf::Quads};
    sf::RectangleShape m_exitPole;
    sf::RectangleShape m_exitFlag;
    float m_flagY = 0.f;
    std::vector<BlockDebris> m_debris;
    std::unique_ptr<TileRenderer> m_renderer;

    // Question Blocks
    std::vector<QuestionBlock> m_questionBlocks;

    // Cracked block tracking (hit once by mushroom Mario)
    std::vector<CrackedBlock> m_crackedBlocks;
    std::set<std::pair<int,int>> m_crackedSet;  // (row, col) for O(1) lookup
};
