#pragma once

#include "levels/LevelData.hpp"

#include <SFML/Graphics.hpp>

#include "levels/renderer/TileRenderer.hpp"
#include <string>
#include <memory>

class Character;

class TileMap {
public:
    void load(const std::string& path);
    void render(sf::RenderWindow& window) const;
    void resolveCollision(Character& character, sf::Time timePerFrame) const;

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
    std::unique_ptr<TileRenderer> m_renderer;
};
