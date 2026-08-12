#pragma once
#include "levels/renderer/TileRenderer.hpp"
#include <string>

class ProceduralTileRenderer : public TileRenderer {
public:
    void buildGeometry(
        sf::VertexArray& tileVertices,
        sf::VertexArray& sceneryVertices,
        sf::VertexArray& backgroundVertices,
        const LevelData& data,
        const std::set<std::pair<int, int>>& hiddenTiles
    ) override;

    void buildSingleTile(
        sf::VertexArray& vertices,
        const LevelData& data,
        int row,
        int col,
        sf::Vector2f offset
    ) const override;

    bool isTransparent(const LevelData& data, int row, int col, int localX, int localY) const override;

    void render(
        sf::RenderTarget& target,
        const sf::VertexArray& tileVertices,
        const sf::VertexArray& sceneryVertices,
        const sf::VertexArray& backgroundVertices
    ) const override;

    sf::Color getTileColor(const LevelData& data) const override;

private:
    struct TilePalette {
        sf::Color outline;
        sf::Color lightFill;
        sf::Color darkFill;
        sf::Color surface;
        sf::Color surfaceHighlight;
        sf::Color skyTop;
        sf::Color skyBottom;
        sf::Color farHill;
        sf::Color nearHill;
    };

    TilePalette paletteFor(const std::string& difficulty) const;
    void appendQuad(
        sf::VertexArray& vertices,
        const sf::FloatRect& bounds,
        sf::Color color
    ) const;
    void appendGradientQuad(
        sf::VertexArray& vertices,
        const sf::FloatRect& bounds,
        sf::Color topColor,
        sf::Color bottomColor
    ) const;
    void appendHill(
        sf::VertexArray& vertices,
        float centerX,
        float baseY,
        float width,
        float height,
        sf::Color color
    ) const;
    sf::Color brighten(sf::Color color, unsigned int amount) const;
};
