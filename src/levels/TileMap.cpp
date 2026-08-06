#include "levels/TileMap.hpp"

#include "entities/player/Character.hpp"
#include "levels/LevelLoader.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace {

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

TilePalette paletteFor(const std::string& difficulty) {
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

void appendQuad(
    sf::VertexArray& vertices,
    const sf::FloatRect& bounds,
    sf::Color color
) {
    vertices.append({{bounds.left, bounds.top}, color});
    vertices.append({{bounds.left + bounds.width, bounds.top}, color});
    vertices.append({
        {bounds.left + bounds.width, bounds.top + bounds.height},
        color
    });
    vertices.append({{bounds.left, bounds.top + bounds.height}, color});
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

sf::Color brighten(sf::Color color, unsigned int amount);

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

sf::Color brighten(sf::Color color, unsigned int amount) {
    return {
        static_cast<sf::Uint8>(std::min(255U, color.r + amount)),
        static_cast<sf::Uint8>(std::min(255U, color.g + amount)),
        static_cast<sf::Uint8>(std::min(255U, color.b + amount)),
        color.a
    };
}

}

void TileMap::load(const std::string& path) {
    m_data = LevelLoader::loadFromFile(path);
    rebuildGeometry();
}

void TileMap::render(sf::RenderWindow& window) const {
    window.draw(m_backgroundVertices);
    window.draw(m_sceneryVertices);
    window.draw(m_tileVertices);

    window.draw(m_exitPole);
    window.draw(m_exitFlag);
}

void TileMap::resolveCollision(
    Character& character,
    sf::Time timePerFrame
) const {
    const float deltaTime = timePerFrame.asSeconds();
    if (deltaTime <= 0.f) {
        return;
    }

    sf::Vector2f position = character.position();
    sf::Vector2f velocity = character.velocity();
    const float width = character.width();
    const float height = character.height();

    character.setOnGround(false);
    character.setHitRoof(false);

    const auto forEachNearbySolid = [this](
        const sf::FloatRect& bounds,
        const auto& visitor
    ) {
        if (m_data.rows.empty() || m_data.tileSize == 0U) {
            return;
        }

        const float tileSize = static_cast<float>(m_data.tileSize);
        // Slightly shrink bounds to avoid treating adjacent tiles as overlapping
        // Increase shrink margin to reduce false‑positive overlaps
        const float right = bounds.left + bounds.width - 0.2f;
        const float bottom = bounds.top + bounds.height - 0.2f;

        int firstColumn = static_cast<int>(std::floor(bounds.left / tileSize));
        int lastColumn = static_cast<int>(std::floor(right / tileSize));
        int firstRow = static_cast<int>(std::floor(bounds.top / tileSize));
        int lastRow = static_cast<int>(std::floor(bottom / tileSize));

        firstColumn = std::max(0, firstColumn);
        firstRow = std::max(0, firstRow);
        lastColumn = std::min(
            static_cast<int>(m_data.widthInTiles()) - 1,
            lastColumn
        );
        lastRow = std::min(
            static_cast<int>(m_data.heightInTiles()) - 1,
            lastRow
        );

        if (firstColumn > lastColumn || firstRow > lastRow) {
            return;
        }

        for (int row = firstRow; row <= lastRow; ++row) {
            for (int column = firstColumn; column <= lastColumn; ++column) {
                if (m_data.rows[row][column] != '#') {
                    continue;
                }

                visitor(sf::FloatRect(
                    static_cast<float>(column) * tileSize,
                    static_cast<float>(row) * tileSize,
                    tileSize,
                    tileSize
                ));
            }
        }
    };

    position.x += velocity.x * deltaTime;
    sf::FloatRect horizontalBounds(position.x, position.y, width, height);

    forEachNearbySolid(horizontalBounds, [&](const sf::FloatRect& tile) {
        if (!horizontalBounds.intersects(tile)) {
            return;
        }

        if (velocity.x > 0.f) {
            // Move Mario just left of the tile with a small skin to avoid immediate re‑collision
            // Apply a larger skin to ensure separation from the tile
            position.x = tile.left - width - 0.1f;
        } else if (velocity.x < 0.f) {
            // Move Mario just right of the tile with a small skin
            // Apply a larger skin to ensure separation from the tile
            position.x = tile.left + tile.width + 0.1f;
        }

        velocity.x = 0.f;
        horizontalBounds.left = position.x;
    });

    const float maximumX = std::max(0.f, m_data.worldSize().x - width);
    if (position.x < 0.f) {
        position.x = 0.f;
        velocity.x = 0.f;
    } else if (position.x > maximumX) {
        position.x = maximumX;
        velocity.x = 0.f;
    }

    position.y += velocity.y * deltaTime;
    sf::FloatRect verticalBounds(position.x, position.y, width, height);

    forEachNearbySolid(verticalBounds, [&](const sf::FloatRect& tile) {
        if (!verticalBounds.intersects(tile)) {
            return;
        }

        if (velocity.y > 0.f) {
            position.y = tile.top - height;
            character.setOnGround(true);
        } else if (velocity.y < 0.f) {
            position.y = tile.top + tile.height;
            character.setHitRoof(true);
        }

        velocity.y = 0.f;
        verticalBounds.top = position.y;
    });

    character.setPosition(position);
    character.setVelocity(velocity);
}

const LevelData& TileMap::data() const {
    if (m_data.rows.empty()) {
        throw std::logic_error("TileMap data requested before loading a level.");
    }
    return m_data;
}

sf::FloatRect TileMap::exitBounds() const {
    const float tileSize = static_cast<float>(m_data.tileSize);
    return {
        m_data.exitPosition.x,
        m_data.exitPosition.y,
        tileSize,
        tileSize
    };
}

sf::FloatRect TileMap::worldBounds() const {
    const sf::Vector2f size = m_data.worldSize();
    return {0.f, 0.f, size.x, size.y};
}

bool TileMap::isSolidAt(sf::Vector2f worldPosition) const {
    if (
        worldPosition.x < 0.f || worldPosition.y < 0.f ||
        m_data.tileSize == 0U
    ) {
        return false;
    }

    const std::size_t column = static_cast<std::size_t>(
        worldPosition.x / static_cast<float>(m_data.tileSize)
    );
    const std::size_t row = static_cast<std::size_t>(
        worldPosition.y / static_cast<float>(m_data.tileSize)
    );

    if (row >= m_data.rows.size() || column >= m_data.rows[row].size()) {
        return false;
    }

    return m_data.rows[row][column] == '#';
}

bool TileMap::intersectsSolid(const sf::FloatRect& bounds) const {
    if (m_data.rows.empty() || m_data.tileSize == 0U) {
        return false;
    }

    const float tileSize = static_cast<float>(m_data.tileSize);
    int firstColumn = static_cast<int>(std::floor(bounds.left / tileSize));
    int lastColumn = static_cast<int>(std::floor(
        (bounds.left + bounds.width - 0.001f) / tileSize
    ));
    int firstRow = static_cast<int>(std::floor(bounds.top / tileSize));
    int lastRow = static_cast<int>(std::floor(
        (bounds.top + bounds.height - 0.001f) / tileSize
    ));

    firstColumn = std::max(0, firstColumn);
    firstRow = std::max(0, firstRow);
    lastColumn = std::min(
        static_cast<int>(m_data.widthInTiles()) - 1,
        lastColumn
    );
    lastRow = std::min(
        static_cast<int>(m_data.heightInTiles()) - 1,
        lastRow
    );

    if (firstColumn > lastColumn || firstRow > lastRow) {
        return false;
    }

    for (int row = firstRow; row <= lastRow; ++row) {
        for (int column = firstColumn; column <= lastColumn; ++column) {
            if (m_data.rows[row][column] != '#') {
                continue;
            }

            const sf::FloatRect tile(
                static_cast<float>(column) * tileSize,
                static_cast<float>(row) * tileSize,
                tileSize,
                tileSize
            );
            if (bounds.intersects(tile)) {
                return true;
            }
        }
    }

    return false;
}

void TileMap::rebuildGeometry() {
    m_backgroundVertices.clear();
    m_sceneryVertices.clear();
    m_tileVertices.clear();

    const float tileSize = static_cast<float>(m_data.tileSize);
    const TilePalette palette = paletteFor(m_data.difficulty);
    const sf::Vector2f worldSize = m_data.worldSize();
    appendGradientQuad(
        m_backgroundVertices,
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
            m_sceneryVertices,
            120.f + static_cast<float>(index) * 470.f,
            890.f,
            520.f,
            175.f + static_cast<float>(index % 2) * 35.f,
            palette.farHill
        );
    }
    for (int index = 0; index < 4; ++index) {
        appendHill(
            m_sceneryVertices,
            300.f + static_cast<float>(index) * 610.f,
            930.f,
            690.f,
            225.f + static_cast<float>((index + 1) % 2) * 45.f,
            palette.nearHill
        );
    }

    for (std::size_t row = 0; row < m_data.rows.size(); ++row) {
        for (std::size_t column = 0; column < m_data.rows[row].size(); ++column) {
            if (m_data.rows[row][column] != '#') {
                continue;
            }

            const sf::Vector2f position{
                static_cast<float>(column) * tileSize,
                static_cast<float>(row) * tileSize
            };

            const sf::Color fillColor = row % 2U == 0U
                ? palette.lightFill
                : palette.darkFill;
            appendQuad(
                m_tileVertices,
                {position.x, position.y, tileSize, tileSize},
                palette.outline
            );
            appendQuad(
                m_tileVertices,
                {
                    position.x + 2.f,
                    position.y + 2.f,
                    tileSize - 4.f,
                    tileSize - 4.f
                },
                fillColor
            );

            const bool exposedTop = row == 0U || m_data.rows[row - 1U][column] != '#';
            if (exposedTop) {
                appendQuad(
                    m_tileVertices,
                    {position.x, position.y, tileSize, 10.f},
                    palette.surface
                );
                appendQuad(
                    m_tileVertices,
                    {position.x, position.y, tileSize, 3.f},
                    palette.surfaceHighlight
                );
            }

            const bool exposedLeft =
                column == 0U || m_data.rows[row][column - 1U] != '#';
            const bool exposedRight =
                column + 1U >= m_data.rows[row].size() ||
                m_data.rows[row][column + 1U] != '#';
            const bool exposedBottom =
                row + 1U >= m_data.rows.size() ||
                m_data.rows[row + 1U][column] != '#';

            if (exposedLeft) {
                appendQuad(
                    m_tileVertices,
                    {position.x, position.y + 3.f, 3.f, tileSize - 3.f},
                    brighten(fillColor, 24U)
                );
            }
            if (exposedRight) {
                appendQuad(
                    m_tileVertices,
                    {position.x + tileSize - 4.f, position.y, 4.f, tileSize},
                    palette.outline
                );
            }
            if (exposedBottom) {
                appendQuad(
                    m_tileVertices,
                    {position.x, position.y + tileSize - 4.f, tileSize, 4.f},
                    palette.outline
                );
            }

            sf::Color detailColor = palette.outline;
            detailColor.a = 72U;
            const std::size_t detailPattern = (row * 17U + column * 31U) % 3U;
            if (detailPattern == 0U) {
                appendQuad(
                    m_tileVertices,
                    {position.x + 11.f, position.y + 20.f, 13.f, 3.f},
                    detailColor
                );
                appendQuad(
                    m_tileVertices,
                    {position.x + 28.f, position.y + 34.f, 8.f, 3.f},
                    detailColor
                );
            } else if (detailPattern == 1U) {
                appendQuad(
                    m_tileVertices,
                    {position.x + 26.f, position.y + 17.f, 10.f, 3.f},
                    detailColor
                );
                appendQuad(
                    m_tileVertices,
                    {position.x + 9.f, position.y + 36.f, 7.f, 3.f},
                    detailColor
                );
            } else {
                appendQuad(
                    m_tileVertices,
                    {position.x + 16.f, position.y + 29.f, 15.f, 3.f},
                    detailColor
                );
            }
        }
    }

    m_exitPole.setSize({4.f, tileSize});
    m_exitPole.setPosition(
        m_data.exitPosition.x + tileSize * 0.5f,
        m_data.exitPosition.y
    );
    m_exitPole.setFillColor(sf::Color(235, 235, 235));

    m_exitFlag.setSize({tileSize * 0.55f, tileSize * 0.35f});
    m_exitFlag.setPosition(
        m_data.exitPosition.x + tileSize * 0.5f,
        m_data.exitPosition.y
    );
    m_exitFlag.setFillColor(sf::Color(45, 205, 80));
}
