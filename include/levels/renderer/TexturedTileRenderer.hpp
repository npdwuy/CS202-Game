#pragma once
#include "levels/renderer/TileRenderer.hpp"

struct TilesetLayout {
    // Surface (Grass Top)
    sf::Vector2i surfaceLeft{8, 11};
    sf::Vector2i surfaceCenter{2, 12};
    sf::Vector2i surfaceRight{9, 11};
    sf::Vector2i surfaceIsolated{5, 0};

    // Dirt Fill
    sf::Vector2i dirtLeft{10, 11};
    sf::Vector2i dirtCenter{5, 12};
    sf::Vector2i dirtRight{11, 11};

    // High/Floating Block
    sf::Vector2i highBlock{1, 0};

    // High Ground Surface (T)
    sf::Vector2i highSurfaceLeft{11, 7};
    sf::Vector2i highSurfaceCenter{9, 7};
    sf::Vector2i highSurfaceRight{10, 7};
    sf::Vector2i highSurfaceIsolated{4, 13};

    // High Ground Dirt (D)
    sf::Vector2i highDirtLeft{11, 8};
    sf::Vector2i highDirtCenter{9, 8};
    sf::Vector2i highDirtRight{10, 8};
    sf::Vector2i highDirtIsolated{5, 13};
};

class TexturedTileRenderer : public TileRenderer {
public:
    explicit TexturedTileRenderer(sf::Texture texture, TilesetLayout layout = TilesetLayout{});

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

    bool getTileSprite(sf::Sprite& outSprite, int quadrant) const override;

private:
    sf::Vector2i getTileCoordFor(const LevelData& data, int row, int col) const;

    void appendTexturedQuad(
        sf::VertexArray& vertices,
        const sf::FloatRect& bounds,
        const sf::FloatRect& texCoords,
        sf::Color color = sf::Color::White
    ) const;

    sf::Texture m_texture;
    sf::Texture m_pipeTexture;
    sf::Image m_image;
    TilesetLayout m_layout;
};
