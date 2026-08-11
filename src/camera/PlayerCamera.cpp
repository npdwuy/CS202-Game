#include "camera/PlayerCamera.hpp"
#include "camera/CameraMath.hpp"

#include <algorithm>
#include <cmath>

namespace {

constexpr float CameraZoom = 0.72f;
constexpr float HorizontalDeadZone = 105.f;
constexpr float VerticalDeadZone = 58.f;
constexpr float HorizontalLookAheadSeconds = 0.42f;
constexpr float MaximumLookAhead = 165.f;
constexpr float VerticalFocusOffset = 70.f;
constexpr float FollowSharpness = 7.5f;

float followOutsideDeadZone(float current, float focus, float deadZone) {
    const float difference = focus - current;
    if (difference > deadZone) {
        return focus - deadZone;
    }
    if (difference < -deadZone) {
        return focus + deadZone;
    }
    return current;
}

}

PlayerCamera::PlayerCamera() {
    m_view.setViewport({0.f, 0.f, 1.f, 1.f});
}

void PlayerCamera::reset(
    sf::Vector2f focusPosition,
    const sf::FloatRect& worldBounds,
    const sf::View& screenView
) {
    updateViewSize(screenView);
    focusPosition.y -= VerticalFocusOffset;
    m_center = camera_math::clampCenter(
        focusPosition,
        m_view.getSize(),
        worldBounds
    );
    m_maxCenterX = m_center.x;
    m_view.setCenter(m_center);
    m_initialized = true;
}

void PlayerCamera::update(
    sf::Vector2f focusPosition,
    sf::Vector2f focusVelocity,
    const sf::FloatRect& worldBounds,
    const sf::View& screenView,
    sf::Time deltaTime
) {
    updateViewSize(screenView);
    if (!m_initialized) {
        reset(focusPosition, worldBounds, screenView);
        return;
    }

    const float lookAhead = std::clamp(
        focusVelocity.x * HorizontalLookAheadSeconds,
        -MaximumLookAhead,
        MaximumLookAhead
    );
    const sf::Vector2f focusTarget{
        focusPosition.x + lookAhead,
        focusPosition.y - VerticalFocusOffset
    };

    sf::Vector2f target{
        followOutsideDeadZone(
            m_center.x,
            focusTarget.x,
            HorizontalDeadZone
        ),
        followOutsideDeadZone(
            m_center.y,
            focusTarget.y,
            VerticalDeadZone
        )
    };
    target = camera_math::clampCenter(
        target,
        m_view.getSize(),
        worldBounds
    );

    const float seconds = std::clamp(deltaTime.asSeconds(), 0.f, 0.1f);
    const float blend = 1.f - std::exp(-FollowSharpness * seconds);
    m_center += (target - m_center) * blend;
    m_center = camera_math::clampCenter(
        m_center,
        m_view.getSize(),
        worldBounds
    );
    
    // Prevent camera from scrolling backward (leftward)
    if (m_center.x < m_maxCenterX) {
        m_center.x = m_maxCenterX;
    } else {
        m_maxCenterX = m_center.x;
    }

    m_view.setCenter(m_center);
}

const sf::View& PlayerCamera::view() const {
    return m_view;
}

sf::FloatRect PlayerCamera::visibleBounds(float padding) const {
    return camera_math::visibleBounds(
        m_view.getCenter(),
        m_view.getSize(),
        padding
    );
}

void PlayerCamera::updateViewSize(const sf::View& screenView) {
    const sf::Vector2f desiredSize = screenView.getSize() * CameraZoom;
    if (m_view.getSize() != desiredSize) {
        m_view.setSize(desiredSize);
    }
}
