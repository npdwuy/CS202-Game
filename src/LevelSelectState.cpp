#include "LevelSelectState.hpp"
#include "CharacterSelectState.hpp"
#include "GameManager.hpp"
#include "PlayState.hpp"
#include "SettingsManager.hpp"
#include "commands/MenuCommands.hpp"
#include "resources/ResourceManager.hpp"

#include <stdexcept>

// Level metadata shown on each button
static const struct {
    const char* name;
    const char* difficulty;
    const char* hint;
} kLevelInfo[3] = {
    { "LEVEL 1",  "Easy",   "Grass Hills"       },
    { "LEVEL 2",  "Medium", "Broken Bridge"      },
    { "LEVEL 3",  "Hard",   "Bowser's Castle"    },
};

LevelSelectState::LevelSelectState() {
    m_font = ResourceManager::getInstance().getFont(
        "assets/fonts/ro-spritendo-font/RoSpritendoSemiboldBeta-vmVwZ.otf"
    );

    // ── Scrolling background map ─────────────────────────────────────────────    // Load scrolling background map
    m_bgMap.load("levels/demo.txt");
    m_bgMap.setDrawSky(false);

    m_parallaxBg.load("assets/backgrounds/level_bg.png");

    const sf::View& gameView = GameManager::getInstance().getGameView();
    const sf::FloatRect world = m_bgMap.worldBounds();

    m_bgCamera = gameView;
    float camCenterY = world.height - gameView.getSize().y / 2.f;
    m_bgCamera.setCenter(gameView.getSize().x / 2.f, camCenterY);

    // ── Title ────────────────────────────────────────────────────────────────
    m_titleText.setFont(m_font);
    m_titleText.setString("SELECT LEVEL");
    m_titleText.setCharacterSize(70);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setOutlineColor(sf::Color::Black);
    m_titleText.setOutlineThickness(3.f);
    {
        sf::FloatRect b = m_titleText.getLocalBounds();
        m_titleText.setOrigin(b.width / 2.f, b.height / 2.f);
        m_titleText.setPosition(gameView.getSize().x / 2.f, 160.f);
    }

    // ── 3 Level buttons ──────────────────────────────────────────────────────
    const sf::Vector2f btnSize(420.f, 100.f);
    const float cornerR = btnSize.y / 2.f;
    const float cx = gameView.getSize().x / 2.f;

    const sf::Color normalBlue  (40,  120, 210, 215);
    const sf::Color hoverBlue   (80,  180, 255, 255);
    const sf::Color normalGreen (40,  170,  80, 215);
    const sf::Color hoverGreen  (60,  220, 100, 255);
    const sf::Color normalRed   (200,  55,  55, 215);
    const sf::Color hoverRed    (255,  80,  80, 255);
    const sf::Color white       (255, 255, 255);

    const sf::Color normalColors[3] = { normalBlue, normalGreen, normalRed };
    const sf::Color hoverColors [3] = { hoverBlue,  hoverGreen,  hoverRed  };
    const float yPositions [3] = { 360.f, 520.f, 680.f };

    for (int i = 0; i < 3; ++i) {
        // Button label: "LEVEL N  -  Subtitle"
        std::string label = std::string(kLevelInfo[i].name)
                          + "  -  "
                          + kLevelInfo[i].hint;
        auto btn = std::make_unique<Button>(label, m_font,
                                            sf::Vector2f(cx, yPositions[i]),
                                            btnSize, 34);
        btn->setColors(normalColors[i], hoverColors[i], white);
        btn->setShapeCornerRadius(cornerR);

        // Capture by value
        int level = i + 1;
        btn->setCommand(std::make_unique<LambdaCommand>([level]() {
            GameManager::getInstance().getSettings().setSelectedLevel(level);
            GameManager::getInstance().changeState(
                std::make_unique<PlayState>(false)
            );
        }));

        m_levelButtons[i] = std::move(btn);

        // Difficulty sub-label
        m_difficultyText[i].setFont(m_font);
        m_difficultyText[i].setString(
            std::string("Difficulty: ") + kLevelInfo[i].difficulty
        );
        m_difficultyText[i].setCharacterSize(22);
        m_difficultyText[i].setFillColor(sf::Color(255, 255, 200));
        {
            sf::FloatRect b = m_difficultyText[i].getLocalBounds();
            m_difficultyText[i].setOrigin(b.width / 2.f, 0.f);
            m_difficultyText[i].setPosition(cx, yPositions[i] + btnSize.y / 2.f + 8.f);
        }
    }

    // ── Back button ──────────────────────────────────────────────────────────
    m_backButton = std::make_unique<Button>("BACK", m_font,
                                            sf::Vector2f(cx, 830.f),
                                            sf::Vector2f(240.f, 60.f), 26);
    m_backButton->setColors(sf::Color(80, 80, 80, 200),
                            sf::Color(140, 140, 140, 255), white);
    m_backButton->setShapeCornerRadius(30.f);
    m_backButton->setCommand(std::make_unique<LambdaCommand>([]() {
        GameManager::getInstance().changeState(
            std::make_unique<CharacterSelectState>()
        );
    }));
}

