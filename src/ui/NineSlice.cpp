#include "ui/NineSlice.hpp"

NineSlice::NineSlice()
    : m_texture(nullptr),
      m_size(0.f, 0.f),
      m_cornerSize(10.f),
      m_color(sf::Color::White)
{
    m_vertices.setPrimitiveType(sf::Quads);
}

NineSlice::NineSlice(const sf::Texture& texture, sf::Vector2f size, float cornerSize)
    : m_texture(&texture),
      m_size(size),
      m_cornerSize(cornerSize),
      m_color(sf::Color::White)
{
    m_vertices.setPrimitiveType(sf::Quads);
    updateGeometry();
}

void NineSlice::setTexture(const sf::Texture& texture) {
    m_texture = &texture;
    updateGeometry();
}

void NineSlice::setSize(sf::Vector2f size) {
    m_size = size;
    updateGeometry();
}

void NineSlice::setCornerSize(float cornerSize) {
    m_cornerSize = cornerSize;
    updateGeometry();
}

void NineSlice::setColor(sf::Color color) {
    m_color = color;
    for (std::size_t i = 0; i < m_vertices.getVertexCount(); ++i) {
        m_vertices[i].color = m_color;
    }
}

sf::Vector2f NineSlice::getSize() const {
    return m_size;
}

sf::FloatRect NineSlice::getGlobalBounds() const {
    return getTransform().transformRect(sf::FloatRect(0.f, 0.f, m_size.x, m_size.y));
}

void NineSlice::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (m_texture) {
        states.transform *= getTransform();
        states.texture = m_texture;
        target.draw(m_vertices, states);
    }
}

void NineSlice::updateGeometry() {
    if (!m_texture) return;

    sf::Vector2u texSize = m_texture->getSize();
    if (texSize.x == 0 || texSize.y == 0) return;

    // Corner size in texture coordinates
    float texCornerX = m_cornerSize;
    float texCornerY = m_cornerSize;
    if (texCornerX * 2.f > texSize.x) texCornerX = texSize.x / 2.f;
    if (texCornerY * 2.f > texSize.y) texCornerY = texSize.y / 2.f;

    // Corner size in screen coordinates
    float screenCornerX = m_cornerSize;
    float screenCornerY = m_cornerSize;
    if (screenCornerX * 2.f > texSize.x) screenCornerX = texSize.x / 2.f;
    if (screenCornerY * 2.f > texSize.y) screenCornerY = texSize.y / 2.f;
    if (screenCornerX * 2.f > m_size.x) screenCornerX = m_size.x / 2.f;
    if (screenCornerY * 2.f > m_size.y) screenCornerY = m_size.y / 2.f;

    float w = m_size.x;
    float h = m_size.y;

    float x[4] = { 0.f, screenCornerX, w - screenCornerX, w };
    float y[4] = { 0.f, screenCornerY, h - screenCornerY, h };

    float tw = static_cast<float>(texSize.x);
    float th = static_cast<float>(texSize.y);

    float u[4] = { 0.f, texCornerX, tw - texCornerX, tw };
    float v[4] = { 0.f, texCornerY, th - texCornerY, th };

    m_vertices.resize(36);

    std::size_t idx = 0;
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            m_vertices[idx + 0].position = sf::Vector2f(x[c],     y[r]);
            m_vertices[idx + 1].position = sf::Vector2f(x[c + 1], y[r]);
            m_vertices[idx + 2].position = sf::Vector2f(x[c + 1], y[r + 1]);
            m_vertices[idx + 3].position = sf::Vector2f(x[c],     y[r + 1]);

            m_vertices[idx + 0].texCoords = sf::Vector2f(u[c],     v[r]);
            m_vertices[idx + 1].texCoords = sf::Vector2f(u[c + 1], v[r]);
            m_vertices[idx + 2].texCoords = sf::Vector2f(u[c + 1], v[r + 1]);
            m_vertices[idx + 3].texCoords = sf::Vector2f(u[c],     v[r + 1]);

            for (int k = 0; k < 4; ++k) {
                m_vertices[idx + k].color = m_color;
            }

            idx += 4;
        }
    }
}
