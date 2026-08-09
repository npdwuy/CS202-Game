#pragma once

#include <SFML/Graphics.hpp>
#include "ui/NineSlice.hpp"
#include <memory>

class Panel {
public:
    Panel(const sf::Texture& texture, sf::Vector2f position, sf::Vector2f size, float cornerSize = 10.f);
    ~Panel() = default;

    void setPosition(sf::Vector2f position);
    void setSize(sf::Vector2f size);
    void setCornerSize(float cornerSize);
    void setColor(sf::Color color);

    sf::Vector2f getSize() const;
    sf::FloatRect getGlobalBounds() const;

    void render(sf::RenderWindow& window) const;

private:
    std::unique_ptr<NineSlice> m_nineSlice;
};
