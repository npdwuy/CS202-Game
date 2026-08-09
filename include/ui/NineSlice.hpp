#pragma once

#include <SFML/Graphics.hpp>

class NineSlice : public sf::Drawable, public sf::Transformable {
public:
    NineSlice();
    NineSlice(const sf::Texture& texture, sf::Vector2f size, float cornerSize = 10.f);
    ~NineSlice() = default;

    void setTexture(const sf::Texture& texture);
    void setSize(sf::Vector2f size);
    void setCornerSize(float cornerSize);
    void setColor(sf::Color color);

    sf::Vector2f getSize() const;
    sf::FloatRect getGlobalBounds() const;

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    void updateGeometry();

    const sf::Texture* m_texture;
    sf::VertexArray m_vertices;
    sf::Vector2f m_size;
    float m_cornerSize;
    sf::Color m_color;
};
