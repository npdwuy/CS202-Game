#include "GameManager.hpp"
#include <iostream>

GameManager::GameManager() 
    : m_window(sf::VideoMode(1920, 1080), "CS202-Group5")
    , m_isRunning(true) 
{
    m_window.setFramerateLimit(60);
}

GameManager& GameManager::getInstance() {
    static GameManager instance;
    return instance;
}

void GameManager::pushState(std::unique_ptr<GameState> state) {
    if (!m_states.empty()) {
        m_states.back()->onPause();
    }
    m_states.push_back(std::move(state));
}

void GameManager::popState() {
    if (!m_states.empty()) {
        m_states.pop_back();
    }
    if (!m_states.empty()) {
        m_states.back()->onResume();
    }
}

void GameManager::changeState(std::unique_ptr<GameState> state) {
    m_states.clear();
    m_states.push_back(std::move(state));
}

void GameManager::run() {
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    const sf::Time timePerFrame = sf::seconds(1.f / 60.f);

    while (m_isRunning && m_window.isOpen()) {
        sf::Time elapsedTime = clock.restart();

        // Prevent spiral of death
        if (elapsedTime > sf::seconds(0.25f)) {
            elapsedTime = sf::seconds(0.25f);
        }

        timeSinceLastUpdate += elapsedTime;

        processInput();

        while (timeSinceLastUpdate >= timePerFrame) {
            timeSinceLastUpdate -= timePerFrame;
            update(timePerFrame);
        }

        render();
    }
}

void GameManager::quit() {
    m_isRunning = false;
    m_window.close();
}

sf::RenderWindow& GameManager::getWindow() {
    return m_window;
}

SettingsManager& GameManager::getSettings() {
    return m_settings;
}

void GameManager::processInput() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            quit();
            return;
        }
        if (!m_states.empty()) {
            m_states.back()->Input(event);
        }
    }
}

void GameManager::update(sf::Time timePerFrame) {
    if (!m_states.empty()) {
        m_states.back()->Update(timePerFrame);
    }
}

void GameManager::render() {
    m_window.clear(sf::Color::Black);

    if (!m_states.empty()) {
        int startIdx = static_cast<int>(m_states.size()) - 1;
        while (startIdx > 0 && m_states[startIdx]->isOverlay()) {
            startIdx--;
        }

        for (int i = startIdx; i < static_cast<int>(m_states.size()); ++i) {
            m_states[i]->Render(m_window);
        }
    }

    m_window.display();
}
