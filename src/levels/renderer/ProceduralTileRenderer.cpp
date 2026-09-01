#include "levels/renderer/ProceduralTileRenderer.hpp"
#include <algorithm>
#include <cmath>

ProceduralTileRenderer::TilePalette ProceduralTileRenderer::paletteFor(const std::string& difficulty) const {
    if (difficulty == "Medium") {
        return {
            sf::Color(103, 66, 34),
            sf::Color(190, 129, 61),
            sf::Color(158, 99, 45),
            sf::Color(224, 178, 75),
            sf::Color(255, 218, 117),
            sf::Color(113, 148, 216),
            sf::Color(255, 210, 135),
            sf::Color(102, 108, 151, 125),
            sf::Color(116, 92, 112, 165)
        };
    }
    if (difficulty == "Hard") {
        return {
            sf::Color(52, 38, 45),
            sf::Color(112, 76, 72),
            sf::Color(82, 54, 59),
            sf::Color(148, 53, 43),
            sf::Color(235, 93, 50),
            sf::Color(35, 26, 49),
            sf::Color(112, 49, 56),
            sf::Color(56, 41, 66, 160),
            sf::Color(66, 43, 55, 205)
        };
    }
    return {
        sf::Color(91, 51, 31),
        sf::Color(158, 93, 52),
        sf::Color(139, 78, 43),
        sf::Color(74, 174, 70),
        sf::Color(122, 220, 90),
        sf::Color(95, 190, 255),
        sf::Color(205, 236, 255),
        sf::Color(101, 181, 163, 120),
        sf::Color(76, 151, 117, 170)
    };
}

void ProceduralTileRenderer::appendQuad(
    sf::VertexArray& vertices,
    const sf::FloatRect& bounds,
    sf::Color color
) const {
    vertices.append({{bounds.left, bounds.top}, color});
    vertices.append({{bounds.left + bounds.width, bounds.top}, color});
    vertices.append({
        {bounds.left + bounds.width, bounds.top + bounds.height},
        color
    });
    vertices.append({{bounds.left, bounds.top + bounds.height}, color});
}

void ProceduralTileRenderer::appendGradientQuad(
    sf::VertexArray& vertices,
    const sf::FloatRect& bounds,
    sf::Color topColor,
    sf::Color bottomColor
) const {
    vertices.append({{bounds.left, bounds.top}, topColor});
    vertices.append({{bounds.left + bounds.width, bounds.top}, topColor});
    vertices.append({
        {bounds.left + bounds.width, bounds.top + bounds.height},
        bottomColor
    });
    vertices.append({{bounds.left, bounds.top + bounds.height}, bottomColor});
}

void ProceduralTileRenderer::appendHill(
    sf::VertexArray& vertices,
    float centerX,
    float baseY,
    float width,
    float height,
    sf::Color color
) const {
    sf::Color peakColor = brighten(color, 18U);
    vertices.append({{centerX - width * 0.5f, baseY}, color});
    vertices.append({{centerX, baseY - height}, peakColor});
    vertices.append({{centerX + width * 0.5f, baseY}, color});
}

sf::Color ProceduralTileRenderer::brighten(sf::Color color, unsigned int amount) const {
    return {
        static_cast<sf::Uint8>(std::min(255U, color.r + amount)),
        static_cast<sf::Uint8>(std::min(255U, color.g + amount)),
        static_cast<sf::Uint8>(std::min(255U, color.b + amount)),
        color.a
    };
}

