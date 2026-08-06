#include "camera/CameraMath.hpp"

#include <cmath>
#include <iostream>
#include <string>

namespace {

bool require(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
    }
    return condition;
}

bool nearlyEqual(float left, float right, float tolerance = 0.02f) {
    return std::abs(left - right) <= tolerance;
}

}

int main() {
    bool passed = true;
    const sf::FloatRect worldBounds(0.f, 0.f, 1920.f, 960.f);
    const sf::Vector2f cameraSize(1382.4f, 777.6f);

    const sf::Vector2f leftBottom = camera_math::clampCenter(
        {100.f, 800.f},
        cameraSize,
        worldBounds
    );
    passed &= require(
        nearlyEqual(leftBottom.x, cameraSize.x * 0.5f),
        "camera clamps to the left world edge"
    );
    passed &= require(
        nearlyEqual(
            leftBottom.y,
            worldBounds.height - cameraSize.y * 0.5f
        ),
        "camera clamps to the bottom world edge"
    );

    const sf::Vector2f rightTop = camera_math::clampCenter(
        {1850.f, 10.f},
        cameraSize,
        worldBounds
    );
    const float maximumCenterX = worldBounds.width - cameraSize.x * 0.5f;
    passed &= require(
        nearlyEqual(rightTop.x, maximumCenterX) &&
            nearlyEqual(rightTop.y, cameraSize.y * 0.5f),
        "camera clamps to the top-right world edges"
    );

    const sf::FloatRect visible = camera_math::visibleBounds(
        rightTop,
        cameraSize,
        48.f
    );
    passed &= require(
        nearlyEqual(visible.width, cameraSize.x + 96.f) &&
            nearlyEqual(visible.height, cameraSize.y + 96.f),
        "visible bounds include caller-provided culling padding"
    );

    const sf::Vector2f smallWorldCenter = camera_math::clampCenter(
        {0.f, 0.f},
        {1200.f, 800.f},
        {20.f, 30.f, 600.f, 400.f}
    );
    passed &= require(
        nearlyEqual(smallWorldCenter.x, 320.f) &&
            nearlyEqual(smallWorldCenter.y, 230.f),
        "oversized views center themselves on small worlds"
    );

    if (passed) {
        std::cout << "All player camera tests passed.\n";
        return 0;
    }
    return 1;
}
