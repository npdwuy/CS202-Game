#include "levels/renderer/TexturedTileRenderer.hpp"
#include <algorithm>
#include <cmath>
#include <utility>

TexturedTileRenderer::TexturedTileRenderer(sf::Texture texture, TilesetLayout layout)
    : m_texture(std::move(texture)), m_image(m_texture.copyToImage()), m_layout(layout) {}

void TexturedTileRenderer::appendTexturedQuad(
    sf::VertexArray& vertices,
    const sf::FloatRect& bounds,
    const sf::FloatRect& texCoords,
    sf::Color color
) const {
    vertices.append({{bounds.left, bounds.top}, color, {texCoords.left, texCoords.top}});
    vertices.append({{bounds.left + bounds.width, bounds.top}, color, {texCoords.left + texCoords.width, texCoords.top}});
    vertices.append({
        {bounds.left + bounds.width, bounds.top + bounds.height},
        color,
        {texCoords.left + texCoords.width, texCoords.top + texCoords.height}
    });
    vertices.append({{bounds.left, bounds.top + bounds.height}, color, {texCoords.left, texCoords.top + texCoords.height}});
}

sf::Vector2i TexturedTileRenderer::getTileCoordFor(const LevelData& data, int row, int col) const {
    char c = data.rows[row][col];
    auto isGround = [](char ch) { return ch == '#' || ch == '=' || ch == 'T' || ch == 'D' || ch == 'W' || ch == '|' || ch == '[' || ch == ']'; };
    const bool left = (col == 0) || isGround(data.rows[row][col - 1]);
    const bool right = (col + 1 >= static_cast<int>(data.rows[row].size())) || isGround(data.rows[row][col + 1]);

    sf::Vector2i tileCoord = m_layout.dirtCenter;

    if (c == 'W' || c == '|' || c == '[' || c == ']') {
        // Vertical / Horizontal pipe: column 14 (left half) & 15 (right half), row 0 (top mouth), row 1 (body extending into ground)
        bool isLeft = (col == 0) || (data.rows[row][col - 1] != 'W' && data.rows[row][col - 1] != '|' && data.rows[row][col - 1] != '[' && data.rows[row][col - 1] != ']');
        if (c == 'W' || c == '[') {
            tileCoord = isLeft ? m_layout.pipeVerticalTopLeft : m_layout.pipeVerticalTopRight;
        } else {
            // Phần thân kéo dài vô đất: luôn dùng tile thân (row 1)
            tileCoord = isLeft ? m_layout.pipeVerticalBodyLeft : m_layout.pipeVerticalBodyRight;
        }
    } else if (c == 'B' || c == '?' || c == '!') {
        tileCoord = m_layout.highBlock;
    } else if (c == 'T') {
        if (!left && right) tileCoord = m_layout.highSurfaceLeft;
        else if (left && !right) tileCoord = m_layout.highSurfaceRight;
        else if (!left && !right) tileCoord = m_layout.highSurfaceIsolated;
        else tileCoord = m_layout.highSurfaceCenter;
    } else if (c == 'D') {
        if (!left && right) tileCoord = m_layout.highDirtLeft;
        else if (left && !right) tileCoord = m_layout.highDirtRight;
        else if (!left && !right) tileCoord = m_layout.highDirtIsolated;
        else tileCoord = m_layout.highDirtCenter;
    } else if (c == '#') {
        if (!left && right) tileCoord = m_layout.surfaceLeft;
        else if (left && !right) tileCoord = m_layout.surfaceRight;
        else if (!left && !right) tileCoord = m_layout.surfaceIsolated;
        else tileCoord = m_layout.surfaceCenter;
    } else if (c == '=') {
        if (!left && right) tileCoord = m_layout.dirtLeft;
        else if (left && !right) tileCoord = m_layout.dirtRight;
        else tileCoord = m_layout.dirtCenter;
    }
    return tileCoord;
}

