#include "MenuState.hpp"
#include "AboutState.hpp"
#include "GameManager.hpp"
#include "OptionsState.hpp"
#include "PlayState.hpp"
#include <iostream>
#include <stdexcept>

MenuState::MenuState() {
  if (!m_backgroundTexture.loadFromFile("assets/sprites/menu_bg.png")) {
    throw std::runtime_error("Failed to load assets/sprites/menu_bg.png");
  }
  m_backgroundSprite.setTexture(m_backgroundTexture);

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

  // Centered vertical layout
  sf::Vector2f btnSize(300.f, 120.f);
  m_playButton = std::make_unique<Button>(
      "PLAY", m_font, m_buttonTexture, sf::Vector2f(960.f, 450.f), btnSize, 36);
  m_aboutButton =
      std::make_unique<Button>("ABOUT", m_font, m_buttonTexture,
                               sf::Vector2f(960.f, 580.f), btnSize, 36);
  m_optionsButton =
      std::make_unique<Button>("OPTIONS", m_font, m_buttonTexture,
                               sf::Vector2f(960.f, 710.f), btnSize, 36);
  m_exitButton = std::make_unique<Button>(
      "EXIT", m_font, m_buttonTexture, sf::Vector2f(960.f, 840.f), btnSize, 36);

  // Set colors
  m_playButton->setColors(sf::Color::White, sf::Color(255, 230, 200, 255),
                          sf::Color(245, 222, 179));
  m_aboutButton->setColors(sf::Color::White, sf::Color(255, 230, 200, 255),
                           sf::Color(245, 222, 179));
  m_optionsButton->setColors(sf::Color::White, sf::Color(255, 230, 200, 255),
                             sf::Color(245, 222, 179));
  m_exitButton->setColors(sf::Color::White, sf::Color(255, 210, 210, 255),
                          sf::Color(245, 222, 179));

  // Button callbacks
  m_playButton->setCallback([]() {
    std::cout << "Transitioning to PlayState...\n";
    GameManager::getInstance().changeState(std::make_unique<PlayState>());
  });

  m_aboutButton->setCallback([]() {
    std::cout << "Opening AboutState...\n";
    GameManager::getInstance().pushState(std::make_unique<AboutState>());
  });

  m_optionsButton->setCallback([]() {
    std::cout << "Opening OptionsState...\n";
    GameManager::getInstance().pushState(std::make_unique<OptionsState>());
  });

  m_exitButton->setCallback([]() {
    std::cout << "Exiting game from Main Menu...\n";
    GameManager::getInstance().quit();
  });
}

void MenuState::Input(const sf::Event &event) {
  sf::RenderWindow &window = GameManager::getInstance().getWindow();

  if (event.type == sf::Event::MouseMoved) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
    m_playButton->update(mousePos);
    m_aboutButton->update(mousePos);
    m_optionsButton->update(mousePos);
    m_exitButton->update(mousePos);
  }

  if (event.type == sf::Event::MouseButtonReleased) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
    if (m_playButton->handleClick(event, mousePos))
      return;
    if (m_aboutButton->handleClick(event, mousePos))
      return;
    if (m_optionsButton->handleClick(event, mousePos))
      return;
    if (m_exitButton->handleClick(event, mousePos))
      return;
  }

  if (event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::Enter) {
      std::cout << "Transitioning to PlayState...\n";
      GameManager::getInstance().changeState(std::make_unique<PlayState>());
    } else if (event.key.code == sf::Keyboard::Escape) {
      std::cout << "Exiting game from Main Menu...\n";
      GameManager::getInstance().quit();
    }
  }
}

void MenuState::Update(sf::Time timePerFrame) {}

void MenuState::Render(sf::RenderWindow &window) {
  window.draw(m_backgroundSprite);

  m_playButton->render(window);
  m_aboutButton->render(window);
  m_optionsButton->render(window);
  m_exitButton->render(window);
}