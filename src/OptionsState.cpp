#include "OptionsState.hpp"
#include "GameManager.hpp"
#include "audio/AudioManager.hpp"
#include "commands/MenuCommands.hpp"
#include <iostream>
#include <stdexcept>

OptionsState::OptionsState() {
    if (!m_buttonTexture.loadFromFile("assets/sprites/button/btn_transparent.png")) {
        throw std::runtime_error("Failed to load assets/sprites/button/btn_transparent.png");
    }

    if (!m_font.loadFromFile("assets/fonts/ro-spritendo-font/RoSpritendoSemiboldBeta-vmVwZ.otf")) {
        throw std::runtime_error("Failed to load assets/fonts/ro-spritendo-font/RoSpritendoSemiboldBeta-vmVwZ.otf");
    }

    initUI();
}

void OptionsState::initUI() {
    auto& settings = GameManager::getInstance().getSettings();

    // Dim Background Overlay
    m_dimOverlay.setSize(sf::Vector2f(1920.f, 1080.f));
    m_dimOverlay.setFillColor(sf::Color(0, 0, 0, 150));

    // Centered Panel Background
    m_panelBackground = std::make_unique<Panel>(m_buttonTexture, sf::Vector2f(960.f, 540.f), sf::Vector2f(900.f, 850.f), 10.f);
    m_panelBackground->setColor(sf::Color(30, 30, 30, 240));

    // Title
    m_titleText.setFont(m_font);
    m_titleText.setString("OPTIONS");
    m_titleText.setCharacterSize(42);
    m_titleText.setFillColor(sf::Color(255, 180, 0));
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setOrigin(titleBounds.left + titleBounds.width / 2.f, titleBounds.top + titleBounds.height / 2.f);
    m_titleText.setPosition(960.f, 160.f);

    // Helper lambda to create section headers
    auto createSectionHeader = [this](const std::string& title, float y) {
        sf::Text headerText;
        headerText.setFont(m_font);
        headerText.setString("--- " + title + " ----------------------------");
        headerText.setCharacterSize(22);
        headerText.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect bounds = headerText.getLocalBounds();
        headerText.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        headerText.setPosition(960.f, y);
        m_sectionHeaders.push_back(headerText);
    };

    // Helper lambda to create labels
    auto createLabel = [this](const std::string& name, const std::string& actionKey, float y) {
        sf::Text labelText;
        labelText.setFont(m_font);
        labelText.setString(name);
        labelText.setCharacterSize(24);
        labelText.setFillColor(sf::Color::White);
        labelText.setPosition(580.f, y - 12.f); // Left align inside the panel
        m_controlLabels.push_back({labelText, actionKey});
    };

    // 1. Difficulty Section
    createSectionHeader("Difficulty", 215.f);
    createLabel("Difficulty", "", 255.f);
    m_difficultyButton = std::make_unique<Button>(
        SettingsManager::difficultyToString(settings.getDifficulty()),
        m_font, m_buttonTexture, sf::Vector2f(1050.f, 255.f), sf::Vector2f(240.f, 50.f), 24
    );
    m_difficultyButton->setColors(sf::Color(60, 60, 60), sf::Color(80, 80, 80), sf::Color::White);
    m_difficultyButton->setCommand(std::make_unique<LambdaCommand>([this]() {
        auto& settings = GameManager::getInstance().getSettings();
        Difficulty current = settings.getDifficulty();
        Difficulty next = Difficulty::Normal;
        if (current == Difficulty::Easy) next = Difficulty::Normal;
        else if (current == Difficulty::Normal) next = Difficulty::Hard;
        else if (current == Difficulty::Hard) next = Difficulty::Easy;
        settings.setDifficulty(next);
        updateButtonLabels();
        settings.saveToFile();
    }));

    // 2. Sound Section
    createSectionHeader("Sound", 315.f);
    createLabel("Effect (SFX)", "", 355.f);
    m_sfxSlider = std::make_unique<Slider>(sf::Vector2f(1050.f, 355.f), 200.f, 0.f, 100.f, settings.getSFXVolume(), m_font);
    
    createLabel("Music (BGM)", "", 415.f);
    m_bgmSlider = std::make_unique<Slider>(sf::Vector2f(1050.f, 415.f), 200.f, 0.f, 100.f, settings.getBGMVolume(), m_font);

    // 3. Control Section
    createSectionHeader("Control", 475.f);
    
    std::vector<std::pair<std::string, std::string>> actions = {
        {"Move Up",    "MoveUp"},
        {"Move Down",  "MoveDown"},
        {"Move Left",  "MoveLeft"},
        {"Move Right", "MoveRight"},
        {"Action",    "Action"},
        {"Pause",     "Pause"}
    };

    float startY = 515.f;
    float stepY = 50.f;
    for (size_t i = 0; i < actions.size(); ++i) {
        float y = startY + i * stepY;
        createLabel(actions[i].first, actions[i].second, y);

        sf::Keyboard::Key key = settings.getKeyBinding(actions[i].second);
        auto btn = std::make_unique<Button>(
            SettingsManager::keyToString(key),
            m_font, m_buttonTexture, sf::Vector2f(1050.f, y), sf::Vector2f(240.f, 40.f), 20
        );
        btn->setColors(sf::Color(60, 60, 60), sf::Color(80, 80, 80), sf::Color::White);
        
        std::string actionKey = actions[i].second;
        btn->setCommand(std::make_unique<LambdaCommand>([this, actionKey]() {
            m_rebindingAction = actionKey;
            updateButtonLabels();
        }));
        m_bindButtons[actionKey] = std::move(btn);
    }

    // Reset Defaults
    m_resetButton = std::make_unique<Button>(
        "RESET DEFAULTS", m_font, m_buttonTexture, sf::Vector2f(960.f, 825.f), sf::Vector2f(300.f, 50.f), 22
    );
    m_resetButton->setColors(sf::Color(120, 40, 40), sf::Color(150, 60, 60), sf::Color::White);
    m_resetButton->setCommand(std::make_unique<LambdaCommand>([this]() {
        auto& settings = GameManager::getInstance().getSettings();
        settings.resetToDefaults();
        m_sfxSlider->setValue(settings.getSFXVolume());
        m_bgmSlider->setValue(settings.getBGMVolume());
        AudioManager::getInstance().setEffectsVolume(
            settings.getSFXVolume()
        );
        AudioManager::getInstance().setMusicVolume(
            settings.getBGMVolume()
        );
        updateButtonLabels();
        settings.saveToFile();
    }));

    // Back Button
    m_backButton = std::make_unique<Button>(
        "BACK", m_font, m_buttonTexture, sf::Vector2f(960.f, 895.f), sf::Vector2f(300.f, 50.f), 22
    );
    m_backButton->setColors(sf::Color(40, 100, 40), sf::Color(60, 130, 60), sf::Color::White);
    m_backButton->setCommand(std::make_unique<PopStateCommand>());
}

