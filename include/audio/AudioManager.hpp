#pragma once

#include <SFML/Audio.hpp>

#include <array>
#include <cstddef>
#include <map>

enum class SoundEffect {
    Jump,
    Coin,
    PowerUp,
    OneUp,
    Invincibility,
    SpeedBoost,
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
    static constexpr std::size_t VoicesPerEffect = 4U;

    struct SoundPool {
        std::array<sf::Sound, VoicesPerEffect> voices;
        std::size_t nextVoice = 0U;
    };

    AudioManager() = default;

    sf::Music m_backgroundMusic;
    std::map<SoundEffect, SoundPool> m_soundPools;
    float m_musicVolume = 80.f;
    float m_effectsVolume = 80.f;
    bool m_initialized = false;
};
