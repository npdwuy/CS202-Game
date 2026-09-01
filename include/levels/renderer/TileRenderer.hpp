#pragma once
#include <SFML/Graphics.hpp>
#include "levels/LevelData.hpp"
#include <set>
#include <utility>

class TileRenderer {
public:
    virtual ~TileRenderer() = default;

    virtual void buildGeometry(
        sf::VertexArray& tileVertices,
        sf::VertexArray& sceneryVertices,
        sf::VertexArray& backgroundVertices,
        const LevelData& data,
        const std::set<std::pair<int, int>>& hiddenTiles
    ) = 0;

    virtual void buildSingleTile(
        sf::VertexArray& vertices,
        const LevelData& data,
        int row,
        int col,
        sf::Vector2f offset = {0.f, 0.f}
    ) const = 0;

    virtual bool isTransparent(const LevelData& data, int row, int col, int localX, int localY) const = 0;

    virtual void setDrawSky(bool draw) {}
    virtual void setDrawHills(bool draw) {}

    virtual void render(
        sf::RenderTarget& target,
        const sf::VertexArray& tileVertices,
        const sf::VertexArray& sceneryVertices,
        const sf::VertexArray& backgroundVertices
    ) const = 0;

    virtual sf::Color getTileColor(const LevelData& data) const {
        return sf::Color(180, 100, 40);
    }

    virtual bool getTileSprite(sf::Sprite& outSprite, int quadrant) const {
        return false;
    }
};
