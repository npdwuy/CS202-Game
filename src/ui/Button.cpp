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
    m_texture = &texture;
    m_shape.setTexture(nullptr);
    m_shape.setOutlineThickness(0.f);
    updateNineSliceGeometry();
}

void Button::centerText() {
    sf::FloatRect textBounds = m_text.getLocalBounds();
    m_text.setOrigin(textBounds.left + textBounds.width / 2.f,
                      textBounds.top + textBounds.height / 2.f);
    sf::Vector2f offset(0.f, -5.f);
    m_text.setPosition(m_shape.getPosition() + offset);
}

void Button::updateNineSliceGeometry() {
    if (!m_texture) return;

    sf::Vector2f size = m_shape.getSize();
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
    if (screenCornerX * 2.f > size.x) screenCornerX = size.x / 2.f;
    if (screenCornerY * 2.f > size.y) screenCornerY = size.y / 2.f;

    float w = size.x;
    float h = size.y;

    float x[4] = { 0.f, screenCornerX, w - screenCornerX, w };
    float y[4] = { 0.f, screenCornerY, h - screenCornerY, h };

    float tw = static_cast<float>(texSize.x);
    float th = static_cast<float>(texSize.y);

    float u[4] = { 0.f, texCornerX, tw - texCornerX, tw };
    float v[4] = { 0.f, texCornerY, th - texCornerY, th };


    sf::Color currentColor = m_isHovered ? m_hoverColor : m_normalColor;

    m_vertices.setPrimitiveType(sf::Quads);
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
                m_vertices[idx + k].color = currentColor;
            }

            idx += 4;
        }
    }
}

void Button::update(sf::Vector2f mousePos) {
    m_isHovered = m_shape.getGlobalBounds().contains(mousePos);
    sf::Color currentColor = m_isHovered ? m_hoverColor : m_normalColor;
    if (m_isHovered) {
        m_shape.setFillColor(m_hoverColor);
        m_shape.setScale(1.08f, 1.08f);
        m_text.setScale(1.08f, 1.08f);
    } else {
        m_shape.setFillColor(m_normalColor);
        m_shape.setScale(1.f, 1.f);
        m_text.setScale(1.f, 1.f);
    }

    if (m_texture) {
        for (std::size_t i = 0; i < m_vertices.getVertexCount(); ++i) {
            m_vertices[i].color = currentColor;
        }
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
        if (m_command) {
            m_command->execute();
        }
        return true;
    }
    return false;
}

void Button::setCommand(std::unique_ptr<ICommand> command) {
    m_command = std::move(command);
}

void Button::render(sf::RenderWindow &window) const {
    if (m_texture) {
        sf::RenderStates states;
        states.transform = m_shape.getTransform();
        states.texture = m_texture;
        window.draw(m_vertices, states);
    } else {
        window.draw(m_shape);
    }
    window.draw(m_text);
}

void Button::setColors(sf::Color normalColor, sf::Color hoverColor, sf::Color textColor) {
    m_normalColor = normalColor;
    m_hoverColor = hoverColor;
    m_textColor = textColor;
    sf::Color currentColor = m_isHovered ? m_hoverColor : m_normalColor;
    m_shape.setFillColor(currentColor);
    m_text.setFillColor(m_textColor);

    if (m_texture) {
        for (std::size_t i = 0; i < m_vertices.getVertexCount(); ++i) {
            m_vertices[i].color = currentColor;
        }
    }
}

void Button::setPosition(sf::Vector2f position) {
    m_shape.setPosition(position);
    centerText();
}

void Button::setText(const std::string &textStr) {
    m_text.setString(textStr);
    centerText();
}

void Button::setCornerSize(float cornerSize) {
    m_cornerSize = cornerSize;
    updateNineSliceGeometry();
}

sf::FloatRect Button::getGlobalBounds() const {
    return m_shape.getGlobalBounds();
}