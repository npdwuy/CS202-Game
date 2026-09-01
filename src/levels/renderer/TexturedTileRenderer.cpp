#include "levels/renderer/TexturedTileRenderer.hpp"
#include <algorithm>
#include <cmath>
#include <utility>

namespace {

struct Palette {
    sf::Color skyTop;
    sf::Color skyBottom;
    sf::Color farHill;
    sf::Color nearHill;
};

Palette paletteFor(const std::string& difficulty) {
    if (difficulty == "Medium") {
        return {
            sf::Color(113, 148, 216),
            sf::Color(255, 210, 135),
            sf::Color(102, 108, 151, 125),
            sf::Color(116, 92, 112, 165)
        };
    }
    if (difficulty == "Hard") {
        return {
            sf::Color(35, 26, 49),
            sf::Color(112, 49, 56),
            sf::Color(56, 41, 66, 160),
            sf::Color(66, 43, 55, 205)
        };
    }
    return {
        sf::Color(95, 190, 255),
        sf::Color(205, 236, 255),
        sf::Color(101, 181, 163, 120),
        sf::Color(76, 151, 117, 170)
    };
}

sf::Color brighten(sf::Color color, unsigned int amount) {
    return {
        static_cast<sf::Uint8>(std::min(255U, color.r + amount)),
        static_cast<sf::Uint8>(std::min(255U, color.g + amount)),
        static_cast<sf::Uint8>(std::min(255U, color.b + amount)),
        color.a
    };
}

void appendGradientQuad(
    sf::VertexArray& vertices,
    const sf::FloatRect& bounds,
    sf::Color topColor,
    sf::Color bottomColor
) {
    vertices.append({{bounds.left, bounds.top}, topColor});
    vertices.append({{bounds.left + bounds.width, bounds.top}, topColor});
    vertices.append({
        {bounds.left + bounds.width, bounds.top + bounds.height},
        bottomColor
    });
    vertices.append({{bounds.left, bounds.top + bounds.height}, bottomColor});
}

void appendHill(
    sf::VertexArray& vertices,
    float centerX,
    float baseY,
    float width,
    float height,
    sf::Color color
) {
    sf::Color peakColor = brighten(color, 18U);
    vertices.append({{centerX - width * 0.5f, baseY}, color});
    vertices.append({{centerX, baseY - height}, peakColor});
    vertices.append({{centerX + width * 0.5f, baseY}, color});
}

} // namespace

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
    auto isGround = [](char ch) { return ch == '#' || ch == '=' || ch == 'T' || ch == 'D' || ch == 'W' || ch == '|'; };
    const bool left = (col == 0) || isGround(data.rows[row][col - 1]);
    const bool right = (col + 1 >= static_cast<int>(data.rows[row].size())) || isGround(data.rows[row][col + 1]);

    sf::Vector2i tileCoord = m_layout.dirtCenter;

    if (c == 'B' || c == '?' || c == '!') {
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
    } else if (c == '=' || c == 'W' || c == '|') {
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

    // 1. Build Background
    const Palette palette = paletteFor(data.difficulty);
    const sf::Vector2f worldSize = data.worldSize();
    appendGradientQuad(
        backgroundVertices,
        {
            -540.f,
            0.f,
            worldSize.x + 1080.f,
            std::max(1080.f, worldSize.y)
        },
        palette.skyTop,
        palette.skyBottom
    );

    // 2. Build Scenery Hills
    for (int index = 0; index < 5; ++index) {
        appendHill(
            sceneryVertices,
            120.f + static_cast<float>(index) * 470.f,
            890.f,
            520.f,
            175.f + static_cast<float>(index % 2) * 35.f,
            palette.farHill
        );
    }
    for (int index = 0; index < 4; ++index) {
        appendHill(
            sceneryVertices,
            300.f + static_cast<float>(index) * 610.f,
            930.f,
            690.f,
            225.f + static_cast<float>((index + 1) % 2) * 45.f,
            palette.nearHill
        );
    }

    // 3. Build Textured Tiles with Autotiling
    const float tileSize = static_cast<float>(data.tileSize);
    const float sourceTileSize = 64.f;

    for (std::size_t row = 0; row < data.rows.size(); ++row) {
        for (std::size_t column = 0; column < data.rows[row].size(); ++column) {
            char c = data.rows[row][column];
            if (c != '#' && c != '=' && c != 'T' && c != 'D' && c != 'B' && c != '?' && c != '!' && c != 'W' && c != '|') {
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

            const int tileCol = tileCoord.x;
            const int tileRow = tileCoord.y;

            sf::FloatRect texCoords(
                static_cast<float>(tileCol) * sourceTileSize,
                static_cast<float>(tileRow) * sourceTileSize,
                sourceTileSize,
                sourceTileSize
            );

            sf::Color vertexColor = sf::Color::White;
            if (c == '?') {
                vertexColor = sf::Color(255, 230, 100);
            } else if (c == '!') {
                vertexColor = sf::Color(120, 100, 80);
            } else if (c == 'W' || c == '|') {
                vertexColor = sf::Color(0, 200, 0); // Green pipe color over dirt texture
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

    appendTexturedQuad(
        vertices,
        {position.x, position.y, tileSize, tileSize},
        texCoords
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