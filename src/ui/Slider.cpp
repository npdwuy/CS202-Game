#include "ui/Slider.hpp"
#include <cmath>

Slider::Slider(sf::Vector2f position, float width, float minVal, float maxVal, float currentVal, const sf::Font& font)
    : m_minVal(minVal)
    , m_maxVal(maxVal)
    , m_value(currentVal)
    , m_width(width)
    , m_isDragging(false)
{
    // Track setup
    m_track.setSize(sf::Vector2f(width, 10.f));
    m_track.setOrigin(width / 2.f, 5.f);
    m_track.setPosition(position);
    m_track.setFillColor(sf::Color(80, 80, 80));

    // Fill setup
    m_fill.setSize(sf::Vector2f(0.f, 10.f));
    m_fill.setOrigin(0.f, 5.f);
    m_fill.setPosition(position.x - width / 2.f, position.y);
    m_fill.setFillColor(sf::Color(255, 180, 0)); // Nice gold fill

    // Handle setup
    m_handle.setRadius(12.f);
    m_handle.setOrigin(12.f, 12.f);
    m_handle.setFillColor(sf::Color::White);
    m_handle.setOutlineColor(sf::Color(180, 180, 180));
    m_handle.setOutlineThickness(2.f);

    // Value text setup
    m_valueText.setFont(font);
    m_valueText.setCharacterSize(22);
    m_valueText.setFillColor(sf::Color::White);

    updateVisuals();
}

void Slider::handleInput(const sf::Event& event, sf::Vector2f mousePos) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        // Expand click area slightly for track/handle
        sf::FloatRect clickArea = m_track.getGlobalBounds();
        clickArea.top -= 10.f;
        clickArea.height += 20.f;
        
        if (clickArea.contains(mousePos) || m_handle.getGlobalBounds().contains(mousePos)) {
            m_isDragging = true;
            update(mousePos);
        }
    } else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        m_isDragging = false;
    }
}

void Slider::update(sf::Vector2f mousePos) {
    if (m_isDragging) {
        float leftX = m_track.getPosition().x - m_width / 2.f;
        float relX = mousePos.x - leftX;
        
        if (relX < 0.f) relX = 0.f;
        if (relX > m_width) relX = m_width;
        
        float percentage = relX / m_width;
        m_value = m_minVal + percentage * (m_maxVal - m_minVal);
        
        updateVisuals();
    }
}

void Slider::render(sf::RenderWindow& window) const {
    window.draw(m_track);
    window.draw(m_fill);
    window.draw(m_handle);
    window.draw(m_valueText);
}

float Slider::getValue() const {
    return m_value;
}

void Slider::setValue(float val) {
    m_value = val;
    if (m_value < m_minVal) m_value = m_minVal;
    if (m_value > m_maxVal) m_value = m_maxVal;
    updateVisuals();
}

void Slider::updateVisuals() {
    float percentage = (m_value - m_minVal) / (m_maxVal - m_minVal);
    float fillWidth = percentage * m_width;
    
    m_fill.setSize(sf::Vector2f(fillWidth, 10.f));
    
    float leftX = m_track.getPosition().x - m_width / 2.f;
    m_handle.setPosition(leftX + fillWidth, m_track.getPosition().y);
    
    m_valueText.setString(std::to_string(static_cast<int>(std::round(m_value))) + "%");
    
    // Position text cleanly to the right
    m_valueText.setPosition(m_track.getPosition().x + m_width / 2.f + 25.f, m_track.getPosition().y - 15.f);
}
