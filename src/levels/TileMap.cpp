#include "levels/TileMap.hpp"

#include "entities/player/Character.hpp"
#include "levels/LevelLoader.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace {

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

}

void TileMap::load(const std::string& path) {
    m_data = LevelLoader::loadFromFile(path);
    rebuildGeometry();
}

void TileMap::render(sf::RenderWindow& window) const {
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
        const float right = bounds.left + bounds.width - 0.001f;
        const float bottom = bounds.top + bounds.height - 0.001f;

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
            position.x = tile.left - width;
        } else if (velocity.x < 0.f) {
            position.x = tile.left + tile.width;
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
    m_tileVertices.clear();

    const float tileSize = static_cast<float>(m_data.tileSize);

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
                ? sf::Color(158, 93, 52)
                : sf::Color(139, 78, 43);
            appendQuad(
                m_tileVertices,
                {position.x, position.y, tileSize, tileSize},
                sf::Color(91, 51, 31)
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
