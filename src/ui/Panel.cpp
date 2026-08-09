#include "ui/Panel.hpp"

Panel::Panel(const sf::Texture& texture, sf::Vector2f position, sf::Vector2f size, float cornerSize)
{
    m_nineSlice = std::make_unique<NineSlice>(texture, size, cornerSize);
    m_nineSlice->setOrigin(size.x / 2.f, size.y / 2.f);
    m_nineSlice->setPosition(position);
}

void Panel::setPosition(sf::Vector2f position) {
    if (m_nineSlice) {
        m_nineSlice->setPosition(position);
    }
}

void Panel::setSize(sf::Vector2f size) {
    if (m_nineSlice) {
        m_nineSlice->setSize(size);
        m_nineSlice->setOrigin(size.x / 2.f, size.y / 2.f);
    }
}

void Panel::setCornerSize(float cornerSize) {
    if (m_nineSlice) {
        m_nineSlice->setCornerSize(cornerSize);
    }
}

void Panel::setColor(sf::Color color) {
    if (m_nineSlice) {
        m_nineSlice->setColor(color);
    }
}

sf::Vector2f Panel::getSize() const {
    if (m_nineSlice) {
        return m_nineSlice->getSize();
    }
    return sf::Vector2f(0.f, 0.f);
}

sf::FloatRect Panel::getGlobalBounds() const {
    if (m_nineSlice) {
        return m_nineSlice->getGlobalBounds();
    }
    return sf::FloatRect();
}

void Panel::render(sf::RenderWindow& window) const {
    if (m_nineSlice) {
        window.draw(*m_nineSlice);
    }
}
