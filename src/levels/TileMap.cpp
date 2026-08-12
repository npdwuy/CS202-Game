#include "levels/TileMap.hpp"
#include "entities/player/Character.hpp"
#include "levels/LevelLoader.hpp"
#include "levels/renderer/ProceduralTileRenderer.hpp"
#include "levels/renderer/TexturedTileRenderer.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

void TileMap::load(const std::string& path) {
    m_data = LevelLoader::loadFromFile(path);

    std::string tilesetPath = "assets/sprites/tilesets/WU_Field_plain.png";
    if (m_data.difficulty == "Medium") {
        tilesetPath = "assets/sprites/tilesets/WU_Field_underground.png";
    } else if (m_data.difficulty == "Hard") {
        tilesetPath = "assets/sprites/tilesets/WU_Field_castle.png";
    }

    sf::Texture texture;
    if (texture.loadFromFile(tilesetPath)) {
        m_renderer = std::make_unique<TexturedTileRenderer>(std::move(texture));
    } else {
        m_renderer = std::make_unique<ProceduralTileRenderer>();
    }

    rebuildGeometry();
}

void TileMap::render(sf::RenderWindow& window) const {
    if (m_renderer) {
        m_renderer->render(window, m_tileVertices, m_sceneryVertices, m_backgroundVertices);
    } else {
        window.draw(m_backgroundVertices);
        window.draw(m_sceneryVertices);
        window.draw(m_tileVertices);
    }

    window.draw(m_exitPole);
    window.draw(m_exitFlag);

    for (const auto& debris : m_debris) {
        window.draw(debris.shape);
    }
}

void TileMap::resolveCollision(Character& character, sf::Time timePerFrame, std::function<void(int row, int col)> onHitRoof) const {
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
            if (onHitRoof) {
                int col = static_cast<int>(tile.left / m_data.tileSize);
                int row = static_cast<int>(tile.top / m_data.tileSize);
                onHitRoof(row, col);
            }
        }

        velocity.y = 0.f;
        verticalBounds.top = position.y;
    });

    character.setPosition(position);
    character.setVelocity(velocity);
}

void TileMap::update(sf::Time dt) {
    const float seconds = dt.asSeconds();
    const float gravity = 2000.f;

    for (auto it = m_debris.begin(); it != m_debris.end(); ) {
        it->velocity.y += gravity * seconds;
        it->position += it->velocity * seconds;
        it->rotation += it->rotationSpeed * seconds;
        it->lifeTime -= seconds;
        
        it->shape.setPosition(it->position);
        it->shape.setRotation(it->rotation);

        if (it->lifeTime <= 0.f || it->position.y > m_data.worldSize().y + 100.f) {
            it = m_debris.erase(it);
        } else {
            ++it;
        }
    }
}

void TileMap::breakBlock(int row, int col) {
    if (row < 0 || row >= m_data.rows.size() || col < 0 || col >= m_data.rows[row].size()) {
        return;
    }
    
    if (m_data.rows[row][col] == '#') {
        m_data.rows[row][col] = '.';
        rebuildGeometry();
        
        float ts = static_cast<float>(m_data.tileSize);
        float halfTs = ts * 0.5f;
        sf::Vector2f center(col * ts + halfTs, row * ts + halfTs);
        
        sf::Color blockColor(180, 100, 40); // Approximate dirt/brick color
        
        for (int i = 0; i < 4; ++i) {
            BlockDebris debris;
            debris.shape.setSize({halfTs, halfTs});
            debris.shape.setOrigin(halfTs * 0.5f, halfTs * 0.5f);
            debris.shape.setFillColor(blockColor);
            
            float dx = (i % 2 == 0) ? -1.f : 1.f;
            float dy = (i < 2) ? -1.f : 0.f;
            
            debris.position = center + sf::Vector2f(dx * halfTs * 0.5f, dy * halfTs * 0.5f);
            debris.velocity = sf::Vector2f(dx * 150.f, -400.f + dy * 150.f);
            debris.rotationSpeed = dx * 360.f; // 1 rotation per second
            debris.lifeTime = 2.0f;
            
            m_debris.push_back(debris);
        }
    }
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
        m_data.exitPosition.x + tileSize * 0.5f - 4.f,
        m_data.exitPosition.y - tileSize * 7.f,
        8.f,
        tileSize * 8.f
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
    if (m_renderer) {
        m_renderer->buildGeometry(m_tileVertices, m_sceneryVertices, m_backgroundVertices, m_data);
    }

    const float tileSize = static_cast<float>(m_data.tileSize);
    float poleHeight = tileSize * 8.f;
    m_exitPole.setSize({4.f, poleHeight});
    m_exitPole.setPosition(
        m_data.exitPosition.x + tileSize * 0.5f - 2.f,
        m_data.exitPosition.y - tileSize * 7.f
    );
    m_exitPole.setFillColor(sf::Color(235, 235, 235));

    m_exitFlag.setSize({tileSize * 0.55f, tileSize * 0.35f});
    m_flagY = m_data.exitPosition.y - tileSize * 7.f;
    m_exitFlag.setPosition(
        m_data.exitPosition.x + tileSize * 0.5f,
        m_flagY
    );
    m_exitFlag.setFillColor(sf::Color(45, 205, 80));
}

bool TileMap::updateFlagAnimation(sf::Time dt, float speed) {
    const float tileSize = static_cast<float>(m_data.tileSize);
    float targetY = m_data.exitPosition.y + tileSize - m_exitFlag.getSize().y;
    
    m_flagY += speed * dt.asSeconds();
    if (m_flagY >= targetY) {
        m_flagY = targetY;
        m_exitFlag.setPosition(m_exitFlag.getPosition().x, m_flagY);
        return true; // hit bottom
    }
    
    m_exitFlag.setPosition(m_exitFlag.getPosition().x, m_flagY);
    return false;
}

float TileMap::getPoleTopY() const {
    const float tileSize = static_cast<float>(m_data.tileSize);
    return m_data.exitPosition.y - tileSize * 7.f;
}

float TileMap::getPoleBottomY() const {
    const float tileSize = static_cast<float>(m_data.tileSize);
    return m_data.exitPosition.y + tileSize;
}

float TileMap::getPoleX() const {
    const float tileSize = static_cast<float>(m_data.tileSize);
    return m_data.exitPosition.x + tileSize * 0.5f;
}
