#pragma once

#include <SFML/Graphics.hpp>

class Slider {
public:
    Slider(sf::Vector2f position, float width, float minVal, float maxVal, float currentVal, const sf::Font& font);

    void handleInput(const sf::Event& event, sf::Vector2f mousePos);
    void update(sf::Vector2f mousePos);
    void render(sf::RenderWindow& window) const;

    float getValue() const;
    void  setValue(float val);

private:
    void updateVisuals();

private:
    sf::RectangleShape m_track;       // background bar
    sf::RectangleShape m_fill;        // filled portion
    sf::CircleShape    m_handle;      // draggable knob
    sf::Text           m_valueText;   // "80%"

    float m_minVal, m_maxVal, m_value;
    float m_width;
    bool  m_isDragging;
};
