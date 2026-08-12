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
    sf::RectangleShape shape;
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
    static constexpr float BounceDuration = 0.3f;
    static constexpr float BounceHeight = 6.f;
};

class TileMap {
public:
    void load(const std::string& path);
    void render(sf::RenderWindow& window) const;
    void update(sf::Time dt);
    void resolveCollision(Character& character, sf::Time timePerFrame, std::function<void(int row, int col)> onHitRoof = nullptr) const;
    void breakBlock(int row, int col);
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

    LevelData m_data;
    sf::VertexArray m_backgroundVertices{sf::Quads};
    sf::VertexArray m_sceneryVertices{sf::Triangles};
    sf::VertexArray m_tileVertices{sf::Quads};
    sf::RectangleShape m_exitPole;
    sf::RectangleShape m_exitFlag;
    float m_flagY = 0.f;
    std::vector<BlockDebris> m_debris;
    std::unique_ptr<TileRenderer> m_renderer;

    // Cracked block tracking (hit once by mushroom Mario)
    std::vector<CrackedBlock> m_crackedBlocks;
    std::set<std::pair<int,int>> m_crackedSet;  // (row, col) for O(1) lookup
};