void ProceduralTileRenderer::buildGeometry(
    sf::VertexArray& tileVertices,
    sf::VertexArray& sceneryVertices,
    sf::VertexArray& backgroundVertices,
    const LevelData& data,
    const std::set<std::pair<int, int>>& hiddenTiles
) {
    tileVertices.clear();
    sceneryVertices.clear();
    backgroundVertices.clear();

    const TilePalette palette = paletteFor(data.difficulty);
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

    const float tileSize = static_cast<float>(data.tileSize);

    for (std::size_t row = 0; row < data.rows.size(); ++row) {
        for (std::size_t column = 0; column < data.rows[row].size(); ++column) {
            char c = data.rows[row][column];
            if (c != '#' && c != 'W' && c != '|') {
                continue;
            }

            if (hiddenTiles.count({static_cast<int>(row), static_cast<int>(column)})) {
                continue;
            }

            const sf::Vector2f position{
                static_cast<float>(column) * tileSize,
                static_cast<float>(row) * tileSize
            };

            sf::Color fillColor = row % 2U == 0U ? palette.lightFill : palette.darkFill;
            if (c == 'W' || c == '|') {
                fillColor = sf::Color(0, 160, 0); // Pipe Green
            }
            appendQuad(
                tileVertices,
                {position.x, position.y, tileSize, tileSize},
                palette.outline
            );
            appendQuad(
                tileVertices,
                {
                    position.x + 2.f,
                    position.y + 2.f,
                    tileSize - 4.f,
                    tileSize - 4.f
                },
                fillColor
            );

            const bool exposedTop = row == 0U || (data.rows[row - 1U][column] != '#' && data.rows[row - 1U][column] != '?' && data.rows[row - 1U][column] != '!');
            if (exposedTop) {
                appendQuad(
                    tileVertices,
                    {position.x, position.y, tileSize, 10.f},
                    palette.surface
                );
                appendQuad(
                    tileVertices,
                    {position.x, position.y, tileSize, 3.f},
                    palette.surfaceHighlight
                );
            }

            const bool exposedLeft =
                column == 0U || (data.rows[row][column - 1U] != '#' && data.rows[row][column - 1U] != '?' && data.rows[row][column - 1U] != '!');
            const bool exposedRight =
                column + 1U >= data.rows[row].size() ||
                (data.rows[row][column + 1U] != '#' && data.rows[row][column + 1U] != '?' && data.rows[row][column + 1U] != '!');
            const bool exposedBottom =
                row + 1U >= data.rows.size() ||
                (data.rows[row + 1U][column] != '#' && data.rows[row + 1U][column] != '?' && data.rows[row + 1U][column] != '!');

            if (exposedLeft) {
                appendQuad(
                    tileVertices,
                    {position.x, position.y + 3.f, 3.f, tileSize - 3.f},
                    brighten(fillColor, 24U)
                );
            }
            if (exposedRight) {
                appendQuad(
                    tileVertices,
                    {position.x + tileSize - 4.f, position.y, 4.f, tileSize},
                    palette.outline
                );
            }
            if (exposedBottom) {
                appendQuad(
                    tileVertices,
                    {position.x, position.y + tileSize - 4.f, tileSize, 4.f},
                    palette.outline
                );
            }
        }
    }
}

void ProceduralTileRenderer::render(
    sf::RenderTarget& target,
    const sf::VertexArray& tileVertices,
    const sf::VertexArray& sceneryVertices,
    const sf::VertexArray& backgroundVertices
) const
{
    target.draw(tileVertices);
}

sf::Color ProceduralTileRenderer::getTileColor(const LevelData& data) const {
    const TilePalette palette = paletteFor(data.difficulty);
    return palette.lightFill;
}

void ProceduralTileRenderer::buildSingleTile(
    sf::VertexArray& vertices,
    const LevelData& data,
    int row,
    int col,
    sf::Vector2f offset
) const {
    const TilePalette palette = paletteFor(data.difficulty);
    const float tileSize = static_cast<float>(data.tileSize);

    const sf::Vector2f position{
        static_cast<float>(col) * tileSize + offset.x,
        static_cast<float>(row) * tileSize + offset.y
    };

    const sf::Color fillColor = row % 2 == 0
        ? palette.lightFill
        : palette.darkFill;
        
    appendQuad(
        vertices,
        {position.x, position.y, tileSize, tileSize},
        palette.outline
    );
    appendQuad(
        vertices,
        {
            position.x + 2.f,
            position.y + 2.f,
            tileSize - 4.f,
            tileSize - 4.f
        },
        fillColor
    );

    const bool exposedTop = row == 0 || data.rows[row - 1][col] != '#';
    if (exposedTop) {
        appendQuad(
            vertices,
            {position.x, position.y, tileSize, 10.f},
            palette.surface
        );
        appendQuad(
            vertices,
            {position.x, position.y, tileSize, 3.f},
            palette.surfaceHighlight
        );
    }

    const bool exposedLeft = col == 0 || data.rows[row][col - 1] != '#';
    const bool exposedRight = col + 1 >= static_cast<int>(data.rows[row].size()) || data.rows[row][col + 1] != '#';
    const bool exposedBottom = row + 1 >= static_cast<int>(data.rows.size()) || data.rows[row + 1][col] != '#';

    if (exposedLeft) {
        appendQuad(
            vertices,
            {position.x, position.y + 3.f, 3.f, tileSize - 3.f},
            brighten(fillColor, 24U)
        );
    }
    if (exposedRight) {
        appendQuad(
            vertices,
            {position.x + tileSize - 4.f, position.y, 4.f, tileSize},
            palette.outline
        );
    }
    if (exposedBottom) {
        appendQuad(
            vertices,
            {position.x, position.y + tileSize - 4.f, tileSize, 4.f},
            palette.outline
        );
    }
}

bool ProceduralTileRenderer::isTransparent(const LevelData& data, int row, int col, int localX, int localY) const {
    return false;
}