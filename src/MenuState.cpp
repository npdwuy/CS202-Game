#include "MenuState.hpp"
#include "CharacterSelectState.hpp"
#include "GameManager.hpp"
#include "OptionsState.hpp"
#include "PlayState.hpp"
#include "commands/MenuCommands.hpp"
#include "resources/ResourceManager.hpp"

#include <stdexcept>

MenuState::MenuState() {
    m_font = ResourceManager::getInstance().getFont(
        "assets/fonts/ro-spritendo-font/RoSpritendoSemiboldBeta-vmVwZ.otf"
    );

    // ── Load scrolling map background ────────────────────────────────────────
    m_bgMap.load("levels/demo.txt");
    
    m_bgTexture = std::make_shared<sf::Texture>();
    if (m_bgTexture->loadFromFile("assets/backgrounds/level_bg.png")) {
        m_bgTexture->setRepeated(true);
        m_bgSprite.setTexture(*m_bgTexture);
        // Cover a wide area so it can scroll seamlessly
        m_bgSprite.setTextureRect(sf::IntRect(0, 0, 100000, m_bgTexture->getSize().y));
        float scaleY = 1080.f / static_cast<float>(m_bgTexture->getSize().y);
        m_bgSprite.setScale(scaleY, scaleY);
        // Center the sprite vertically
        m_bgSprite.setPosition(0.f, (1080.f - m_bgTexture->getSize().y * scaleY) / 2.f);
    }

    const sf::View& gameView = GameManager::getInstance().getGameView();
    const sf::FloatRect world = m_bgMap.worldBounds();

    m_bgCamera = gameView;
    // Position camera at the bottom of the map so the ground / platforms are visible
    float camCenterY = world.height - gameView.getSize().y / 2.f;
    m_bgCamera.setCenter(gameView.getSize().x / 2.f, camCenterY);

    // ── Button layout: pill-shaped (cornerRadius = height/2) ────────────────
    // 4 buttons stacked vertically, centred at x=960 (half of 1920)
    const sf::Vector2f btnSize(340.f, 72.f);
    const float cornerR = btnSize.y / 2.f;   // = 36 → fully rounded ends
    const float cx      = gameView.getSize().x / 2.f;

    const sf::Color normalOrange (255, 155, 40,  215);
    const sf::Color hoverYellow  (255, 215, 0,   255);
    const sf::Color normalRed    (210, 55,  55,  215);
    const sf::Color hoverRed     (255, 80,  80,  255);
    const sf::Color white        (255, 255, 255);

    auto makeBtn = [&](const std::string& label, float cy,
                       sf::Color norm, sf::Color hov) {
        auto btn = std::make_unique<Button>(label, m_font,
                                            sf::Vector2f(cx, cy), btnSize, 30);
        btn->setColors(norm, hov, white);
        btn->setShapeCornerRadius(cornerR);
        return btn;
    };

    // Vertical spacing: buttons centred in the lower 55 % of a 1080 px screen
    m_playButton    = makeBtn("PLAY",    600.f, normalOrange, hoverYellow);
    m_loadButton    = makeBtn("LOAD",    700.f, normalOrange, hoverYellow);
    m_optionsButton = makeBtn("OPTIONS", 800.f, normalOrange, hoverYellow);
    m_exitButton    = makeBtn("EXIT",    900.f, normalRed,    hoverRed   );

    // ── Commands ─────────────────────────────────────────────────────────────
    m_playButton->setCommand(std::make_unique<LambdaCommand>([]() {
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
}

void MenuState::Render(sf::RenderWindow& window) {
    const sf::View& screenView = GameManager::getInstance().getGameView();

    // 1. Draw scrolling map and background with the scrolling camera
    window.setView(m_bgCamera);
    
    // The sprite needs to scroll too. We can draw it, but since it has a 
    // different parallax speed, we'll shift its texture rect. Wait, it's easier to just 
    // position it based on the camera.
    if (m_bgTexture) {
        // Simple parallax: move sprite position with camera but slower
        sf::Vector2f camCenter = m_bgCamera.getCenter();
        sf::Vector2f camSize = m_bgCamera.getSize();
        float leftEdge = camCenter.x - camSize.x / 2.f;
        
        m_bgSprite.setPosition(leftEdge * 0.5f, m_bgSprite.getPosition().y);
        window.draw(m_bgSprite);
    }
    
    m_bgMap.render(window);

    // 2. Switch to the fixed screen view for all UI
    window.setView(screenView);

    // 3. Semi-transparent dark overlay so buttons are readable over the map
    sf::RectangleShape overlay(screenView.getSize());
    overlay.setOrigin(screenView.getSize() / 2.f);
    overlay.setPosition(screenView.getCenter());
    overlay.setFillColor(sf::Color(0, 0, 0, 130));
    window.draw(overlay);

    // 4. Draw the 4 buttons
    m_playButton   ->render(window);
    m_loadButton   ->render(window);
    m_optionsButton->render(window);
    m_exitButton   ->render(window);
}
