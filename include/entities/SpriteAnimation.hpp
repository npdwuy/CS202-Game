#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

struct SpriteAnimation {
    static void Advance(
        const std::vector<sf::IntRect>& frames,
        int& currentFrame,
        float& timer,
        float frameDuration,
        bool loop,
        bool& reachedLast)
    {
        if (timer >= frameDuration) {
            timer -= frameDuration;
            ++currentFrame;
            if (static_cast<std::size_t>(currentFrame) >= frames.size()) {
                if (loop) {
                    currentFrame = 0;
                } else {
                    currentFrame = static_cast<int>(frames.size()) - 1;
                    reachedLast = true;
                }
            }
        }
    }
};
