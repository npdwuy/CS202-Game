#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

#include <algorithm>

namespace camera_math {

inline float clampAxis(float value, float lower, float upper) {
    if (lower > upper) {
        return (lower + upper) * 0.5f;
    }
    return std::clamp(value, lower, upper);
}

inline sf::Vector2f clampCenter(
    sf::Vector2f center,
    sf::Vector2f viewSize,
    const sf::FloatRect& worldBounds
) {
    const sf::Vector2f halfView = viewSize * 0.5f;
    const float right = worldBounds.left + worldBounds.width;
    const float bottom = worldBounds.top + worldBounds.height;
    center.x = clampAxis(
        center.x,
        worldBounds.left + halfView.x,
        right - halfView.x
    );
    center.y = clampAxis(
        center.y,
        worldBounds.top + halfView.y,
        bottom - halfView.y
    );
    return center;
}

inline sf::FloatRect visibleBounds(
    sf::Vector2f center,
    sf::Vector2f viewSize,
    float padding = 0.f
) {
    const float safePadding = std::max(0.f, padding);
    return {
        center.x - viewSize.x * 0.5f - safePadding,
        center.y - viewSize.y * 0.5f - safePadding,
        viewSize.x + safePadding * 2.f,
        viewSize.y + safePadding * 2.f
    };
}

}
