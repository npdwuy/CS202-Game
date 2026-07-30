#include "AboutState.hpp"
#include "GameManager.hpp"
#include <iostream>
#include <stdexcept>

AboutState::AboutState() {
    if (!m_buttonTexture.loadFromFile("assets/sprites/button/btn_transparent.png")) {
        throw std::runtime_error("Failed to load assets/sprites/button/btn_transparent.png");
    }

    if (!m_font.loadFromFile("assets/fonts/ro-spritendo-font/RoSpritendoSemiboldBeta-vmVwZ.otf")) {
        throw std::runtime_error("Failed to load assets/fonts/ro-spritendo-font/RoSpritendoSemiboldBeta-vmVwZ.otf");
    }

    initUI();
}

void AboutState::initUI() {
    // Dim background overlay
    m_dimOverlay.setSize(sf::Vector2f(1920.f, 1080.f));
    m_dimOverlay.setFillColor(sf::Color(0, 0, 0, 150));

    // Centered panel background
    m_panelBackground.setSize(sf::Vector2f(700.f, 550.f));
    m_panelBackground.setOrigin(350.f, 275.f);
    m_panelBackground.setPosition(960.f, 540.f);
    m_panelBackground.setFillColor(sf::Color(30, 30, 30, 240));
    m_panelBackground.setOutlineColor(sf::Color(100, 100, 100));
    m_panelBackground.setOutlineThickness(4.f);

    // Title text
    m_titleText.setFont(m_font);
    m_titleText.setString("ABOUT");
    m_titleText.setCharacterSize(42);
    m_titleText.setFillColor(sf::Color(255, 180, 0));
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setOrigin(titleBounds.left + titleBounds.width / 2.f, titleBounds.top + titleBounds.height / 2.f);
    m_titleText.setPosition(960.f, 310.f);

    // Credits details
    std::vector<std::string> lines = {
        "CS202 Crossing Game",
        "Version 1.0",
        "",
        "Developed by: Group 5",
        "Course: CS202 - OOP"
    };

    float startY = 370.f;
    float stepY = 40.f;
    for (size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].empty()) continue;
        sf::Text creditText;
        creditText.setFont(m_font);
        creditText.setString(lines[i]);
        creditText.setCharacterSize(24);
        creditText.setFillColor(i < 2 ? sf::Color(255, 230, 200) : sf::Color::White);
        
        sf::FloatRect bounds = creditText.getLocalBounds();
        creditText.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        creditText.setPosition(960.f, startY + i * stepY);
        m_creditsTexts.push_back(creditText);
    }

    // Back Button
    m_backButton = std::make_unique<Button>(
        "BACK", m_font, m_buttonTexture, sf::Vector2f(960.f, 720.f), sf::Vector2f(260.f, 60.f), 24
    );
    m_backButton->setColors(sf::Color::White, sf::Color(255, 230, 200, 255), sf::Color(245, 222, 179));
    m_backButton->setCallback([]() {
        GameManager::getInstance().popState();
    });
}

void AboutState::Input(const sf::Event &event) {
    sf::RenderWindow &window = GameManager::getInstance().getWindow();
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

    if (event.type == sf::Event::MouseMoved) {
        m_backButton->update(mousePos);
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        if (m_backButton->handleClick(event, mousePos)) return;
    }

    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        GameManager::getInstance().popState();
    }
}

void AboutState::Update(sf::Time timePerFrame) {
}

void AboutState::Render(sf::RenderWindow &window) {
    window.draw(m_dimOverlay);
    window.draw(m_panelBackground);
    window.draw(m_titleText);

    for (const auto& creditText : m_creditsTexts) {
        window.draw(creditText);
    }

    m_backButton->render(window);
}
