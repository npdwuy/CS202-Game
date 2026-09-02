#include "GenerateLevelState.hpp"
#include "GameManager.hpp"
#include "LevelSelectState.hpp"
#include "PlayState.hpp"
#include "resources/ResourceManager.hpp"
#include "levels/MapGenerator.hpp"
#include "commands/MenuCommands.hpp"
#include <iostream>

GenerateLevelState::GenerateLevelState() {
    m_font = ResourceManager::getInstance().getFont(
        "assets/fonts/ro-spritendo-font/RoSpritendoSemiboldBeta-vmVwZ.otf"
    );

    m_bgMap.load("levels/demo.txt");

    const sf::View& gameView = GameManager::getInstance().getGameView();
    const sf::FloatRect world = m_bgMap.worldBounds();

    m_bgCamera = gameView;
    float camCenterY = world.height - gameView.getSize().y / 2.f;
    m_bgCamera.setCenter(gameView.getSize().x / 2.f, camCenterY);

    m_titleText.setFont(m_font);
    m_titleText.setString("GENERATE LEVEL\nSelect Difficulty");
    m_titleText.setCharacterSize(50);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setOutlineColor(sf::Color::Black);
    m_titleText.setOutlineThickness(3.f);
    sf::FloatRect b = m_titleText.getLocalBounds();
    m_titleText.setOrigin(b.width / 2.f, b.height / 2.f);
    m_titleText.setPosition(gameView.getSize().x / 2.f, 160.f);

    const sf::Vector2f btnSize(420.f, 100.f);
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
    const char* labels[3] = { "Easy", "Medium", "Hard" };

    for (int i = 0; i < 3; ++i) {
        auto btn = std::make_unique<Button>(labels[i], m_font,
                                            sf::Vector2f(cx, yPositions[i]),
                                            btnSize, 34);
        btn->setColors(normalColors[i], hoverColors[i], white);
        btn->setShapeCornerRadius(btnSize.y / 2.f);

        int diff = i + 1;
        btn->setCommand(std::make_unique<LambdaCommand>([this, diff]() {
            generateAndPlay(diff);
        }));

        m_diffButtons[i] = std::move(btn);
    }

    m_backButton = std::make_unique<Button>("BACK", m_font,
                                            sf::Vector2f(cx, 830.f),
                                            sf::Vector2f(240.f, 60.f), 26);
    m_backButton->setColors(sf::Color(80, 80, 80, 200), sf::Color(140, 140, 140, 255), white);
    m_backButton->setShapeCornerRadius(30.f);
    m_backButton->setCommand(std::make_unique<LambdaCommand>([]() {
        GameManager::getInstance().changeState(std::make_unique<LevelSelectState>());
    }));
}

void GenerateLevelState::generateAndPlay(int difficulty) {
    MapGenerator::generateMap(difficulty, "levels/level4.txt");
    GameManager::getInstance().getSettings().setSelectedLevel(4);
    GameManager::getInstance().changeState(std::make_unique<PlayState>(false));
}

void GenerateLevelState::Input(const sf::Event& event) {
    sf::RenderWindow& window = GameManager::getInstance().getWindow();
    const sf::View& view = GameManager::getInstance().getGameView();

    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mp = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y), view);
        for (auto& btn : m_diffButtons) btn->update(mp);
        m_backButton->update(mp);
    }
    if (event.type == sf::Event::MouseButtonReleased) {
        sf::Vector2f mp = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), view);
        for (auto& btn : m_diffButtons) if (btn->handleClick(event, mp)) return;
        if (m_backButton->handleClick(event, mp)) return;
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        GameManager::getInstance().changeState(std::make_unique<LevelSelectState>());
    }
}

void GenerateLevelState::Update(sf::Time timePerFrame) {
    m_scrollX += 40.f * timePerFrame.asSeconds();
    const sf::FloatRect world = m_bgMap.worldBounds();
    const float viewHW = m_bgCamera.getSize().x / 2.f;
    if (m_scrollX + viewHW > world.width) m_scrollX = 0.f;
    m_bgCamera.setCenter(m_scrollX + viewHW, m_bgCamera.getCenter().y);
}

void GenerateLevelState::Render(sf::RenderWindow& window) {
    const sf::View& screenView = GameManager::getInstance().getGameView();
    window.setView(m_bgCamera);
    m_bgMap.render(window);
    window.setView(screenView);
    sf::RectangleShape overlay(screenView.getSize());
    overlay.setOrigin(screenView.getSize() / 2.f);
    overlay.setPosition(screenView.getCenter());
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);
    window.draw(m_titleText);
    for (auto& btn : m_diffButtons) btn->render(window);
    m_backButton->render(window);
}
