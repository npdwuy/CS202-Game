#include "ui/RoundedRectangleShape.hpp"
#include <cmath>

RoundedRectangleShape::RoundedRectangleShape(const sf::Vector2f& size, float radius, unsigned int cornerPointCount)
    : m_size(size), m_radius(radius), m_cornerPointCount(cornerPointCount)
{
    update();
}

void RoundedRectangleShape::setSize(const sf::Vector2f& size)
{
    m_size = size;
    update();
}

const sf::Vector2f& RoundedRectangleShape::getSize() const
{
    return m_size;
}

void RoundedRectangleShape::setCornerRadius(float radius)
{
    m_radius = radius;
    update();
}

float RoundedRectangleShape::getCornerRadius() const
{
    return m_radius;
}

void RoundedRectangleShape::setCornerPointCount(unsigned int count)
{
    m_cornerPointCount = count;
    update();
}

std::size_t RoundedRectangleShape::getPointCount() const
{
    return m_cornerPointCount * 4;
}

sf::Vector2f RoundedRectangleShape::getPoint(std::size_t index) const
{
    if (index >= m_cornerPointCount * 4) {
        return sf::Vector2f(0, 0);
    }
    
    float deltaAngle = 90.0f / (m_cornerPointCount - 1);
    sf::Vector2f center;
    unsigned int centerIndex = index / m_cornerPointCount;
    static const float pi = 3.141592654f;
    
    switch (centerIndex) {
        case 0: center.x = m_size.x - m_radius; center.y = m_radius; break;
        case 1: center.x = m_radius; center.y = m_radius; break;
        case 2: center.x = m_radius; center.y = m_size.y - m_radius; break;
        case 3: center.x = m_size.x - m_radius; center.y = m_size.y - m_radius; break;
    }
    
    float angle = centerIndex * 90.0f + (index % m_cornerPointCount) * deltaAngle;
    float rad = angle * pi / 180.0f;
    
    return sf::Vector2f(
        m_radius * std::cos(rad) + center.x,
        -m_radius * std::sin(rad) + center.y
    );
}
