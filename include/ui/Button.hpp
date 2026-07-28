#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class Button {
public:
    Button(const std::string &textStr,
           const sf::Font &font,
           sf::Vector2f position,
           sf::Vector2f size = sf::Vector2f(300.f, 60.f),
           unsigned int characterSize = 28);

    Button(const std::string &textStr,
           const sf::Font &font,
           const sf::Texture &texture,
           sf::Vector2f position,
           sf::Vector2f size = sf::Vector2f(300.f, 60.f),
           unsigned int characterSize = 28);

    ~Button() = default;

    void update(sf::Vector2f mousePos);

    bool isClicked(const sf::Event &event, sf::Vector2f mousePos) const;

    void render(sf::RenderWindow &window) const;

    void setColors(sf::Color normalColor, sf::Color hoverColor, sf::Color textColor);
    void setPosition(sf::Vector2f position);
    void setText(const std::string &textStr);

    bool isHovered() const { return m_isHovered; }

    sf::FloatRect getGlobalBounds() const;

private: // Private methods
    void centerText();

private: // Private Attributes
    sf::RectangleShape m_shape;
    sf::Text m_text;

    sf::Color m_normalColor;
    sf::Color m_hoverColor;
    sf::Color m_textColor;

    bool m_isHovered;
};
