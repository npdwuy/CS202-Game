#include "entities/player/PlayerSpriteLoader.hpp"
#include <iostream>
#include <vector>

namespace {
bool isCanvasBackgroundColor(const sf::Color& color) {
    // Check for canvas background colors (20,20,20) and (26,27,38)
    bool isDarkBg1 = (color.r >= 15 && color.r <= 25 && color.g >= 15 && color.g <= 25 && color.b >= 15 && color.b <= 25);
    bool isDarkBg2 = (color.r >= 20 && color.r <= 32 && color.g >= 22 && color.g <= 34 && color.b >= 32 && color.b <= 44);
    return isDarkBg1 || isDarkBg2;
}

sf::Image loadOverlayImage(const std::string& filename, const std::string& fallbackFilename) {
    sf::Image image;
    const std::vector<std::string> candidatePaths = {
        "assets/sprites/player/" + filename,
        filename,
        "../" + filename,
        "assets/sprites/player/" + fallbackFilename,
        fallbackFilename,
        "../" + fallbackFilename
    };

    for (const auto& path : candidatePaths) {
        if (image.loadFromFile(path)) {
            return image;
        }
    }

    std::cerr << "[PlayerSpriteLoader Warning] Could not locate overlay asset: " << filename << std::endl;
    return sf::Image{};
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
    const std::vector<std::string> mainSheetPaths = (characterType == CharacterType::Mario)
        ? std::vector<std::string>{"assets/sprites/player/mario.png", "mario.png"}
        : std::vector<std::string>{"assets/sprites/player/luigi.png", "luigi.png", "assets/sprites/player/mario.png"};

    bool loaded = false;
    for (const auto& path : mainSheetPaths) {
        if (mainSheet.loadFromFile(path)) {
            loaded = true;
            break;
        }
    }
    if (!loaded) {
        std::cerr << "[PlayerSpriteLoader Error] Base player sprite sheet could not be loaded." << std::endl;
        // Create fallback colored image to avoid crash
        mainSheet.create(1471, 720, sf::Color::Transparent);
    } else {
        mainSheet.createMaskFromColor(sf::Color::White);
    }

    std::string standFilename = (characterType == CharacterType::Mario) ? "HoldStand.png" : "HoldStand_Luigi.png";
    std::string walkFilename  = (characterType == CharacterType::Mario) ? "HoldWalk.png"  : "HoldWalk_Luigi.png";

    sf::Image holdStand = loadOverlayImage(standFilename, "HoldStand.png");
    sf::Image holdWalk  = loadOverlayImage(walkFilename, "HoldWalk.png");

    if (holdStand.getSize().x > 0 && holdStand.getSize().y > 0) {
        processCanvasTransparency(holdStand);
        sf::Image fireHoldStand = createFireRecolor(holdStand, characterType);
        mainSheet.copy(holdStand, 0, 295, sf::IntRect(0, 0, 0, 0), true);
        mainSheet.copy(fireHoldStand, 736, 295, sf::IntRect(0, 0, 0, 0), true);
    }

    if (holdWalk.getSize().x > 0 && holdWalk.getSize().y > 0) {
        processCanvasTransparency(holdWalk);
        sf::Image fireHoldWalk = createFireRecolor(holdWalk, characterType);
        mainSheet.copy(holdWalk, 0, 365, sf::IntRect(0, 0, 0, 0), true);
        mainSheet.copy(fireHoldWalk, 736, 365, sf::IntRect(0, 0, 0, 0), true);
    }

    return mainSheet;
}
