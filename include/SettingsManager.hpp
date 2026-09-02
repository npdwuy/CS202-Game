#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <map>

enum class Difficulty {
    Easy,
    Normal,
    Hard
};

class SettingsManager {
public:
    SettingsManager();
    ~SettingsManager() = default;

    // Difficulty
    Difficulty getDifficulty() const;
    void       setDifficulty(Difficulty diff);

    // Sound — Effect (SFX)
    float getSFXVolume() const;
    void  setSFXVolume(float volume);

    // Sound — Music (BGM)
    float getBGMVolume() const;
    void  setBGMVolume(float volume);

    // Control
    sf::Keyboard::Key getKeyBinding(const std::string& action) const;
    void setKeyBinding(const std::string& action, sf::Keyboard::Key key);
    const std::map<std::string, sf::Keyboard::Key>& getAllBindings() const;
    void resetToDefaults();

    // Persistence
    void  saveToFile(const std::string& path = "settings.txt");
    void  loadFromFile(const std::string& path = "settings.txt");

    // Static Helpers for Key-to-String conversions
    static std::string keyToString(sf::Keyboard::Key key);
    static sf::Keyboard::Key stringToKey(const std::string& str);

    static std::string difficultyToString(Difficulty diff);
    static Difficulty stringToDifficulty(const std::string& str);

    // Character selection (transient, not persisted to file)
    const std::string& getSelectedCharacter() const;
    void setSelectedCharacter(const std::string& name);

    // Level selection (transient, not persisted to file)
    int  getSelectedLevel() const;
    void setSelectedLevel(int level);

private:
    Difficulty m_difficulty;
    float m_sfxVolume;
    float m_bgmVolume;
    std::map<std::string, sf::Keyboard::Key> m_keyBindings;
    std::string m_selectedCharacter = "Mario";
    int         m_selectedLevel     = 1;
};