void LevelSelectState::selectLevel(int level) {
    GameManager::getInstance().getSettings().setSelectedLevel(level);
    GameManager::getInstance().changeState(std::make_unique<PlayState>(false));
}

void LevelSelectState::Input(const sf::Event& event) {
    sf::RenderWindow& window = GameManager::getInstance().getWindow();
    const sf::View& view = GameManager::getInstance().getGameView();

    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mp = window.mapPixelToCoords(
            sf::Vector2i(event.mouseMove.x, event.mouseMove.y), view
        );
        for (auto& btn : m_levelButtons) btn->update(mp);
        m_backButton->update(mp);
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        sf::Vector2f mp = window.mapPixelToCoords(
            sf::Vector2i(event.mouseButton.x, event.mouseButton.y), view
        );
        for (auto& btn : m_levelButtons)
            if (btn->handleClick(event, mp)) return;
        if (m_backButton->handleClick(event, mp)) return;
    }

    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::Num1: selectLevel(1); break;
            case sf::Keyboard::Num2: selectLevel(2); break;
            case sf::Keyboard::Num3: selectLevel(3); break;
            case sf::Keyboard::Escape:
                GameManager::getInstance().changeState(
                    std::make_unique<CharacterSelectState>()
                );
                break;
            default: break;
        }
    }
}

void LevelSelectState::Update(sf::Time timePerFrame) {
    m_scrollX += ScrollSpeed * timePerFrame.asSeconds();

    const sf::FloatRect world = m_bgMap.worldBounds();
    const float viewHW = m_bgCamera.getSize().x / 2.f;
    if (m_scrollX + viewHW > world.width) m_scrollX = 0.f;
    m_bgCamera.setCenter(m_scrollX + viewHW, m_bgCamera.getCenter().y);
    m_parallaxBg.update(m_bgCamera, 0.5f);
}

void LevelSelectState::Render(sf::RenderWindow& window) {
    const sf::View& screenView = GameManager::getInstance().getGameView();

    // 1. Scrolling background
    window.setView(m_bgCamera);

    m_parallaxBg.render(window);

    m_bgMap.render(window);

    // 2. Switch to screen-space
    window.setView(screenView);

    // 3. Dark overlay
    sf::RectangleShape overlay(screenView.getSize());
    overlay.setOrigin(screenView.getSize() / 2.f);
    overlay.setPosition(screenView.getCenter());
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    // 4. Title
    window.draw(m_titleText);

    // 5. Level buttons + difficulty labels
    for (int i = 0; i < 3; ++i) {
        m_levelButtons[i]->render(window);
        window.draw(m_difficultyText[i]);
    }

    // 6. Back button
    m_backButton->render(window);
}
