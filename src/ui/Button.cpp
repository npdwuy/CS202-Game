#include "ui/Button.hpp"

Button::Button(const std::string &textStr, const sf::Font &font,
               sf::Vector2f position, sf::Vector2f size,
               unsigned int characterSize) :
               m_normalColor(sf::Color(100, 100, 100, 128)),
               m_hoverColor(sf::Color(150, 150, 150, 128)),
               m_textColor(sf::Color::White),
               m_isHovered(false)
{
    m_shape.setSize(size);
    m_shape.setOrigin(size.x / 2.f, size.y / 2.f);
    m_shape.setPosition(position);
    m_shape.setFillColor(m_normalColor);
    m_shape.setOutlineColor(sf::Color::White);
    m_shape.setOutlineThickness(2.f);

    m_text.setFont(font);
    m_text.setString(textStr);
    m_text.setCharacterSize(characterSize);
    m_text.setFillColor(m_textColor);

    centerText();
}

Button::Button(const std::string &textStr, const sf::Font &font,
               const sf::Texture &texture,
               sf::Vector2f position, sf::Vector2f size,
               unsigned int characterSize) :
               Button(textStr, font, position, size, characterSize)
{
    m_shape.setTexture(&texture);
    m_shape.setOutlineThickness(0.f);
}

void Button::centerText() {
    sf::FloatRect textBounds = m_text.getLocalBounds();
    m_text.setOrigin(textBounds.left + textBounds.width / 2.f,
                      textBounds.top + textBounds.height / 2.f);
    m_text.setPosition(m_shape.getPosition());
}

void Button::update(sf::Vector2f mousePos) {
    m_isHovered = m_shape.getGlobalBounds().contains(mousePos);
    if (m_isHovered) {
        m_shape.setFillColor(m_hoverColor);
        m_shape.setScale(1.08f, 1.08f);
        m_text.setScale(1.08f, 1.08f);
    } else {
        m_shape.setFillColor(m_normalColor);
        m_shape.setScale(1.f, 1.f);
        m_text.setScale(1.f, 1.f);
    }
}

bool Button::isClicked(const sf::Event &event, sf::Vector2f mousePos) const {
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        return m_shape.getGlobalBounds().contains(mousePos);
    }
    return false;
}

bool Button::handleClick(const sf::Event &event, sf::Vector2f mousePos) {
    if (isClicked(event, mousePos)) {
        if (m_callback) {
            m_callback();
        }
        return true;
    }
    return false;
}

void Button::setCallback(std::function<void()> callback) {
    m_callback = callback;
}

void Button::render(sf::RenderWindow &window) const {
    window.draw(m_shape);
    window.draw(m_text);
}

void Button::setColors(sf::Color normalColor, sf::Color hoverColor, sf::Color textColor) {
    m_normalColor = normalColor;
    m_hoverColor = hoverColor;
    m_textColor = textColor;
    m_shape.setFillColor(m_isHovered ? m_hoverColor : m_normalColor);
    m_text.setFillColor(m_textColor);
}

void Button::setPosition(sf::Vector2f position) {
    m_shape.setPosition(position);
    centerText();
}

void Button::setText(const std::string &textStr) {
    m_text.setString(textStr);
    centerText();
}

sf::FloatRect Button::getGlobalBounds() const {
    return m_shape.getGlobalBounds();
}