#include "entities/player/PlayerSpriteLoader.hpp"
#include <stdexcept>

namespace {
bool isCanvasBackgroundColor(const sf::Color& color) {
    // Check for canvas background colors (20,20,20) and (26,27,38)
    bool isDarkBg1 = (color.r >= 15 && color.r <= 25 && color.g >= 15 && color.g <= 25 && color.b >= 15 && color.b <= 25);
    bool isDarkBg2 = (color.r >= 20 && color.r <= 32 && color.g >= 22 && color.g <= 34 && color.b >= 32 && color.b <= 44);
    return isDarkBg1 || isDarkBg2;
}

sf::Image loadOverlayImage(const std::string& primaryPath, const std::string& fallbackPath) {
    sf::Image image;
    if (!image.loadFromFile(primaryPath)) {
        if (!image.loadFromFile(fallbackPath)) {
            throw std::runtime_error("Failed to load overlay image: " + primaryPath + " or " + fallbackPath);
        }
    }
    return image;
}
} // namespace

void PlayerSpriteLoader::processCanvasTransparency(sf::Image& image) {
    sf::Vector2u size = image.getSize();
    for (unsigned int y = 0; y < size.y; ++y) {
        for (unsigned int x = 0; x < size.x; ++x) {
            sf::Color px = image.getPixel(x, y);
            if (isCanvasBackgroundColor(px)) {
                image.setPixel(x, y, sf::Color(0, 0, 0, 0));
            }
        }
    }
}

sf::Image PlayerSpriteLoader::createFireRecolor(const sf::Image& source, CharacterType characterType) {
    sf::Image recolored = source;
    sf::Vector2u size = recolored.getSize();

    for (unsigned int y = 0; y < size.y; ++y) {
        for (unsigned int x = 0; x < size.x; ++x) {
            sf::Color px = recolored.getPixel(x, y);
            if (px.a < 10) continue;

            if (characterType == CharacterType::Mario) {
                // Mario Red shirt/cap (R > 150, G < 60, B < 60) -> Fire White (252, 252, 252)
                if (px.r > 150 && px.g < 60 && px.b < 60) {
                    recolored.setPixel(x, y, sf::Color(252, 252, 252, px.a));
                }
                // Mario Blue overalls (R < 60, G < 100, B > 140) -> Fire Red (210, 23, 28)
                else if (px.r < 60 && px.g < 100 && px.b > 140) {
                    recolored.setPixel(x, y, sf::Color(210, 23, 28, px.a));
                }
            } else {
                // Luigi Green shirt/cap (R < 60, G > 120, B < 80) -> Fire White (252, 252, 252)
                if (px.r < 60 && px.g > 120 && px.b < 80) {
                    recolored.setPixel(x, y, sf::Color(252, 252, 252, px.a));
                }
                // Luigi Blue overalls (R < 60, G < 100, B > 140) -> Fire Red (210, 23, 28)
                else if (px.r < 60 && px.g < 100 && px.b > 140) {
                    recolored.setPixel(x, y, sf::Color(210, 23, 28, px.a));
                }
            }
        }
    }
    return recolored;
}

sf::Image PlayerSpriteLoader::loadCompositedSpriteSheet(CharacterType characterType) {
    sf::Image mainSheet;
    std::string mainPath = (characterType == CharacterType::Mario) 
        ? "assets/sprites/player/mario.png" 
        : "assets/sprites/player/luigi.png";

    if (!mainSheet.loadFromFile(mainPath)) {
        if (!mainSheet.loadFromFile("assets/sprites/player/mario.png")) {
            throw std::runtime_error("Failed to load base player sprite sheet.");
        }
    }
    mainSheet.createMaskFromColor(sf::Color::White);

    std::string standPath = (characterType == CharacterType::Mario) ? "HoldStand.png" : "HoldStand_Luigi.png";
    std::string walkPath  = (characterType == CharacterType::Mario) ? "HoldWalk.png"  : "HoldWalk_Luigi.png";

    sf::Image holdStand = loadOverlayImage(standPath, "HoldStand.png");
    sf::Image holdWalk  = loadOverlayImage(walkPath, "HoldWalk.png");

    processCanvasTransparency(holdStand);
    processCanvasTransparency(holdWalk);

    sf::Image fireHoldStand = createFireRecolor(holdStand, characterType);
    sf::Image fireHoldWalk  = createFireRecolor(holdWalk, characterType);

    // Composite normal hold animations onto main sheet (destY: 295 and 365, destX: 0)
    mainSheet.copy(holdStand, 0, 295, sf::IntRect(0, 0, 0, 0), true);
    mainSheet.copy(holdWalk, 0, 365, sf::IntRect(0, 0, 0, 0), true);

    // Composite fire palette hold animations at offset X=736
    mainSheet.copy(fireHoldStand, 736, 295, sf::IntRect(0, 0, 0, 0), true);
    mainSheet.copy(fireHoldWalk, 736, 365, sf::IntRect(0, 0, 0, 0), true);

    return mainSheet;
}
