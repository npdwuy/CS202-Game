#include "SettingsManager.hpp"
#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

SettingsManager::SettingsManager() {
    resetToDefaults();
    loadFromFile(); // Load saved configurations on startup if any
}

Difficulty SettingsManager::getDifficulty() const {
    return m_difficulty;
}

void SettingsManager::setDifficulty(Difficulty diff) {
    m_difficulty = diff;
}

float SettingsManager::getSFXVolume() const {
    return m_sfxVolume;
}

void SettingsManager::setSFXVolume(float volume) {
    m_sfxVolume = volume;
    if (m_sfxVolume < 0.f) m_sfxVolume = 0.f;
    if (m_sfxVolume > 100.f) m_sfxVolume = 100.f;
}

float SettingsManager::getBGMVolume() const {
    return m_bgmVolume;
}

void SettingsManager::setBGMVolume(float volume) {
    m_bgmVolume = volume;
    if (m_bgmVolume < 0.f) m_bgmVolume = 0.f;
    if (m_bgmVolume > 100.f) m_bgmVolume = 100.f;
}

sf::Keyboard::Key SettingsManager::getKeyBinding(const std::string& action) const {
    auto it = m_keyBindings.find(action);
    if (it != m_keyBindings.end()) {
        return it->second;
    }
    return sf::Keyboard::Unknown;
}

void SettingsManager::setKeyBinding(const std::string& action, sf::Keyboard::Key key) {
    m_keyBindings[action] = key;
}

const std::map<std::string, sf::Keyboard::Key>& SettingsManager::getAllBindings() const {
    return m_keyBindings;
}

void SettingsManager::resetToDefaults() {
    m_difficulty = Difficulty::Normal;
    m_sfxVolume = 80.f;
    m_bgmVolume = 80.f;
    m_keyBindings = {
        {"MoveUp",    sf::Keyboard::W},
        {"MoveDown",  sf::Keyboard::S},
        {"MoveLeft",  sf::Keyboard::A},
        {"MoveRight", sf::Keyboard::D},
        {"Action",    sf::Keyboard::Space},
        {"Pause",     sf::Keyboard::Escape}
    };
}

void SettingsManager::saveToFile(const std::string& path) {
    std::ofstream outFile(path);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open settings file for saving: " << path << std::endl;
        return;
    }

    outFile << "difficulty=" << difficultyToString(m_difficulty) << "\n";
    outFile << "sfxVolume=" << m_sfxVolume << "\n";
    outFile << "bgmVolume=" << m_bgmVolume << "\n";

    for (const auto& binding : m_keyBindings) {
        outFile << "key_" << binding.first << "=" << keyToString(binding.second) << "\n";
    }
}

void SettingsManager::loadFromFile(const std::string& path) {
    std::ifstream inFile(path);
    if (!inFile.is_open()) {
        // Safe to ignore on first launch
        return;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        // Remove comments or whitespace if needed
        std::size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) continue;

        std::string key = line.substr(0, equalPos);
        std::string val = line.substr(equalPos + 1);

        try {
            if (key == "difficulty") {
                if (val == "Easy" || val == "Normal" || val == "Hard") {
                    m_difficulty = stringToDifficulty(val);
                }
            } else if (key == "sfxVolume" || key == "bgmVolume") {
                std::size_t parsedCharacters = 0;
                const float volume = std::stof(val, &parsedCharacters);
                if (parsedCharacters != val.size() || !std::isfinite(volume)) {
                    throw std::invalid_argument("invalid volume");
                }

                if (key == "sfxVolume") {
                    setSFXVolume(volume);
                } else {
                    setBGMVolume(volume);
                }
            } else if (key.rfind("key_", 0) == 0) {
                const std::string action = key.substr(4);
                const sf::Keyboard::Key parsedKey = stringToKey(val);
                if (
                    parsedKey != sf::Keyboard::Unknown &&
                    m_keyBindings.find(action) != m_keyBindings.end()
                ) {
                    m_keyBindings[action] = parsedKey;
                }
            }
        } catch (const std::exception&) {
            std::cerr << "Ignoring invalid setting: " << line << '\n';
        }
    }
}

