#pragma once

#include "levels/LevelData.hpp"

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

class Character;

class TileMap {
public:
    void load(const std::string& path);
    void render(sf::RenderWindow& window) const;
    void resolveCollision(Character& character, sf::Time timePerFrame) const;

    const LevelData& data() const;
    sf::FloatRect exitBounds() const;
    sf::FloatRect worldBounds() const;
    bool isSolidAt(sf::Vector2f worldPosition) const;
    bool intersectsSolid(const sf::FloatRect& bounds) const;

private:
    void rebuildGeometry();

    LevelData m_data;
    std::vector<sf::RectangleShape> m_tiles;
    sf::RectangleShape m_exitPole;
    sf::RectangleShape m_exitFlag;
};
