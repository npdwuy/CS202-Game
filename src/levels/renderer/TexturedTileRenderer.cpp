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
    : m_texture(std::move(texture)), m_layout(layout) {}

void TexturedTileRenderer::appendTexturedQuad(
    sf::VertexArray& vertices,
    const sf::FloatRect& bounds,
    const sf::FloatRect& texCoords
) const {
    vertices.append({{bounds.left, bounds.top}, sf::Color::White, {texCoords.left, texCoords.top}});
    vertices.append({{bounds.left + bounds.width, bounds.top}, sf::Color::White, {texCoords.left + texCoords.width, texCoords.top}});
    vertices.append({
        {bounds.left + bounds.width, bounds.top + bounds.height},
        sf::Color::White,
        {texCoords.left + texCoords.width, texCoords.top + texCoords.height}
    });
    vertices.append({{bounds.left, bounds.top + bounds.height}, sf::Color::White, {texCoords.left, texCoords.top + texCoords.height}});
}

void TexturedTileRenderer::buildGeometry(
    sf::VertexArray& tileVertices,
    sf::VertexArray& sceneryVertices,
    sf::VertexArray& backgroundVertices,
    const LevelData& data
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
            if (data.rows[row][column] != '#') {
                continue;
            }

            const sf::Vector2f position{
                static_cast<float>(column) * tileSize,
                static_cast<float>(row) * tileSize
            };

            const bool above = (row > 0 && data.rows[row - 1][column] == '#');
            const bool below = (row + 1 < data.rows.size() && data.rows[row + 1][column] == '#');
            const bool left = (column == 0) || (data.rows[row][column - 1] == '#');
            const bool right = (column + 1 >= data.rows[row].size()) || (data.rows[row][column + 1] == '#');

            sf::Vector2i tileCoord = m_layout.dirtCenter;

            if (row < data.rows.size() - 2) {
                // High/Floating block
                tileCoord = m_layout.highBlock;
            } else if (!above) {
                // Grass Surface
                if (!left && right) {
                    tileCoord = m_layout.surfaceLeft;
                } else if (left && !right) {
                    tileCoord = m_layout.surfaceRight;
                } else if (!left && !right) {
                    tileCoord = m_layout.surfaceIsolated;
                } else {
                    tileCoord = m_layout.surfaceCenter;
                }
            } else {
                // Dirt Fill Below Surface
                if (!left && right) {
                    tileCoord = m_layout.dirtLeft;
                } else if (left && !right) {
                    tileCoord = m_layout.dirtRight;
                } else {
                    tileCoord = m_layout.dirtCenter;
                }
            }

            const int tileCol = tileCoord.x;
            const int tileRow = tileCoord.y;

            sf::FloatRect texCoords(
                static_cast<float>(tileCol) * sourceTileSize,
                static_cast<float>(tileRow) * sourceTileSize,
                sourceTileSize,
                sourceTileSize
            );

            appendTexturedQuad(
                tileVertices,
                {position.x, position.y, tileSize, tileSize},
                texCoords
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