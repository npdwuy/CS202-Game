#include "audio/AudioManager.hpp"

#include "resources/ResourceManager.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>

AudioManager& AudioManager::getInstance() {
    static AudioManager instance;
    return instance;
}

bool AudioManager::initialize() {
    if (m_initialized) {
        return true;
    }

    try {
        ResourceManager& resources = ResourceManager::getInstance();

        const std::map<SoundEffect, std::string> effectPaths = {
            {SoundEffect::Jump, "assets/audio/jump.wav"},
            {SoundEffect::Coin, "assets/audio/coin.wav"},
            {SoundEffect::PowerUp, "assets/audio/power_up.wav"},
            {SoundEffect::EnemyDefeated, "assets/audio/enemy_defeated.wav"},
            {SoundEffect::GameOver, "assets/audio/game_over.wav"}
        };

        for (const auto& entry : effectPaths) {
            SoundPool& pool = m_soundPools[entry.first];
            const sf::SoundBuffer& buffer = resources.getSoundBuffer(entry.second);
            for (sf::Sound& voice : pool.voices) {
                voice.setBuffer(buffer);
                voice.setVolume(m_effectsVolume);
            }
        }

        if (!m_backgroundMusic.openFromFile("assets/audio/background.wav")) {
            throw std::runtime_error(
                "Failed to load background music: assets/audio/background.wav"
            );
        }

        m_backgroundMusic.setLoop(true);
        m_backgroundMusic.setVolume(m_musicVolume);
        m_initialized = true;
    } catch (const std::exception& error) {
        std::cerr << "Audio system disabled: " << error.what() << '\n';
        m_soundPools.clear();
        m_initialized = false;
    }

    return m_initialized;
}

void AudioManager::playMusic() {
    if (!m_initialized && !initialize()) {
        return;
    }

    if (m_backgroundMusic.getStatus() != sf::SoundSource::Playing) {
        m_backgroundMusic.play();
    }
}

void AudioManager::stopMusic() {
    if (m_initialized) {
        m_backgroundMusic.stop();
    }
}

void AudioManager::shutdown()
{
    m_backgroundMusic.stop();

    for (auto& entry : m_soundPools) {
        for (sf::Sound& voice : entry.second.voices) {
            voice.stop();
        }
    }

    m_soundPools.clear();
    m_initialized = false;
}

void AudioManager::playEffect(SoundEffect effect) {
    if (!m_initialized && !initialize()) {
        return;
    }

    const auto found = m_soundPools.find(effect);
    if (found == m_soundPools.end()) {
        return;
    }

    SoundPool& pool = found->second;
    sf::Sound* selectedVoice = nullptr;
    for (sf::Sound& voice : pool.voices) {
        if (voice.getStatus() != sf::SoundSource::Playing) {
            selectedVoice = &voice;
            break;
        }
    }

    if (selectedVoice == nullptr) {
        selectedVoice = &pool.voices[pool.nextVoice];
        selectedVoice->stop();
    }
    pool.nextVoice = (pool.nextVoice + 1U) % pool.voices.size();
    selectedVoice->play();
}

void AudioManager::setMusicVolume(float volume) {
    m_musicVolume = std::clamp(volume, 0.f, 100.f);
    if (m_initialized) {
        m_backgroundMusic.setVolume(m_musicVolume);
    }
}

void AudioManager::setEffectsVolume(float volume) {
    m_effectsVolume = std::clamp(volume, 0.f, 100.f);
    for (auto& entry : m_soundPools) {
        for (sf::Sound& voice : entry.second.voices) {
            voice.setVolume(m_effectsVolume);
        }
    }
}

bool AudioManager::isInitialized() const {
    return m_initialized;
}
