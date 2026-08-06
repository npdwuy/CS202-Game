#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include <memory>
#include "commands/ICommand.hpp"

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
    bool handleClick(const sf::Event &event, sf::Vector2f mousePos);
    void setCommand(std::unique_ptr<ICommand> command);

    void render(sf::RenderWindow &window) const;

    void setColors(sf::Color normalColor, sf::Color hoverColor, sf::Color textColor);
    void setPosition(sf::Vector2f position);
    void setText(const std::string &textStr);
    void setCornerSize(float cornerSize);

    bool isHovered() const { return m_isHovered; }

    sf::FloatRect getGlobalBounds() const;

private: // Private methods
    void centerText();
    void updateNineSliceGeometry();

private: // Private Attributes
    sf::RectangleShape m_shape;
    sf::Text m_text;

    const sf::Texture *m_texture = nullptr;
    sf::VertexArray m_vertices;
    float m_cornerSize = 10.f;

    sf::Color m_normalColor;
    sf::Color m_hoverColor;
    sf::Color m_textColor;

    std::unique_ptr<ICommand> m_command;

    bool m_isHovered;
};

