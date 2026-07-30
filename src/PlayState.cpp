#include "PlayState.hpp"
#include "MenuState.hpp"
#include "GameManager.hpp"
#include "PauseState.hpp"

#include "audio/AudioManager.hpp"
#include "entities/enemies/BossEnemy.hpp"
#include "entities/items/Coin.hpp"
#include "entities/items/PowerUpPickup.hpp"
#include "entities/player/Mario.hpp"
#include "persistence/LoadManager.hpp"
#include "persistence/SaveManager.hpp"
#include "resources/ResourceManager.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>

#if __has_include("entities/items/ItemEffect.hpp")
#include "entities/items/ItemEffect.hpp"
#endif

PlayState::PlayState(bool loadSavedGame) {
    m_hudFont = &ResourceManager::getInstance().getFont(
        "assets/fonts/ro-spritendo-font/RoSpritendoSemiboldBeta-vmVwZ.otf"
    );

    m_hudText.setFont(*m_hudFont);
    m_hudText.setCharacterSize(24);
    m_hudText.setFillColor(sf::Color::White);
    m_hudText.setOutlineColor(sf::Color::Black);
    m_hudText.setOutlineThickness(2.f);
    m_hudText.setPosition(18.f, 14.f);

    m_statusText.setFont(*m_hudFont);
    m_statusText.setCharacterSize(34);
    m_statusText.setFillColor(sf::Color(255, 235, 120));
    m_statusText.setOutlineColor(sf::Color::Black);
    m_statusText.setOutlineThickness(3.f);

    SettingsManager& settings = GameManager::getInstance().getSettings();
    AudioManager& audio = AudioManager::getInstance();
    audio.setMusicVolume(settings.getBGMVolume());
    audio.setEffectsVolume(settings.getSFXVolume());
    audio.initialize();
    audio.playMusic();

    if (loadSavedGame) {
        const std::optional<SaveData> loadedData = LoadManager::load();
        if (loadedData) {
            m_saveData = *loadedData;
            loadLevel(m_saveData.currentLevel, true);
            showStatus("Save loaded");
        } else {
            loadLevel(1, false);
            showStatus("No valid save found - starting Level 1", 3.f);
        }
    } else {
        loadLevel(1, false);
        showStatus("Level 1 - Green Hill Start", 2.5f);
    }

    updateHud();
}

PlayState::~PlayState() {
    AudioManager::getInstance().stopMusic();
}

void PlayState::Input(const sf::Event& event) {
    if (event.type != sf::Event::KeyPressed) {
        return;
    }

    if (event.key.code == sf::Keyboard::F5) {
        saveGame();
        return;
    }

    if (event.key.code == sf::Keyboard::F9) {
        loadGame();
        return;
    }

    if (event.key.code == sf::Keyboard::R && (m_gameOver || m_victory)) {
        restartGame();
        return;
    }

    if (m_victory && event.key.code == sf::Keyboard::Enter) {
        GameManager::getInstance().changeState(std::make_unique<MenuState>());
        return;
    }

    const sf::Keyboard::Key pauseKey =
        GameManager::getInstance().getSettings().getKeyBinding("Pause");
    if (event.key.code == pauseKey) {
        GameManager::getInstance().pushState(std::make_unique<PauseState>());
    }
}

void PlayState::Update(sf::Time timePerFrame) {
    if (m_statusTimeRemaining > 0.f) {
        m_statusTimeRemaining = std::max(
            0.f,
            m_statusTimeRemaining - timePerFrame.asSeconds()
        );
    }

    if (m_gameOver || m_victory) {
        updateHud();
        return;
    }

    m_player->update(timePerFrame);
    if (m_player->consumeJumpEvent()) {
        AudioManager::getInstance().playEffect(SoundEffect::Jump);
    }

    for (auto& enemy : m_enemies) {
        enemy->Update(timePerFrame);
    }

    for (auto& item : m_items) {
        item->Update(timePerFrame);
    }

    handleItemCollisions();
    if (handleEnemyCollisions()) {
        updateHud();
        return;
    }
    handlePlayerFall();
    if (m_gameOver) {
        updateHud();
        return;
    }
    handleLevelExit();

    m_enemies.erase(
        std::remove_if(
            m_enemies.begin(),
            m_enemies.end(),
            [](const std::unique_ptr<Enemy>& enemy) {
                return !enemy->IsActive();
            }
        ),
        m_enemies.end()
    );

    m_items.erase(
        std::remove_if(
            m_items.begin(),
            m_items.end(),
            [](const std::unique_ptr<Item>& item) {
                return item->IsCollected();
            }
        ),
        m_items.end()
    );

    updateHud();
}