void TexturedTileRenderer::buildGeometry(
    sf::VertexArray& tileVertices,
    sf::VertexArray& sceneryVertices,
    sf::VertexArray& backgroundVertices,
    const LevelData& data,
    const std::set<std::pair<int, int>>& hiddenTiles
) {
    tileVertices.clear();
    sceneryVertices.clear();
    backgroundVertices.clear();

    const float tileSize = static_cast<float>(data.tileSize);
    const float sourceTileSize = 64.f;

    for (std::size_t row = 0; row < data.rows.size(); ++row) {
        for (std::size_t column = 0; column < data.rows[row].size(); ++column) {
            char c = data.rows[row][column];
            if (c != '#' && c != '=' && c != 'T' && c != 'D' && c != 'B' && c != '?' && c != '!' && c != 'W' && c != '|' && c != '[' && c != ']') {
                continue;
            }

            if (hiddenTiles.count({static_cast<int>(row), static_cast<int>(column)})) {
                continue;
            }

            const sf::Vector2f position{
                static_cast<float>(column) * tileSize,
                static_cast<float>(row) * tileSize
            };

            sf::Vector2i tileCoord = getTileCoordFor(data, row, column);

            sf::FloatRect texCoords(
                static_cast<float>(tileCoord.x) * sourceTileSize,
                static_cast<float>(tileCoord.y) * sourceTileSize,
                sourceTileSize,
                sourceTileSize
            );

            sf::Color vertexColor = sf::Color::White;
            if (c == '?') {
                vertexColor = sf::Color(255, 230, 100);
            } else if (c == '!') {
                vertexColor = sf::Color(120, 100, 80);
            }

            appendTexturedQuad(
                tileVertices,
                {position.x, position.y, tileSize, tileSize},
                texCoords,
                vertexColor
            );
        }
    }
}

void TexturedTileRenderer::render(
    sf::RenderTarget& target,
    const sf::VertexArray& tileVertices,
    const sf::VertexArray& sceneryVertices,
    const sf::VertexArray& backgroundVertices
) const
{
    sf::RenderStates states;
    states.texture = &m_texture;
    target.draw(tileVertices, states);
}

bool TexturedTileRenderer::getTileSprite(sf::Sprite& outSprite, int quadrant) const {
    const float sourceTileSize = 64.f;
    sf::Vector2i tileCoord = m_layout.highBlock;
    
    float halfTs = sourceTileSize * 0.5f;
    float texX = tileCoord.x * sourceTileSize;
    float texY = tileCoord.y * sourceTileSize;
    
    if (quadrant == 1 || quadrant == 3) texX += halfTs;
    if (quadrant == 2 || quadrant == 3) texY += halfTs;
    
    outSprite.setTexture(m_texture);
    outSprite.setTextureRect(sf::IntRect(static_cast<int>(texX), static_cast<int>(texY), static_cast<int>(halfTs), static_cast<int>(halfTs)));
    return true;
}

void TexturedTileRenderer::buildSingleTile(
    sf::VertexArray& vertices,
    const LevelData& data,
    int row,
    int col,
    sf::Vector2f offset
) const {
    const float tileSize = static_cast<float>(data.tileSize);
    const float sourceTileSize = 64.f;

    const sf::Vector2f position{
        static_cast<float>(col) * tileSize + offset.x,
        static_cast<float>(row) * tileSize + offset.y
    };

    sf::Vector2i tileCoord = getTileCoordFor(data, row, col);

    sf::FloatRect texCoords(
        static_cast<float>(tileCoord.x) * sourceTileSize,
        static_cast<float>(tileCoord.y) * sourceTileSize,
        sourceTileSize,
        sourceTileSize
    );

    sf::Color vertexColor = sf::Color::White;
    char c = data.rows[row][col];
    if (c == '?') {
        vertexColor = sf::Color(255, 230, 100);
    } else if (c == '!') {
        vertexColor = sf::Color(120, 100, 80);
    }

    appendTexturedQuad(
        vertices,
        {position.x, position.y, tileSize, tileSize},
        texCoords,
        vertexColor
    );
}

bool TexturedTileRenderer::isTransparent(const LevelData& data, int row, int col, int localX, int localY) const {
    const float sourceTileSize = 64.f;
    const float destTileSize = static_cast<float>(data.tileSize);

    sf::Vector2i tileCoord = getTileCoordFor(data, row, col);

    float ratio = sourceTileSize / destTileSize;
    int srcX = static_cast<int>(localX * ratio);
    int srcY = static_cast<int>(localY * ratio);
    
    if (srcX < 0 || srcX >= static_cast<int>(sourceTileSize) || 
        srcY < 0 || srcY >= static_cast<int>(sourceTileSize)) {
        return true;
    }

    int pixelX = static_cast<int>(tileCoord.x * sourceTileSize) + srcX;
    int pixelY = static_cast<int>(tileCoord.y * sourceTileSize) + srcY;
    
    if (pixelX < 0 || pixelX >= static_cast<int>(m_image.getSize().x) ||
        pixelY < 0 || pixelY >= static_cast<int>(m_image.getSize().y)) {
        return true;
    }

    return m_image.getPixel(pixelX, pixelY).a == 0;
}