#include "resources/ResourceManager.hpp"

#include <stdexcept>

ResourceManager& ResourceManager::getInstance() {
    static ResourceManager instance;
    return instance;
}

const sf::Texture& ResourceManager::getTexture(const std::string& path) {
    const auto existing = m_textures.find(path);
    if (existing != m_textures.end()) {
        return *existing->second;
    }

    auto texture = std::make_unique<sf::Texture>();
    if (!texture->loadFromFile(path)) {
        throw std::runtime_error("Failed to load texture resource: " + path);
    }

    const sf::Texture& resource = *texture;
    m_textures.emplace(path, std::move(texture));
    return resource;
}

const sf::Font& ResourceManager::getFont(const std::string& path) {
    const auto existing = m_fonts.find(path);
    if (existing != m_fonts.end()) {
        return *existing->second;
    }

    auto font = std::make_unique<sf::Font>();
    if (!font->loadFromFile(path)) {
        throw std::runtime_error("Failed to load font resource: " + path);
    }

    const sf::Font& resource = *font;
    m_fonts.emplace(path, std::move(font));
    return resource;
}

const sf::SoundBuffer& ResourceManager::getSoundBuffer(
    const std::string& path
) {
    const auto existing = m_soundBuffers.find(path);
    if (existing != m_soundBuffers.end()) {
        return *existing->second;
    }

    auto buffer = std::make_unique<sf::SoundBuffer>();
    if (!buffer->loadFromFile(path)) {
        throw std::runtime_error("Failed to load audio resource: " + path);
    }

    const sf::SoundBuffer& resource = *buffer;
    m_soundBuffers.emplace(path, std::move(buffer));
    return resource;
}

void ResourceManager::clear() {
    m_soundBuffers.clear();
    m_fonts.clear();
    m_textures.clear();
}
