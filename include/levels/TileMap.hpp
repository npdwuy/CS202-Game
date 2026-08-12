#pragma once

#include "levels/LevelData.hpp"

#include <SFML/Graphics.hpp>

#include "levels/renderer/TileRenderer.hpp"
#include <string>
#include <memory>

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

class TileMap {
public:
    void load(const std::string& path);
    void render(sf::RenderWindow& window) const;
    void update(sf::Time dt);
    void resolveCollision(Character& character, sf::Time timePerFrame, std::function<void(int row, int col)> onHitRoof = nullptr) const;
    void breakBlock(int row, int col);

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
};