void PlayState::Render(sf::RenderWindow& window) {
    m_tileMap.render(window);

    for (const auto& enemy : m_enemies) {
        enemy->Render(window);
    }

    for (const auto& item : m_items) {
        item->Render(window);
    }

    if (m_player) {
        m_player->Render(window);
    }

    window.draw(m_hudText);
    if (m_statusTimeRemaining > 0.f || m_gameOver || m_victory) {
        window.draw(m_statusText);
    }
}

void PlayState::loadLevel(int levelNumber, bool restoreSavedPosition) {
    if (levelNumber < 1 || levelNumber > 3) {
        throw std::out_of_range("Level number must be between 1 and 3.");
    }

    m_saveData.currentLevel = levelNumber;
    m_tileMap.load(levelPath(levelNumber));
    m_enemies.clear();
    m_items.clear();
    createLevelObjects();

    sf::Vector2f spawnPosition = m_tileMap.data().playerStart;
    if (restoreSavedPosition && m_saveData.hasPlayerPosition) {
        const sf::Vector2f savedPosition{
            m_saveData.playerX,
            m_saveData.playerY
        };
        if (
            m_tileMap.worldBounds().contains(savedPosition) &&
            !m_tileMap.isSolidAt(savedPosition)
        ) {
            spawnPosition = savedPosition;
        }
    }

    m_player = std::make_unique<Mario>(spawnPosition);
    m_player->setCollisionResolver(
        [this](Character& character, sf::Time deltaTime) {
            m_tileMap.resolveCollision(character, deltaTime);
        }
    );

    m_saveData.hasPlayerPosition = false;
    m_gameOver = m_saveData.remainingLives <= 0;
    m_victory = false;
}

void PlayState::createLevelObjects() {
    const LevelData& level = m_tileMap.data();
    const float tileSize = static_cast<float>(level.tileSize);
    const float levelWidth = level.worldSize().x;

    for (const LevelSpawnRequest& request : level.spawnRequests) {
        if (LevelObjectFactory::isEnemySymbol(request.symbol)) {
            m_enemies.push_back(
                m_objectFactory.createEnemy(
                    request.symbol,
                    request.position,
                    tileSize,
                    levelWidth
                )
            );
        } else if (LevelObjectFactory::isItemSymbol(request.symbol)) {
            m_items.push_back(
                m_objectFactory.createItem(request.symbol, request.position)
            );
        }
    }
}

void PlayState::saveGame() {
    if (m_player) {
        const sf::Vector2f position = m_player->position();
        m_saveData.hasPlayerPosition = true;
        m_saveData.playerX = position.x;
        m_saveData.playerY = position.y;
    }

    if (SaveManager::save(m_saveData)) {
        showStatus("Game saved");
    } else {
        showStatus("Save failed - check the console", 3.f);
    }
}

void PlayState::loadGame() {
    const std::optional<SaveData> loadedData = LoadManager::load();
    if (!loadedData) {
        showStatus("No valid save file found", 3.f);
        return;
    }

    m_saveData = *loadedData;
    loadLevel(m_saveData.currentLevel, true);
    showStatus("Game loaded");
    updateHud();
}

void PlayState::handleItemCollisions() {
    const sf::FloatRect bounds = playerBounds();

    for (auto& item : m_items) {
        if (item->IsCollected() || !bounds.intersects(item->GetBounds())) {
            continue;
        }

        item->Collect();

        if (const auto* coin = dynamic_cast<const Coin*>(item.get())) {
            m_saveData.score += coin->GetValue();
            AudioManager::getInstance().playEffect(SoundEffect::Coin);
        } else if (
            const auto* powerUp =
                dynamic_cast<const PowerUpPickup*>(item.get())
        ) {
            m_saveData.score += 500;
            m_saveData.powerUpState =
                powerUp->getKind() == PowerUpKind::Mushroom
                    ? "Mushroom"
                    : "FireFlower";
            AudioManager::getInstance().playEffect(SoundEffect::PowerUp);
            showStatus(m_saveData.powerUpState + " collected");
#if __has_include("entities/items/ItemEffect.hpp")
        } else {
            const ItemEffect effect = item->GetEffect();
            if (effect.type == ItemEffectType::GrowPlayer) {
                m_saveData.score += 500;
                m_saveData.powerUpState = "Mushroom";
                AudioManager::getInstance().playEffect(SoundEffect::PowerUp);
                showStatus("Mushroom collected");
            } else if (effect.type == ItemEffectType::EnableFirePower) {
                m_saveData.score += 500;
                m_saveData.powerUpState = "FireFlower";
                AudioManager::getInstance().playEffect(SoundEffect::PowerUp);
                showStatus("FireFlower collected");
            }
#endif
        }
    }
}