std::string SettingsManager::keyToString(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::A: return "A";
        case sf::Keyboard::B: return "B";
        case sf::Keyboard::C: return "C";
        case sf::Keyboard::D: return "D";
        case sf::Keyboard::E: return "E";
        case sf::Keyboard::F: return "F";
        case sf::Keyboard::G: return "G";
        case sf::Keyboard::H: return "H";
        case sf::Keyboard::I: return "I";
        case sf::Keyboard::J: return "J";
        case sf::Keyboard::K: return "K";
        case sf::Keyboard::L: return "L";
        case sf::Keyboard::M: return "M";
        case sf::Keyboard::N: return "N";
        case sf::Keyboard::O: return "O";
        case sf::Keyboard::P: return "P";
        case sf::Keyboard::Q: return "Q";
        case sf::Keyboard::R: return "R";
        case sf::Keyboard::S: return "S";
        case sf::Keyboard::T: return "T";
        case sf::Keyboard::U: return "U";
        case sf::Keyboard::V: return "V";
        case sf::Keyboard::W: return "W";
        case sf::Keyboard::X: return "X";
        case sf::Keyboard::Y: return "Y";
        case sf::Keyboard::Z: return "Z";
        case sf::Keyboard::Space: return "Space";
        case sf::Keyboard::Escape: return "Escape";
        case sf::Keyboard::Enter: return "Enter";
        case sf::Keyboard::Up: return "Up";
        case sf::Keyboard::Down: return "Down";
        case sf::Keyboard::Left: return "Left";
        case sf::Keyboard::Right: return "Right";
        case sf::Keyboard::LShift: return "LShift";
        case sf::Keyboard::RShift: return "RShift";
        case sf::Keyboard::LControl: return "LControl";
        case sf::Keyboard::RControl: return "RControl";
        case sf::Keyboard::LAlt: return "LAlt";
        case sf::Keyboard::RAlt: return "RAlt";
        default: return "Unknown";
    }
}

sf::Keyboard::Key SettingsManager::stringToKey(const std::string& str) {
    if (str == "A") return sf::Keyboard::A;
    if (str == "B") return sf::Keyboard::B;
    if (str == "C") return sf::Keyboard::C;
    if (str == "D") return sf::Keyboard::D;
    if (str == "E") return sf::Keyboard::E;
    if (str == "F") return sf::Keyboard::F;
    if (str == "G") return sf::Keyboard::G;
    if (str == "H") return sf::Keyboard::H;
    if (str == "I") return sf::Keyboard::I;
    if (str == "J") return sf::Keyboard::J;
    if (str == "K") return sf::Keyboard::K;
    if (str == "L") return sf::Keyboard::L;
    if (str == "M") return sf::Keyboard::M;
    if (str == "N") return sf::Keyboard::N;
    if (str == "O") return sf::Keyboard::O;
    if (str == "P") return sf::Keyboard::P;
    if (str == "Q") return sf::Keyboard::Q;
    if (str == "R") return sf::Keyboard::R;
    if (str == "S") return sf::Keyboard::S;
    if (str == "T") return sf::Keyboard::T;
    if (str == "U") return sf::Keyboard::U;
    if (str == "V") return sf::Keyboard::V;
    if (str == "W") return sf::Keyboard::W;
    if (str == "X") return sf::Keyboard::X;
    if (str == "Y") return sf::Keyboard::Y;
    if (str == "Z") return sf::Keyboard::Z;
    if (str == "Space") return sf::Keyboard::Space;
    if (str == "Escape") return sf::Keyboard::Escape;
    if (str == "Enter") return sf::Keyboard::Enter;
    if (str == "Up") return sf::Keyboard::Up;
    if (str == "Down") return sf::Keyboard::Down;
    if (str == "Left") return sf::Keyboard::Left;
    if (str == "Right") return sf::Keyboard::Right;
    if (str == "LShift") return sf::Keyboard::LShift;
    if (str == "RShift") return sf::Keyboard::RShift;
    if (str == "LControl") return sf::Keyboard::LControl;
    if (str == "RControl") return sf::Keyboard::RControl;
    if (str == "LAlt") return sf::Keyboard::LAlt;
    if (str == "RAlt") return sf::Keyboard::RAlt;
    return sf::Keyboard::Unknown;
}

std::string SettingsManager::difficultyToString(Difficulty diff) {
    switch (diff) {
        case Difficulty::Easy: return "Easy";
        case Difficulty::Normal: return "Normal";
        case Difficulty::Hard: return "Hard";
    }
    return "Normal";
}

Difficulty SettingsManager::stringToDifficulty(const std::string& str) {
    if (str == "Easy") return Difficulty::Easy;
    if (str == "Normal") return Difficulty::Normal;
    if (str == "Hard") return Difficulty::Hard;
    return Difficulty::Normal;
}
