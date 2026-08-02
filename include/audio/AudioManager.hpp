#pragma once

#include <SFML/Audio.hpp>

#include <map>

enum class SoundEffect {
    Jump,
    Coin,
    PowerUp,
    EnemyDefeated,
    GameOver
};

class AudioManager {
public:
    static AudioManager& getInstance();

    bool initialize();
    void playMusic();
    void stopMusic();
    void shutdown();
    void playEffect(SoundEffect effect);


    void setMusicVolume(float volume);
    void setEffectsVolume(float volume);
    bool isInitialized() const;

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

private:
    AudioManager() = default;

    sf::Music m_backgroundMusic;
    std::map<SoundEffect, sf::Sound> m_sounds;
    float m_musicVolume = 80.f;
    float m_effectsVolume = 80.f;
    bool m_initialized = false;
};
