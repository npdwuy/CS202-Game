#pragma once

#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

class RoundedRectangleShape : public sf::Shape {
public:
    explicit RoundedRectangleShape(const sf::Vector2f& size = sf::Vector2f(0, 0), float radius = 0.f, unsigned int cornerPointCount = 10);
    
    void setSize(const sf::Vector2f& size);
    const sf::Vector2f& getSize() const;
    
    void setCornerRadius(float radius);
    float getCornerRadius() const;
    
    void setCornerPointCount(unsigned int count);

    // Call after changing size or radius to force vertex array rebuild.
    void refresh() { update(); }
    
    std::size_t getPointCount() const override;
    sf::Vector2f getPoint(std::size_t index) const override;

private:
    sf::Vector2f m_size;
    float m_radius;
    unsigned int m_cornerPointCount;
};
