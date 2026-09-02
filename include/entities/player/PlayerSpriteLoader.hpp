#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class PlayerSpriteLoader {
public:
    enum class CharacterType {
        Mario,
        Luigi
    };

    static sf::Image loadCompositedSpriteSheet(CharacterType characterType);

private:
    static void processCanvasTransparency(sf::Image& image);
    static sf::Image createFireRecolor(const sf::Image& source, CharacterType characterType);
};