bool PlayState::handleEnemyCollisions() {
    const sf::FloatRect bounds = playerBounds();

    for (auto& enemy : m_enemies) {
        if (!enemy->IsActive() || !bounds.intersects(enemy->GetBounds())) {
            continue;
        }

        const sf::FloatRect enemyBounds = enemy->GetBounds();
        const bool stomped =
            m_player->velocity().y > 0.f &&
            bounds.top + bounds.height <=
                enemyBounds.top + enemyBounds.height * 0.65f;

        if (stomped) {
            enemy->Deactivate();
            sf::Vector2f velocity = m_player->velocity();
            velocity.y = -330.f;
            m_player->setVelocity(velocity);
            m_saveData.score +=
                dynamic_cast<BossEnemy*>(enemy.get()) != nullptr ? 2000 : 200;
            AudioManager::getInstance().playEffect(
                SoundEffect::EnemyDefeated
            );
            showStatus(
                dynamic_cast<BossEnemy*>(enemy.get()) != nullptr
                    ? "Boss defeated - the exit is open!"
                    : "Enemy defeated",
                1.5f
            );
        } else {
            loseLife();
            return true;
        }
    }

    return false;
}

void PlayState::handlePlayerFall() {
    if (
        m_player->position().y >
        m_tileMap.worldBounds().height + 180.f
    ) {
        loseLife();
    }
}

void PlayState::handleLevelExit() {
    if (!playerBounds().intersects(m_tileMap.exitBounds())) {
        return;
    }

    if (m_saveData.currentLevel == 3 && hasActiveBoss()) {
        showStatus("Defeat the boss before using the exit", 2.f);
        return;
    }

    m_saveData.score += 1000;
    m_saveData.hasPlayerPosition = false;

    if (m_saveData.currentLevel < 3) {
        const int nextLevel = m_saveData.currentLevel + 1;
        loadLevel(nextLevel, false);
        SaveManager::save(m_saveData);
        showStatus(
            "Level " + std::to_string(nextLevel) + " - " +
            m_tileMap.data().name,
            2.5f
        );
    } else {
        m_victory = true;
        m_statusText.setString(
            "YOU WIN! Press Enter for menu or R to restart"
        );
        const sf::FloatRect textBounds = m_statusText.getLocalBounds();
        m_statusText.setOrigin(
            textBounds.left + textBounds.width / 2.f,
            textBounds.top + textBounds.height / 2.f
        );
        m_statusText.setPosition(960.f, 180.f);
        SaveManager::save(m_saveData);
    }
}

void PlayState::loseLife() {
    --m_saveData.remainingLives;
    m_saveData.powerUpState = "None";
    m_saveData.hasPlayerPosition = false;

    if (m_saveData.remainingLives <= 0) {
        m_saveData.remainingLives = 0;
        m_gameOver = true;
        AudioManager::getInstance().playEffect(SoundEffect::GameOver);
        m_statusText.setString("GAME OVER - Press R to restart");
        const sf::FloatRect textBounds = m_statusText.getLocalBounds();
        m_statusText.setOrigin(
            textBounds.left + textBounds.width / 2.f,
            textBounds.top + textBounds.height / 2.f
        );
        m_statusText.setPosition(960.f, 180.f);
        return;
    }

    loadLevel(m_saveData.currentLevel, false);
    showStatus(
        "Life lost - " + std::to_string(m_saveData.remainingLives) +
        " remaining",
        2.f
    );
}

void PlayState::restartGame() {
    m_saveData = SaveData{};
    m_gameOver = false;
    m_victory = false;
    loadLevel(1, false);
    showStatus("New game started");
}

void PlayState::updateHud() {
    std::string status =
        "LEVEL " + std::to_string(m_saveData.currentLevel) +
        "   SCORE " + std::to_string(m_saveData.score) +
        "   LIVES " + std::to_string(m_saveData.remainingLives) +
        "   CHARACTER " + m_saveData.selectedCharacter +
        "   POWER " + m_saveData.powerUpState +
        "   [F5 SAVE / F9 LOAD]";

    m_hudText.setString(status);
}

void PlayState::showStatus(const std::string& message, float duration) {
    m_statusText.setString(message);
    const sf::FloatRect textBounds = m_statusText.getLocalBounds();
    m_statusText.setOrigin(
        textBounds.left + textBounds.width / 2.f,
        textBounds.top + textBounds.height / 2.f
    );
    m_statusText.setPosition(960.f, 120.f);
    m_statusTimeRemaining = duration;
}

sf::FloatRect PlayState::playerBounds() const {
    if (!m_player) {
        return {};
    }

    const sf::Vector2f position = m_player->position();
    return {
        position.x,
        position.y,
        m_player->width(),
        m_player->height()
    };
}

bool PlayState::hasActiveBoss() const {
    return std::any_of(
        m_enemies.begin(),
        m_enemies.end(),
        [](const std::unique_ptr<Enemy>& enemy) {
            return (
                enemy->IsActive() &&
                dynamic_cast<const BossEnemy*>(enemy.get()) != nullptr
            );
        }
    );
}

std::string PlayState::levelPath(int levelNumber) {
    return "levels/level" + std::to_string(levelNumber) + ".txt";
}
