#pragma once
#include "levels/renderer/TileRenderer.hpp"
#include <string>

class ProceduralTileRenderer : public TileRenderer {
public:
    void buildGeometry(
        sf::VertexArray& tileVertices,
        sf::VertexArray& sceneryVertices,
        sf::VertexArray& backgroundVertices,
        const LevelData& data
    ) override;

    void render(
        sf::RenderTarget& target,
        const sf::VertexArray& tileVertices,
        const sf::VertexArray& sceneryVertices,
        const sf::VertexArray& backgroundVertices
    ) const override;

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
