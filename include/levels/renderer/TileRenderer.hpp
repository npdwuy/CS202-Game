#pragma once
#include <SFML/Graphics.hpp>
#include "levels/LevelData.hpp"

class TileRenderer {
public:
    virtual ~TileRenderer() = default;

    virtual void buildGeometry(
        sf::VertexArray& tileVertices,
        sf::VertexArray& sceneryVertices,
        sf::VertexArray& backgroundVertices,
        const LevelData& data
    ) = 0;

    virtual void render(
        sf::RenderTarget& target,
        const sf::VertexArray& tileVertices,
        const sf::VertexArray& sceneryVertices,
        const sf::VertexArray& backgroundVertices
    ) const = 0;
};
