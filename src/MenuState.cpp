#include "MenuState.hpp"
#include "CharacterSelectState.hpp"
#include "GameManager.hpp"
#include "OptionsState.hpp"
#include "PlayState.hpp"
#include "commands/MenuCommands.hpp"
#include "resources/ResourceManager.hpp"
#include "persistence/SaveManager.hpp"

#include <stdexcept>

MenuState::MenuState() {
    m_font = ResourceManager::getInstance().getFont(
        "assets/fonts/ro-spritendo-font/RoSpritendoSemiboldBeta-vmVwZ.otf"
    );

    // ── Load scrolling map background ────────────────────────────────────────
    m_bgMap.load("levels/demo.txt");
    m_bgMap.setDrawSky(false);
    
    m_parallaxBg.load("assets/backgrounds/level_bg.png");

    const sf::View& gameView = GameManager::getInstance().getGameView();
    const sf::FloatRect world = m_bgMap.worldBounds();

    m_bgCamera = gameView;
    // Position camera at the bottom of the map so the ground / platforms are visible
    float camCenterY = world.height - gameView.getSize().y / 2.f;
    m_bgCamera.setCenter(gameView.getSize().x / 2.f, camCenterY);

    // ── Button layout: pill-shaped (cornerRadius = height/2) ────────────────
    // 5 buttons stacked vertically, centred at x=960 (half of 1920)
    const sf::Vector2f btnSize(340.f, 66.f);
    const float cornerR = btnSize.y / 2.f;   // fully rounded ends
    const float cx      = gameView.getSize().x / 2.f;

    const sf::Color normalOrange (255, 155, 40,  215);
    const sf::Color hoverYellow  (255, 215, 0,   255);
    const sf::Color normalRed    (210, 55,  55,  215);
    const sf::Color hoverRed     (255, 80,  80,  255);
    const sf::Color white        (255, 255, 255);

    auto makeBtn = [&](const std::string& label, float cy,
                       sf::Color norm, sf::Color hov) {
        auto btn = std::make_unique<Button>(label, m_font,
                                            sf::Vector2f(cx, cy), btnSize, 26);
        btn->setColors(norm, hov, white);
        btn->setShapeCornerRadius(cornerR);
        return btn;
    };

    // Vertical spacing: 5 buttons centred in lower portion of screen
    m_playButton    = makeBtn("PLAY",     520.f, normalOrange, hoverYellow);
    m_newGameButton = makeBtn("NEW GAME", 610.f, normalOrange, hoverYellow);
    m_loadButton    = makeBtn("LOAD",     700.f, normalOrange, hoverYellow);
    m_optionsButton = makeBtn("OPTIONS",  790.f, normalOrange, hoverYellow);
    m_exitButton    = makeBtn("EXIT",     880.f, normalRed,    hoverRed   );

    // ── Commands ─────────────────────────────────────────────────────────────
    m_playButton->setCommand(std::make_unique<LambdaCommand>([]() {
        GameManager::getInstance().changeState(
            std::make_unique<CharacterSelectState>()
        );
    }));

    m_newGameButton->setCommand(std::make_unique<LambdaCommand>([]() {
        SaveData defaultData;
        defaultData.version = 1;
        defaultData.currentLevel = 1;
        defaultData.highestUnlockedLevel = 1;
        defaultData.score = 0;
        defaultData.remainingLives = 3;
        defaultData.coins = 0;
        defaultData.remainingTime = 400.f;
        defaultData.powerUpState = "None";
        defaultData.selectedCharacter = "Mario";
        defaultData.playerX = 100.f;
        defaultData.playerY = 700.f;
        SaveManager::save(defaultData);

        GameManager::getInstance().changeState(
            std::make_unique<CharacterSelectState>()
        );
    }));

    m_loadButton->setCommand(std::make_unique<LambdaCommand>([]() {
        GameManager::getInstance().changeState(
            std::make_unique<PlayState>(true)   // true = load saved game
        );
    }));

    m_optionsButton->setCommand(std::make_unique<LambdaCommand>([]() {
        GameManager::getInstance().pushState(std::make_unique<OptionsState>());
    }));

    m_exitButton->setCommand(std::make_unique<ExitGameCommand>());
}

void MenuState::Input(const sf::Event& event) {
    sf::RenderWindow& window = GameManager::getInstance().getWindow();

    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mp = window.mapPixelToCoords(
            sf::Vector2i(event.mouseMove.x, event.mouseMove.y),
            GameManager::getInstance().getGameView()
        );
        m_playButton   ->update(mp);
        m_newGameButton->update(mp);
        m_loadButton   ->update(mp);
        m_optionsButton->update(mp);
        m_exitButton   ->update(mp);
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        sf::Vector2f mp = window.mapPixelToCoords(
            sf::Vector2i(event.mouseButton.x, event.mouseButton.y),
            GameManager::getInstance().getGameView()
        );
        if (m_playButton   ->handleClick(event, mp)) return;
        if (m_newGameButton->handleClick(event, mp)) return;
        if (m_loadButton   ->handleClick(event, mp)) return;
        if (m_optionsButton->handleClick(event, mp)) return;
        if (m_exitButton   ->handleClick(event, mp)) return;
    }

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Return) {
            // Default Enter → go to character select
            GameManager::getInstance().changeState(
                std::make_unique<CharacterSelectState>()
            );
        } else if (event.key.code == sf::Keyboard::Escape) {
            GameManager::getInstance().quit();
        }
    }
}

void MenuState::Update(sf::Time timePerFrame) {
    // Scroll the background map to the right; wrap at map edge.
    m_scrollX += ScrollSpeed * timePerFrame.asSeconds();

    const sf::FloatRect world   = m_bgMap.worldBounds();
    const float          viewHW  = m_bgCamera.getSize().x / 2.f;

    if (m_scrollX + viewHW > world.width) {
        m_scrollX = 0.f;   // seamless wrap back to start
    }

    m_bgCamera.setCenter(m_scrollX + viewHW, m_bgCamera.getCenter().y);
    m_parallaxBg.update(m_bgCamera, 0.5f);
}

void MenuState::Render(sf::RenderWindow& window) {
    const sf::View& screenView = GameManager::getInstance().getGameView();

    // 1. Draw scrolling map and background with the scrolling camera
    window.setView(m_bgCamera);
    
    m_parallaxBg.render(window);
    
    m_bgMap.render(window);

    // 2. Switch to the fixed screen view for all UI
    window.setView(screenView);

    // 3. Semi-transparent dark overlay so buttons are readable over the map
    sf::RectangleShape overlay(screenView.getSize());
    overlay.setOrigin(screenView.getSize() / 2.f);
    overlay.setPosition(screenView.getCenter());
    overlay.setFillColor(sf::Color(0, 0, 0, 130));
    window.draw(overlay);

    // 4. Draw the 5 buttons
    m_playButton   ->render(window);
    m_newGameButton->render(window);
    m_loadButton   ->render(window);
    m_optionsButton->render(window);
    m_exitButton   ->render(window);
}
