#include "GameManager.hpp"
#include <iostream>

GameManager::GameManager() : m_window(sf::VideoMode(1920, 1080), "CS202-Group5", sf::Style::Default),
                             m_isRunning(true) 
{
    m_window.setFramerateLimit(30);
    m_gameView.setSize(1920.f, 1080.f);
    m_gameView.setCenter(960.f, 540.f);
    updateView(m_window.getSize().x, m_window.getSize().y);
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
    const sf::Time timePerFrame = sf::seconds(1.f / 30.f);

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

const sf::View& GameManager::getGameView() const {
    return m_gameView;
}

void GameManager::updateView(unsigned int windowWidth, unsigned int windowHeight) {
    if (windowWidth == 0 || windowHeight == 0) return;

    const float targetRatio = 1920.f / 1080.f; // 16:9
    float windowRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

    float viewportX = 0.f;
    float viewportY = 0.f;
    float viewportWidth = 1.f;
    float viewportHeight = 1.f;

    if (windowRatio > targetRatio) {
        // Window is wider than 16:9 -> Fit height, black bars left & right (Pillarboxing)
        viewportWidth = targetRatio / windowRatio;
        viewportX = (1.f - viewportWidth) / 2.f;
    } else {
        // Window is taller than 16:9 -> Fit width, black bars top & bottom (Letterboxing)
        viewportHeight = windowRatio / targetRatio;
        viewportY = (1.f - viewportHeight) / 2.f;
    }

    m_gameView.setViewport(sf::FloatRect(viewportX, viewportY, viewportWidth, viewportHeight));
    m_window.setView(m_gameView);
}

void GameManager::processInput() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            quit();
            return;
        }
        if (event.type == sf::Event::Resized) {
            updateView(event.size.width, event.size.height);
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

