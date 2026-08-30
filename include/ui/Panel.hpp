#pragma once

#include <SFML/Graphics.hpp>
#include "ui/RoundedRectangleShape.hpp"
#include <memory>

class Panel {
public:
    Panel(sf::Vector2f position, sf::Vector2f size, float cornerSize = 10.f);
    ~Panel() = default;

    void setPosition(sf::Vector2f position);
    void setSize(sf::Vector2f size);
    void setCornerSize(float cornerSize);
    void setColor(sf::Color color);

    sf::Vector2f getSize() const;
    sf::FloatRect getGlobalBounds() const;

    void render(sf::RenderWindow& window) const;

private:
    RoundedRectangleShape m_shape;
};
