#pragma once

#include <SFML/Graphics.hpp>

class PlayerCamera {
public:
    PlayerCamera();

    void reset(
        sf::Vector2f focusPosition,
        const sf::FloatRect& worldBounds,
        const sf::View& screenView
    );
    void update(
        sf::Vector2f focusPosition,
        sf::Vector2f focusVelocity,
        const sf::FloatRect& worldBounds,
        const sf::View& screenView,
        sf::Time deltaTime
    );

    const sf::View& view() const;
    sf::FloatRect visibleBounds(float padding = 0.f) const;

private:
    void updateViewSize(const sf::View& screenView);

    sf::View m_view;
    sf::Vector2f m_center{};
    bool m_initialized = false;
    float m_maxCenterX = 0.f;
};
