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
};

class TexturedTileRenderer : public TileRenderer {
public:
    explicit TexturedTileRenderer(sf::Texture texture, TilesetLayout layout = TilesetLayout{});

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
    void appendTexturedQuad(
        sf::VertexArray& vertices,
        const sf::FloatRect& bounds,
        const sf::FloatRect& texCoords
    ) const;

    sf::Texture m_texture;
    TilesetLayout m_layout;
};
