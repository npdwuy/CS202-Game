#include "ui/Panel.hpp"

Panel::Panel(sf::Vector2f position, sf::Vector2f size, float cornerSize)
{
    m_shape.setSize(size);
    m_shape.setCornerRadius(cornerSize);
    m_shape.setCornerPointCount(10);
    m_shape.setOrigin(size.x / 2.f, size.y / 2.f);
    m_shape.setPosition(position);
}

void Panel::setPosition(sf::Vector2f position) {
    m_shape.setPosition(position);
}

void Panel::setSize(sf::Vector2f size) {
    m_shape.setSize(size);
    m_shape.setOrigin(size.x / 2.f, size.y / 2.f);
}

void Panel::setCornerSize(float cornerSize) {
    m_shape.setCornerRadius(cornerSize);
}

void Panel::setColor(sf::Color color) {
    m_shape.setFillColor(color);
}

sf::Vector2f Panel::getSize() const {
    return m_shape.getSize();
}

sf::FloatRect Panel::getGlobalBounds() const {
    return m_shape.getGlobalBounds();
}

void Panel::render(sf::RenderWindow& window) const {
    window.draw(m_shape);
}