void OptionsState::updateButtonLabels() {
    auto& settings = GameManager::getInstance().getSettings();

    // 1. Difficulty
    m_difficultyButton->setText(SettingsManager::difficultyToString(settings.getDifficulty()));

    // 2. Control Keybinds
    for (auto& pair : m_bindButtons) {
        if (m_rebindingAction == pair.first) {
            pair.second->setText("[ PRESS ANY KEY ]");
        } else {
            sf::Keyboard::Key key = settings.getKeyBinding(pair.first);
            pair.second->setText(SettingsManager::keyToString(key));
        }
    }
}

void OptionsState::Input(const sf::Event &event) {
    auto& settings = GameManager::getInstance().getSettings();

    // Rebinding input routing
    if (!m_rebindingAction.empty()) {
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code != sf::Keyboard::Escape) {
                settings.setKeyBinding(m_rebindingAction, event.key.code);
                settings.saveToFile();
            }
            m_rebindingAction = "";
            updateButtonLabels();
        }
        return; // Absorb all inputs during rebinding
    }

    // Normal Input Routing
    sf::RenderWindow &window = GameManager::getInstance().getWindow();
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

    // Handle mouse move updates (hover)
    if (event.type == sf::Event::MouseMoved) {
        m_difficultyButton->update(mousePos);
        for (auto& pair : m_bindButtons) {
            pair.second->update(mousePos);
        }
        m_resetButton->update(mousePos);
        m_backButton->update(mousePos);
    }

    // Handle clicks
    if (event.type == sf::Event::MouseButtonReleased) {
        if (m_difficultyButton->handleClick(event, mousePos)) return;
        for (auto& pair : m_bindButtons) {
            if (pair.second->handleClick(event, mousePos)) return;
        }
        if (m_resetButton->handleClick(event, mousePos)) return;
        if (m_backButton->handleClick(event, mousePos)) return;
    }

    // Sliders
    float oldSfx = m_sfxSlider->getValue();
    float oldBgm = m_bgmSlider->getValue();
    m_sfxSlider->handleInput(event, mousePos);
    m_bgmSlider->handleInput(event, mousePos);

    if (m_sfxSlider->getValue() != oldSfx) {
        settings.setSFXVolume(m_sfxSlider->getValue());
        AudioManager::getInstance().setEffectsVolume(
            settings.getSFXVolume()
        );
        settings.saveToFile();
    }
    if (m_bgmSlider->getValue() != oldBgm) {
        settings.setBGMVolume(m_bgmSlider->getValue());
        AudioManager::getInstance().setMusicVolume(
            settings.getBGMVolume()
        );
        settings.saveToFile();
    }

    // Escape exits options
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        GameManager::getInstance().popState();
    }
}

void OptionsState::Update(sf::Time timePerFrame) {
}

void OptionsState::Render(sf::RenderWindow &window) {
    window.draw(m_dimOverlay);
    if (m_panelBackground) {
        m_panelBackground->render(window);
    }
    window.draw(m_titleText);

    for (const auto& header : m_sectionHeaders) {
        window.draw(header);
    }

    for (const auto& labelPair : m_controlLabels) {
        window.draw(labelPair.first);
    }

    m_difficultyButton->render(window);
    m_sfxSlider->render(window);
    m_bgmSlider->render(window);

    for (const auto& pair : m_bindButtons) {
        pair.second->render(window);
    }

    m_resetButton->render(window);
    m_backButton->render(window);
}
