#include "PauseState.hpp"
#include "GameManager.hpp"
#include "MenuState.hpp"
#include "OptionsState.hpp"
#include "commands/MenuCommands.hpp"
#include <iostream>
#include <stdexcept>

PauseState::PauseState() {
  if (!m_buttonTexture.loadFromFile(
          "assets/sprites/button/btn_transparent.png")) {
    throw std::runtime_error(
        "Failed to load assets/sprites/button/btn_transparent.png");
  }

  if (!m_font.loadFromFile(
          "assets/fonts/ro-spritendo-font/RoSpritendoSemiboldBeta-vmVwZ.otf")) {
    throw std::runtime_error(
        "Failed to load "
        "assets/fonts/ro-spritendo-font/RoSpritendoSemiboldBeta-vmVwZ.otf");
  }

  initUI();
}

void PauseState::initUI() {
  // Dim background overlay
  m_dimOverlay.setSize(sf::Vector2f(1920.f, 1080.f));
  m_dimOverlay.setFillColor(sf::Color(0, 0, 0, 150));

  // Centered panel background
  m_panelBackground.setSize(sf::Vector2f(450.f, 550.f));
  m_panelBackground.setOrigin(225.f, 275.f);
  m_panelBackground.setPosition(960.f, 540.f);
  m_panelBackground.setFillColor(sf::Color(30, 30, 30, 240));
  m_panelBackground.setOutlineColor(sf::Color(100, 100, 100));
  m_panelBackground.setOutlineThickness(4.f);

  // Title text
  m_titleText.setFont(m_font);
  m_titleText.setString("PAUSED");
  m_titleText.setCharacterSize(42);
  m_titleText.setFillColor(sf::Color(255, 180, 0));
  sf::FloatRect titleBounds = m_titleText.getLocalBounds();
  m_titleText.setOrigin(titleBounds.left + titleBounds.width / 2.f,
                        titleBounds.top + titleBounds.height / 2.f);
  m_titleText.setPosition(960.f, 320.f);

  // Buttons
  sf::Vector2f btnSize(300.f, 60.f);
  m_resumeButton =
      std::make_unique<Button>("RESUME", m_font, m_buttonTexture,
                               sf::Vector2f(960.f, 410.f), btnSize, 24);
  m_optionsButton =
      std::make_unique<Button>("OPTIONS", m_font, m_buttonTexture,
                               sf::Vector2f(960.f, 500.f), btnSize, 24);
  m_menuButton =
      std::make_unique<Button>("MAIN MENU", m_font, m_buttonTexture,
                               sf::Vector2f(960.f, 590.f), btnSize, 24);
  m_exitButton =
      std::make_unique<Button>("EXIT GAME", m_font, m_buttonTexture,
                               sf::Vector2f(960.f, 680.f), btnSize, 24);

  m_resumeButton->setColors(sf::Color::White, sf::Color(255, 230, 200, 255),
                            sf::Color(245, 222, 179));
  m_optionsButton->setColors(sf::Color::White, sf::Color(255, 230, 200, 255),
                             sf::Color(245, 222, 179));
  m_menuButton->setColors(sf::Color::White, sf::Color(255, 230, 200, 255),
                          sf::Color(245, 222, 179));
  m_exitButton->setColors(sf::Color::White, sf::Color(255, 210, 210, 255),
                          sf::Color(245, 222, 179));

  // Callbacks
  m_resumeButton->setCommand(std::make_unique<PopStateCommand>());

  m_optionsButton->setCommand(std::make_unique<LambdaCommand>([]() {
    GameManager::getInstance().pushState(std::make_unique<OptionsState>());
  }));

  m_menuButton->setCommand(std::make_unique<LambdaCommand>([]() {
    GameManager::getInstance().changeState(std::make_unique<MenuState>());
  }));

  m_exitButton->setCommand(std::make_unique<ExitGameCommand>());
}

void PauseState::Input(const sf::Event &event) {
  sf::RenderWindow &window = GameManager::getInstance().getWindow();

  if (event.type == sf::Event::MouseMoved) {
    sf::Vector2f mousePos = window.mapPixelToCoords(
        sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
    m_resumeButton->update(mousePos);
    m_optionsButton->update(mousePos);
    m_menuButton->update(mousePos);
    m_exitButton->update(mousePos);
  }

  if (event.type == sf::Event::MouseButtonReleased) {
    sf::Vector2f mousePos = window.mapPixelToCoords(
        sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
    if (m_resumeButton->handleClick(event, mousePos))
      return;
    if (m_optionsButton->handleClick(event, mousePos))
      return;
    if (m_menuButton->handleClick(event, mousePos))
      return;
    if (m_exitButton->handleClick(event, mousePos))
      return;
  }

  if (event.type == sf::Event::KeyPressed &&
      event.key.code == sf::Keyboard::Escape) {
    GameManager::getInstance().popState();
  }
}

void PauseState::Update(sf::Time timePerFrame) {}

void PauseState::Render(sf::RenderWindow &window) {
  window.draw(m_dimOverlay);
  window.draw(m_panelBackground);
  window.draw(m_titleText);

  m_resumeButton->render(window);
  m_optionsButton->render(window);
  m_menuButton->render(window);
  m_exitButton->render(window);
}
