#include "PlayState.hpp"
#include "entities/items/FloatingItem.hpp"
#include "MenuState.hpp"
#include "GameManager.hpp"
#include "PauseState.hpp"
#include "GameOverState.hpp"
#include "LevelCompleteState.hpp"
#include "commands/MenuCommands.hpp"

#include "audio/AudioManager.hpp"
// BossEnemy header removed, handled polymorphically
#include "entities/player/Mario.hpp"
#include "entities/player/Luigi.hpp"
#include "entities/player/Entity.hpp"
#include "persistence/LoadManager.hpp"
#include "persistence/SaveManager.hpp"
#include "resources/ResourceManager.hpp"
#include "events/GameEventManager.hpp"
#include "entities/enemies/Koopa.hpp"
#include "levels/MapGenerator.hpp"

#include <filesystem>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

PlayState::PlayState(bool loadSavedGame)
    : m_hud(ResourceManager::getInstance().getFont(
          "assets/fonts/ro-spritendo-font/RoSpritendoSemiboldBeta-vmVwZ.otf"
      )) {
    const sf::Font& hudFont = ResourceManager::getInstance().getFont(
        "assets/fonts/ro-spritendo-font/RoSpritendoSemiboldBeta-vmVwZ.otf"
    );

    
    m_flagScoreText.setFont(hudFont);
    m_flagScoreText.setCharacterSize(16);
    m_flagScoreText.setFillColor(sf::Color::White);
    m_flagScoreText.setOutlineColor(sf::Color::Black);
    m_flagScoreText.setOutlineThickness(1.f);
    
    m_menuButton = std::make_unique<Button>("...", hudFont, sf::Vector2f(24.f, 25.f), sf::Vector2f(40.f, 40.f), 24);
    m_menuButton->setColors(sf::Color::White, sf::Color(255, 230, 200, 255), sf::Color(255, 180, 0));
    m_menuButton->setCommand(std::make_unique<LambdaCommand>([]() {
        GameManager::getInstance().pushState(std::make_unique<PauseState>());
    }));
    SettingsManager& settings =
        GameManager::getInstance().getSettings();

    AudioManager& audio = AudioManager::getInstance();

    audio.setMusicVolume(settings.getBGMVolume());
    audio.setEffectsVolume(settings.getSFXVolume());
    audio.initialize();
    audio.playMusic();

    if (loadSavedGame)
    {
        const std::optional<SaveData> loadedData =
            LoadManager::load();

        if (loadedData)
        {
            m_saveData = *loadedData;
            loadLevel(m_saveData.currentLevel, true);
            showStatus("Save loaded");
        }
        else
        {
            loadLevel(1, false);
            showStatus(
                "No valid save found - starting Level 1",
                3.f
            );
        }
    }
    else
    {
        // Fresh start: use the character and level chosen in select screens
        m_saveData.selectedCharacter =
            GameManager::getInstance().getSettings().getSelectedCharacter();
        const int startLevel =
            GameManager::getInstance().getSettings().getSelectedLevel();
        loadLevel(startLevel, false);
    }

    updateHud();

    m_warpFadeOverlay.setSize(sf::Vector2f(1920.f, 1080.f));
    m_warpFadeOverlay.setFillColor(sf::Color(0, 0, 0, 0));

    GameEventManager::GetInstance().AddListener(this);
}

PlayState::~PlayState()
{
    GameEventManager::GetInstance().RemoveListener(this);
    AudioManager::getInstance().stopMusic();
}

void PlayState::OnGameEvent(const GameEvent& event)
{
    switch (event.type)
    {
        case GameEventType::CoinCollected:
            m_saveData.score += event.value;
            m_saveData.coins += 1;
            AudioManager::getInstance().playEffect(
                SoundEffect::Coin
            );

            if (m_saveData.coins >= 100) {
                m_saveData.coins -= 100;
                GameEventManager::GetInstance().Notify(
                    {
                        GameEventType::ExtraLifeCollected,
                        1,
                        "100 Coins"
                    }
                );
            }
            break;

        case GameEventType::PowerUpCollected:
            m_saveData.score += event.value;
            m_saveData.powerUpState =
                event.data.empty() ? "None" : event.data;

            AudioManager::getInstance().playEffect(
                SoundEffect::PowerUp
            );

            showStatus(
                m_saveData.powerUpState + " collected"
            );
            break;

        case GameEventType::ExtraLifeCollected:
            m_saveData.remainingLives = std::min(
                99,
                m_saveData.remainingLives + std::max(1, event.value)
            );
            AudioManager::getInstance().playEffect(SoundEffect::OneUp);
            showStatus("1-Up! Extra life gained", 2.f);
            break;

        case GameEventType::InvincibilityCollected:
            m_invincibilityTimeRemaining = std::max(
                m_invincibilityTimeRemaining,
                static_cast<float>(std::max(1, event.value))
            );
            AudioManager::getInstance().playEffect(SoundEffect::Invincibility);
            showStatus("Star power - invincible!", 2.f);
            break;

        case GameEventType::SpeedBoostCollected:
            m_speedBoostTimeRemaining = std::max(
                m_speedBoostTimeRemaining,
                static_cast<float>(std::max(1, event.value))
            );
            if (m_player) {
                m_player->setSpeedMultiplier(1.45f);
            }
            AudioManager::getInstance().playEffect(SoundEffect::SpeedBoost);
            showStatus("Speed boost active!", 2.f);
            break;

        case GameEventType::EnemyDefeated:
            m_saveData.score += event.value;

            AudioManager::getInstance().playEffect(
                SoundEffect::EnemyDefeated
            );

            showStatus(
                event.data.empty()
                    ? "Enemy defeated"
                    : event.data,
                1.5f
            );
            break;

        case GameEventType::PlayerDamaged:
            // Applying damage can reload the level and replace the enemy
            // collection. Defer it until the active collision pass has ended.
            m_playerDamagePending = true;
            break;

        case GameEventType::PlayerFell:
            loseLife();
            break;

        case GameEventType::LevelCompleted:
            m_saveData.score += event.value;
            break;

        case GameEventType::TimeExpired:
            triggerDeath();
            break;

        case GameEventType::EnemyFiredProjectile: {
            if (event.data.rfind("BossFiredProjectile:", 0) == 0) {
                std::string data = event.data.substr(20);
                std::stringstream ss(data);
                std::string token;
                std::vector<float> values;
                while (std::getline(ss, token, ',')) {
                    values.push_back(std::stof(token));
                }
                if (values.size() >= 4) {
                    m_bossFireballs.push_back(std::make_unique<BossFireball>(
                        sf::Vector2f(values[0], values[1]),
                        sf::Vector2f(values[2], values[3])
                    ));
                }
            } else if (event.data.rfind("HammerFiredProjectile:", 0) == 0) {
                std::string data = event.data.substr(22);
                std::stringstream ss(data);
                std::string token;
                std::vector<float> values;
                while (std::getline(ss, token, ',')) {
                    values.push_back(std::stof(token));
                }
                if (values.size() >= 4) {
                    float hammerScale = (values.size() >= 5) ? values[4] : 0.75f;
                    m_hammerProjectiles.push_back(std::make_unique<HammerProjectile>(
                        sf::Vector2f(values[0], values[1]),
                        sf::Vector2f(values[2], values[3]),
                        hammerScale
                    ));
                }
            }
            break;
        }
    }

    updateHud();
}

void PlayState::Input(const sf::Event& event) {
    if (event.type == sf::Event::MouseMoved || event.type == sf::Event::MouseButtonReleased) {
        sf::RenderWindow &window = GameManager::getInstance().getWindow();
        sf::Vector2i pixelPos = (event.type == sf::Event::MouseMoved) 
                                ? sf::Vector2i(event.mouseMove.x, event.mouseMove.y) 
                                : sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
        sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos, GameManager::getInstance().getGameView());
        
        if (event.type == sf::Event::MouseMoved) {
            if (m_menuButton) m_menuButton->update(mousePos);
        }
        if (event.type == sf::Event::MouseButtonReleased) {
            if (m_menuButton && m_menuButton->handleClick(event, mousePos)) return;
        }
    }

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


    const sf::Keyboard::Key pauseKey =
        GameManager::getInstance().getSettings().getKeyBinding("Pause");
    if (event.key.code == pauseKey) {
        GameManager::getInstance().pushState(std::make_unique<PauseState>());
    }
}

void PlayState::Update(sf::Time timePerFrame) {
    if (m_exitSequence != ExitSequence::None) {
        if (m_exitSequence == ExitSequence::Sliding) {
            // player is sliding down
            bool playerAtBottom = false;
            if (m_player->position().y + m_player->height() >= m_tileMap.getPoleBottomY() - 1.f) {
                m_player->setPosition({m_player->position().x, m_tileMap.getPoleBottomY() - m_player->height()});
                playerAtBottom = true;
            } else {
                sf::Vector2f pos = m_player->position();
                pos.y += 250.f * timePerFrame.asSeconds();
                m_player->setPosition(pos);
            }
            
            bool flagAtBottom = m_tileMap.updateFlagAnimation(timePerFrame, 200.f);
            
            if (playerAtBottom && flagAtBottom) {
                m_player->forceState(Player::State::AutoWalk);
                m_player->setFacing(1);
                m_exitSequence = ExitSequence::WalkingRight;
            }
        } else if (m_exitSequence == ExitSequence::WalkingRight) {
            m_player->setVelocity({180.f, 0.f}); 
            m_exitTimer += timePerFrame.asSeconds();
            if (m_exitTimer >= 1.0f) {
                m_exitSequence = ExitSequence::IrisWipe; 
                m_exitTimer = 0.f;
            }
        } else if (m_exitSequence == ExitSequence::IrisWipe) {
            m_player->setVelocity({180.f, 0.f}); // keep walking out
            m_exitTimer += timePerFrame.asSeconds();
            if (m_exitTimer >= 1.0f) {
                finishLevelExit();
                m_exitSequence = ExitSequence::None;
            }
        }
        
        m_player->update(timePerFrame);
        updateCamera(timePerFrame);
        updateBackgroundLayers(timePerFrame, m_camera.view());
        m_hud.update(timePerFrame);
        if (m_showFlagScore) {
            m_flagScoreTimer += timePerFrame.asSeconds();
            m_flagScoreText.move(0.f, -50.f * timePerFrame.asSeconds());
            if (m_flagScoreTimer > 2.0f) {
                m_showFlagScore = false;
            }
        }
        return;
    }

    if (m_levelStarting) {
        m_startTimer += timePerFrame.asSeconds();
        if (m_startTimer >= 1.0f) { // 1.0s opening to match closing duration
            m_levelStarting = false;
            if (m_player) {
                m_player->setInputEnabled(true);
            }
        }
    }

    updateBackgroundLayers(
        timePerFrame,
        m_camera.view()
    );
    m_hud.update(timePerFrame);
    
    if (m_warpCooldown > 0.f) {
        m_warpCooldown -= timePerFrame.asSeconds();
    }

    if (m_isWarping) {
        m_warpTimer += timePerFrame.asSeconds();
        float alpha = std::min(255.f, m_warpTimer * 255.f);
        m_warpFadeOverlay.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(alpha)));
        if (m_player) m_player->update(timePerFrame);
        
        if (m_warpTimer > 1.2f) {
            m_isWarping = false;
            m_wasWarping = true;
            m_warpTimer = 0.f;
            m_warpCooldown = 1.0f; // 1s cooldown after warping
            if (m_player) {
                m_player->isWarpingDown_ = false;
                if (m_player->isFireMario()) {
                    m_saveData.powerUpState = "FireFlower";
                } else if (m_player->isBig()) {
                    m_saveData.powerUpState = "Mushroom";
                } else {
                    m_saveData.powerUpState = "None";
                }
            }
            m_warpFadeOverlay.setFillColor(sf::Color(0, 0, 0, 0));
            loadLevel(m_warpDestinationLevel, false);
            updateHud();
        }
        return;
    }

    if (m_player && m_warpCooldown <= 0.f && sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        if (m_player->onGround()) {
            const auto& data = m_tileMap.data();
            sf::FloatRect bounds(m_player->position().x, m_player->position().y, m_player->width(), m_player->height());
            
            float centerX = bounds.left + bounds.width / 2.0f;
            float bottomY = bounds.top + bounds.height + 2.0f; 
            
            int col = static_cast<int>(centerX / data.tileSize);
            int row = static_cast<int>(bottomY / data.tileSize);
            
            if (row >= 0 && row < data.rows.size() && col >= 0 && col < data.rows[row].size()) {
                if (data.rows[row][col] == 'W') {
                    m_isWarping = true;
                    m_warpTimer = 0.f;
                    
                    if (m_saveData.currentLevel != 5) {
                        // Enter random sub-level
                        m_returnLevel = m_saveData.currentLevel;
                        m_returnPlayerPos = m_player->position();
                        m_hasReturnPos = true;
                        m_warpDestinationLevel = 5;

                        std::string pipeKey = "L" + std::to_string(m_saveData.currentLevel) + "_" + std::to_string(row) + "_" + std::to_string(col);
                        if (m_pipeSubLevelCache.find(pipeKey) == m_pipeSubLevelCache.end()) {
                            // Dynamically generate a fresh random compact sub-level matching the active level difficulty/theme
                            std::string currentDiff = m_tileMap.data().difficulty;
                            if (currentDiff.empty()) {
                                currentDiff = (m_saveData.currentLevel == 2) ? "Medium" : ((m_saveData.currentLevel == 3) ? "Hard" : "Easy");
                            }
                            MapGenerator::generateSubLevel(currentDiff, "levels/level5.txt");
                            std::ifstream f("levels/level5.txt");
                            if (f.is_open()) {
                                std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
                                m_pipeSubLevelCache[pipeKey] = content;
                            }
                        } else {
                            // Restore cached sub-level map from RAM for this specific pipe
                            const std::string& content = m_pipeSubLevelCache[pipeKey];
                            std::vector<std::string> paths = { "levels/level5.txt", "build/levels/level5.txt", "../levels/level5.txt" };
                            for (const auto& p : paths) {
                                std::ofstream out(p);
                                if (out.is_open()) {
                                    out << content;
                                }
                            }
                        }
                    } else {
                        // Exit sub-level back to main level
                        m_warpDestinationLevel = m_returnLevel;
                    }
                    
                    m_player->isWarpingDown_ = true;
                    m_player->setVelocity({0.f, 25.f}); 
                    AudioManager::getInstance().playEffect(SoundEffect::Pipe);
                }
            }
        }
    }
    
    if (m_player) {
        m_player->update(timePerFrame);

        if (m_fireballCooldown > 0.f) {
            m_fireballCooldown -= timePerFrame.asSeconds();
        }

        bool zPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Z);
        bool zJustPressed = zPressed && !m_wasZPressed;
        bool zJustReleased = !zPressed && m_wasZPressed;
        m_wasZPressed = zPressed;

        if (m_heldShell) {
            // Throw shell when releasing Z
            if (zJustReleased) {
                m_heldShell->Throw(m_player->facing());
                m_heldShell = nullptr;
                m_player->triggerThrow();
                m_fireballCooldown = 0.35f; 
            }
        } else {
            // Throw fireball when pressing Z
            if (zJustPressed && m_player->isFireMario() && m_fireballCooldown <= 0.f) {
                if (m_fireballs.size() < 20) { 
                    auto fb = std::make_unique<Fireball>(
                        m_player->position() + sf::Vector2f(m_player->facing() == 1 ? m_player->width() : -10.f, m_player->height() / 2.f),
                        m_player->facing()
                    );
                    fb->setCollisionResolver([this](Character& character, sf::Time dt) {
                        m_tileMap.resolveCollision(character, dt);
                    });
                    m_fireballs.push_back(std::move(fb));
                    
                    m_player->triggerThrow();
                    m_fireballCooldown = 0.35f;
                    AudioManager::getInstance().playEffect(SoundEffect::Fireball);
                }
            }
        }
    }

    if (m_heldShell && m_player) {
        m_player->setCarrying(true);
        sf::Vector2f pos = m_player->position();
        if (m_player->facing() == 1) {
            pos.x += m_player->width();
        } else {
            pos.x -= 48.f;
        }
        pos.y += m_player->height() / 2.f - 24.f; 
        m_heldShell->SetPosition(pos); 
    } else if (m_player) {
        m_player->setCarrying(false);
    }
    
    m_tileMap.update(timePerFrame);
    if (m_player->consumeJumpEvent()) {
        AudioManager::getInstance().playEffect(SoundEffect::Jump);
    }
    updateCamera(timePerFrame);

    if (m_player->isDead()) {
        updateHud();
        if (m_player->position().y > m_tileMap.worldBounds().height + 200.f) {
            loseLife();
        }
        return;
    }

    if (m_player->isTransforming()) {
        updateHud();
        return;
    }

    updateTimedPowerUps(timePerFrame);
    updateLevelTimer(timePerFrame);

    for (auto& enemy : m_enemies) {
        // Broadcast player position to every enemy.
        // Goomba/Koopa/Boss forward it to their ChaseStrategy. Other enemies ignore it (no-op).
        if (m_player) {
            enemy->SetPlayerPosition(m_player->position());
        }
        enemy->Update(timePerFrame);
        if (auto* koopa =
        dynamic_cast<Koopa*>(enemy.get()))
{
    koopa->UpdateShellPhysics(
        timePerFrame,
        m_tileMap
    );
}
    }
handleMovingShellEnemyCollisions();
    for (auto& item : m_items) {
        item->Update(timePerFrame);
    }

    for (auto& fb : m_fireballs) {
        fb->update(timePerFrame);
    }

    // Check Fireball Enemy collision
    for (auto& fb : m_fireballs) {
        if (fb->IsDestroyed()) continue;
        for (auto& enemy : m_enemies) {
            if (enemy.get() == m_heldShell) continue;
            if (enemy->IsActive() && fb->GetBounds().intersects(enemy->GetBounds())) {
                fb->Destroy();
                // Boss overrides TakeDamage to respect multi-hit HP, others just deactivate
                enemy->TakeDamage();
                break;
            }
        }
    }

    const sf::FloatRect visibleWorld = m_camera.visibleBounds(200.f); // slightly larger bounds

    for (auto& fb : m_fireballs) {
        if (!visibleWorld.intersects(fb->GetBounds())) {
            fb->Destroy();
        }
    }

    m_fireballs.erase(
        std::remove_if(m_fireballs.begin(), m_fireballs.end(), [](const std::unique_ptr<Fireball>& fb) {
            return fb->IsDestroyed();
        }),
        m_fireballs.end()
    );

    // Cập nhật và xử lý va chạm của Boss Fireballs
    for (auto& fireball : m_bossFireballs) {
        fireball->Update(timePerFrame);

        if (!fireball->IsDestroyed()) {
            const sf::FloatRect fbBounds = fireball->GetBounds();
            const sf::FloatRect mapBounds = m_tileMap.worldBounds();
            if (fbBounds.left < 0.f || fbBounds.left > mapBounds.width + 200.f) {
                fireball->Destroy();
                continue;
            }

            // Tự hủy khi va chạm vào tường/khối đất/gạch của map
            if (m_tileMap.intersectsSolid(fbBounds)) {
                fireball->Destroy();
                continue;
            }

            if (m_player && !m_player->isDead() && !m_player->isTransforming()) {
                if (playerBounds().intersects(fbBounds)) {
                    fireball->Destroy();
                    if (m_invincibilityTimeRemaining <= 0.f && m_damageCooldown <= 0.f) {
                        sf::Vector2f knockback = m_player->velocity();
                        knockback.x = (m_player->position().x < fbBounds.left) ? -280.f : 280.f;
                        knockback.y = -350.f;
                        m_player->setVelocity(knockback);

                        handlePlayerDamage();
                    }
                }
            }
        }
    }

    m_bossFireballs.erase(
        std::remove_if(m_bossFireballs.begin(), m_bossFireballs.end(), [](const std::unique_ptr<BossFireball>& fb) {
            return fb->IsDestroyed();
        }),
        m_bossFireballs.end()
    );

    // Cập nhật và xử lý va chạm của Hammer Projectiles
    for (auto& hammer : m_hammerProjectiles) {
        hammer->Update(timePerFrame);

        if (!hammer->IsDestroyed()) {
            const sf::FloatRect hBounds = hammer->GetBounds();
            const sf::FloatRect mapBounds = m_tileMap.worldBounds();
            if (hBounds.left < 0.f || hBounds.left > mapBounds.width + 200.f || hBounds.top > mapBounds.height + 300.f) {
                hammer->Destroy();
                continue;
            }

            if (m_player && !m_player->isDead() && !m_player->isTransforming()) {
                if (playerBounds().intersects(hBounds)) {
                    hammer->Destroy();
                    if (m_invincibilityTimeRemaining <= 0.f && m_damageCooldown <= 0.f) {
                        sf::Vector2f knockback = m_player->velocity();
                        knockback.x = (m_player->position().x < hBounds.left) ? -280.f : 280.f;
                        knockback.y = -350.f;
                        m_player->setVelocity(knockback);

                        handlePlayerDamage();
                    }
                }
            }
        }
    }

    m_hammerProjectiles.erase(
        std::remove_if(m_hammerProjectiles.begin(), m_hammerProjectiles.end(), [](const std::unique_ptr<HammerProjectile>& hp) {
            return hp->IsDestroyed();
        }),
        m_hammerProjectiles.end()
    );

    handleItemCollisions();
    if (handleEnemyCollisions()) {
        if (m_playerDamagePending) {
            m_playerDamagePending = false;
            handlePlayerDamage();
        }
        updateHud();
        return;
    }
    if (handlePlayerFall()) {
        updateHud();
        return;
    }
    handleLevelExit();

    m_enemies.erase(
        std::remove_if(
            m_enemies.begin(),
            m_enemies.end(),
            [this](const std::unique_ptr<Enemy>& enemy) {
                if (!enemy->IsActive()) {
                    if (enemy.get() == m_heldShell) {
                        m_heldShell = nullptr;
                    }
                    return true;
                }
                return false;
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
    const sf::View& screenView = GameManager::getInstance().getGameView();

    window.setView(m_camera.view());

    renderBackgroundLayers(window);

    const sf::FloatRect visibleWorld = m_camera.visibleBounds(96.f);

    m_tileMap.render(window);

    for (const auto& enemy : m_enemies) {
        if (visibleWorld.intersects(enemy->GetBounds())) {
            enemy->Render(window);
        }
    }

    for (const auto& item : m_items) {
        if (visibleWorld.intersects(item->GetBounds())) {
            item->Render(window);
        }
    }

    for (const auto& fb : m_fireballs) {
        if (visibleWorld.intersects(fb->GetBounds())) {
            fb->Render(window);
        }
    }

    for (const auto& fireball : m_bossFireballs) {
        if (visibleWorld.intersects(fireball->GetBounds())) {
            fireball->Render(window);
        }
    }

    for (const auto& hammer : m_hammerProjectiles) {
        if (visibleWorld.intersects(hammer->GetBounds())) {
            hammer->Render(window);
        }
    }

    if (m_player) {
        m_player->Render(window);
    }
    m_tileMap.renderForegroundPipes(window);

    if (m_showFlagScore) {
        window.draw(m_flagScoreText);
    }

    if (m_levelStarting || m_exitSequence == ExitSequence::IrisWipe || m_isWarping) {
        float rawProgress = 0.f;
        if (m_levelStarting) {
            rawProgress = std::clamp(m_startTimer / 1.0f, 0.0f, 1.0f); 
            // Cubic Ease-In for opening (mirroring closing wipe)
            rawProgress = std::pow(rawProgress, 3.0f);
        } else if (m_exitSequence == ExitSequence::IrisWipe) {
            rawProgress = std::clamp(1.0f - (m_exitTimer / 1.0f), 0.0f, 1.0f); 
            // Cubic Ease-In for closing
            rawProgress = std::pow(rawProgress, 3.0f);
        } else if (m_isWarping) {
            rawProgress = std::clamp(1.0f - (m_warpTimer / 1.2f), 0.0f, 1.0f);
            rawProgress = std::pow(rawProgress, 3.0f);
        }
        
        float radius = 1200.f * rawProgress;
        
        if (radius <= 0.5f) {
            // Draw a completely black screen when radius is effectively 0
            sf::RectangleShape blackScreen(sf::Vector2f(10000.f, 10000.f));
            blackScreen.setFillColor(sf::Color::Black);
            if (m_player) {
                blackScreen.setPosition(m_player->position().x - 5000.f, m_player->position().y - 5000.f);
            }
            window.draw(blackScreen);
        } else {
            // Draw smooth high-resolution circle (150 points)
            sf::CircleShape iris(radius, 150);
            iris.setOrigin(radius, radius);
            if (m_player) {
                iris.setPosition(m_player->position().x + m_player->width()/2.f, m_player->position().y + m_player->height()/2.f);
            }
            iris.setFillColor(sf::Color::Transparent);
            iris.setOutlineColor(sf::Color::Black);
            iris.setOutlineThickness(3000.f); // Massive outline to cover the screen
            
            window.draw(iris);
        }
    }

    window.setView(screenView);

    // Position the menu button relative to the current game view's top-left corner
    const sf::Vector2f viewSize = screenView.getSize();
    const sf::Vector2f viewCenter = screenView.getCenter();
    const float left = viewCenter.x - viewSize.x * 0.5f;
    const float top = viewCenter.y - viewSize.y * 0.5f;

    if (m_menuButton) {
        m_menuButton->setPosition(sf::Vector2f(left + 24.f, top + 25.f));
    }

    m_hud.layout(screenView);
    m_hud.render(window);
    if (m_menuButton) {
        m_menuButton->render(window);
    }
}

void PlayState::handleLevelStart() {
    m_levelStarting = true;
    m_startTimer = 0.f;
    if (m_player) {
        m_player->setInputEnabled(false);
    }
}

void PlayState::loadLevel(int levelNumber, bool restoreSavedPosition) {
    if (levelNumber < 1 || levelNumber > 5) {
        throw std::out_of_range("Level number must be between 1 and 5.");
    }

    handleLevelStart();

    m_saveData.currentLevel = levelNumber;
    m_playerDamagePending = false;
    m_tileMap.load(levelPath(levelNumber));
    loadBackgroundLayers();
    m_heldShell = nullptr;
    m_enemies.clear();
    m_items.clear();
    m_fireballs.clear();
    m_bossFireballs.clear();
    m_hammerProjectiles.clear();
    createLevelObjects();

    sf::Vector2f spawnPosition = m_tileMap.data().playerStart;
    
    if (m_wasWarping) {
        m_wasWarping = false;
        if (m_saveData.currentLevel != 5 && m_hasReturnPos) {
            // Returning to main level: spawn at saved pipe position
            spawnPosition = m_returnPlayerPos;
            m_hasReturnPos = false;
        } else {
            // Find the first 'W' vertical pipe
            bool foundPipe = false;
            for (int r = 0; r < m_tileMap.data().rows.size(); ++r) {
                for (int c = 0; c < m_tileMap.data().rows[r].size(); ++c) {
                    char ch = m_tileMap.data().rows[r][c];
                    if (ch == 'W') {
                        spawnPosition = sf::Vector2f(
                            c * m_tileMap.data().tileSize,
                            (r - 1) * m_tileMap.data().tileSize
                        );
                        foundPipe = true;
                        break;
                    }
                }
                if (foundPipe) break;
            }
        }
    } else if (restoreSavedPosition && m_saveData.hasPlayerPosition) {
        const sf::Vector2f savedPosition{
            m_saveData.playerX,
            m_saveData.playerY
        };
        const sf::FloatRect savedBounds(
            savedPosition.x,
            savedPosition.y,
            Player::CollisionWidth,
            Player::CollisionHeight
        );
        const sf::FloatRect worldBounds = m_tileMap.worldBounds();
        if (
            worldBounds.contains(savedBounds.left, savedBounds.top) &&
            worldBounds.contains(
                savedBounds.left + savedBounds.width,
                savedBounds.top + savedBounds.height
            ) &&
            !m_tileMap.intersectsSolid(savedBounds)
        ) {
            spawnPosition = savedPosition;
        }
    }

    if (m_saveData.selectedCharacter == "Luigi") {
        m_player = std::make_unique<Luigi>(spawnPosition);
    } else {
        m_player = std::make_unique<Mario>(spawnPosition);
    }

    if (m_saveData.powerUpState == "FireFlower" || m_saveData.powerUpState == "Fire") {
        m_player->setPowerUpState(Player::PowerUpState::Fire);
    } else if (m_saveData.powerUpState == "Mushroom" || m_saveData.powerUpState == "Big" || m_saveData.powerUpState == "Super") {
        m_player->setPowerUpState(Player::PowerUpState::Big);
    } else {
        m_player->setPowerUpState(Player::PowerUpState::Small);
    }

    if (m_invincibilityTimeRemaining > 0.f) {
        m_player->setInvincible(true);
    }
    if (m_speedBoostTimeRemaining > 0.f) {
        m_player->setSpeedMultiplier(1.45f);
    }
    m_player->setCollisionResolver(
        [this](Character& character, sf::Time deltaTime) {
            if (m_exitSequence != ExitSequence::None) {
                // Bypass all collisions during cutscenes so Mario can slide/walk off-screen freely
                sf::Vector2f pos = character.position();
                pos += character.velocity() * deltaTime.asSeconds();
                character.setPosition(pos);
                return;
            }

            m_tileMap.resolveCollision(character, deltaTime, [&](int row, int col) {
                if (&character == m_player.get()) {
                    if (row < m_tileMap.data().rows.size() - 2) {
                        bool bumped = false;

                        if (m_tileMap.data().rows[row][col] == '?') {
                            if (m_tileMap.hitQuestionBlock(row, col)) {
                                bumped = true;
                                AudioManager::getInstance().playEffect(SoundEffect::Jump); // Or another sound
                                
                                int randVal = std::rand() % 100;
                                char itemSymbol = 'M';
                                if (randVal < 1) itemSymbol = 'S';        // 1%
                                else if (randVal < 10) itemSymbol = 'L';  // 9%
                                else if (randVal < 30) itemSymbol = 'F';  // 20%
                                else if (randVal < 60) itemSymbol = 'V';  // 30%
                                
                                sf::Vector2f itemPos(col * m_tileMap.data().tileSize, row * m_tileMap.data().tileSize);
                                auto newItem = m_objectFactory.createItem(itemSymbol, itemPos);
                                if (auto floatingItem = dynamic_cast<FloatingItem*>(newItem.get())) {
                                    float targetY = row * m_tileMap.data().tileSize - floatingItem->GetBounds().height;
                                    floatingItem->StartSpawning(targetY, 0.5f);
                                }
                                m_items.push_back(std::move(newItem));
                            }
                        } else if (m_tileMap.data().rows[row][col] == 'B') {
                            if (m_invincibilityTimeRemaining > 0.f) {
                                m_tileMap.breakBlock(row, col);
                                bumped = true;
                                AudioManager::getInstance().playEffect(SoundEffect::EnemyDefeated);
                            } else if (m_player->isSuper()) {
                                bool destroyed = m_tileMap.hitBlock(row, col);
                                bumped = true;
                                if (destroyed) {
                                    AudioManager::getInstance().playEffect(SoundEffect::EnemyDefeated);
                                } else {
                                    AudioManager::getInstance().playEffect(SoundEffect::Jump);
                                }
                            }
                        }

                        // Check for enemies standing on top of the bumped block
                        if (bumped) {
                            float tileSize = static_cast<float>(m_tileMap.data().tileSize);
                            sf::FloatRect blockTopRect(col * tileSize, row * tileSize - 2.f, tileSize, 4.f);
                            
                            for (auto& enemy : m_enemies) {
                                if (enemy->IsActive() && !enemy->IsFlung() && enemy->GetBounds().intersects(blockTopRect)) {
                                    enemy->Fling();
                                    GameEventManager::GetInstance().Notify({GameEventType::EnemyDefeated, 100, "Enemy defeated by block bump!"});
                                }
                            }
                        }
                    }
                }
            });
        }
    );

    const sf::Vector2f playerCenter = spawnPosition + sf::Vector2f(
        m_player->width() * 0.5f,
        m_player->height() * 0.5f
    );
    m_camera.reset(
        playerCenter,
        m_tileMap.worldBounds(),
        GameManager::getInstance().getGameView()
    );

    m_saveData.hasPlayerPosition = false;
    m_timeRemaining = 400.f;
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
                    levelWidth,
                    m_tileMap
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
    m_saveData.remainingTime = m_timeRemaining;

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
    m_timeRemaining = m_saveData.remainingTime;
    resetTransientEffects();
    loadLevel(m_saveData.currentLevel, true);
    showStatus("Game loaded");
    updateHud();
}

void PlayState::handleItemCollisions()
{
    const sf::FloatRect bounds = playerBounds();

    for (auto& item : m_items)
    {
        if (
            item->IsCollected() ||
            !bounds.intersects(item->GetBounds())
        )
        {
            continue;
        }

        const ItemEffect effect = item->GetEffect();
        item->Collect();

        switch (effect.type)
        {
            case ItemEffectType::AddScore:
                GameEventManager::GetInstance().Notify(
                    {
                        GameEventType::CoinCollected,
                        effect.amount,
                        ""
                    }
                );
                break;

            case ItemEffectType::GrowPlayer:
                if (m_player->powerUpState() == Player::PowerUpState::Small) {
                    GameEventManager::GetInstance().Notify(
                        {
                            GameEventType::PowerUpCollected,
                            500,
                            "Mushroom"
                        }
                    );
                    m_player->getMushroom();
                } else {
                    GameEventManager::GetInstance().Notify(
                        {
                            GameEventType::PowerUpCollected,
                            1000,
                            ""
                        }
                    );
                }
                break;

            case ItemEffectType::EnableFirePower:
                if (m_player->isFireMario()) {
                    GameEventManager::GetInstance().Notify(
                        {
                            GameEventType::PowerUpCollected,
                            1000,
                            ""
                        }
                    );
                } else {
                    GameEventManager::GetInstance().Notify(
                        {
                            GameEventType::PowerUpCollected,
                            500,
                            "FireFlower"
                        }
                    );
                    m_player->up2Fire();
                }
                break;

            case ItemEffectType::ExtraLife:
                GameEventManager::GetInstance().Notify(
                    {
                        GameEventType::ExtraLifeCollected,
                        effect.amount,
                        "1-Up Mushroom"
                    }
                );
                break;

            case ItemEffectType::Invincibility:
                GameEventManager::GetInstance().Notify(
                    {
                        GameEventType::InvincibilityCollected,
                        effect.amount,
                        "Star"
                    }
                );
                break;

            case ItemEffectType::SpeedBoost:
                GameEventManager::GetInstance().Notify(
                    {
                        GameEventType::SpeedBoostCollected,
                        effect.amount,
                        "Speed Boost"
                    }
                );
                break;
        }
    }
}

bool PlayState::handleEnemyCollisions()
{
    const sf::FloatRect bounds = playerBounds();
    const bool isFalling = m_player && m_player->velocity().y > 0.f;
    bool stompedAny = false;

    for (auto& enemy : m_enemies)
    {
        if (enemy.get() == m_heldShell)
        {
            continue;
        }

        if (
            !enemy->IsActive() ||
            enemy->IsFlung() ||
            !bounds.intersects(enemy->GetBounds())
        )
        {
            continue;
        }

        const sf::FloatRect enemyBounds =
            enemy->GetBounds();

        const bool stomped =
            isFalling &&
            bounds.top + bounds.height <=
                enemyBounds.top +
                enemyBounds.height * 0.65f;

        const bool isBoss = enemy->IsBoss();

        auto* koopa =
            dynamic_cast<Koopa*>(enemy.get());

        // =========================================================
        // BOSS HURT STATE
        // =========================================================
        if (isBoss && enemy->IsHurt())
        {
            continue;
        }

        // =========================================================
        // STAR / INVINCIBILITY COLLISION
        // =========================================================
        if (m_invincibilityTimeRemaining > 0.f)
        {
            if (isBoss)
            {
                enemy->TakeDamage();

                if (!enemy->IsActive())
                {
                    GameEventManager::GetInstance().Notify(
                        {
                            GameEventType::EnemyDefeated,
                            2000,
                            "Boss defeated - the exit is open!"
                        }
                    );
                }
            }
            else
            {
                enemy->Deactivate();

                GameEventManager::GetInstance().Notify(
                    {
                        GameEventType::EnemyDefeated,
                        200,
                        "Enemy defeated by Star power"
                    }
                );
            }

            continue;
        }

        // =========================================================
        // PLAYER STOMPS ENEMY
        // =========================================================
        if (stomped)
        {
            stompedAny = true;
            m_damageCooldown = std::max(m_damageCooldown, 0.35f);

            // -----------------------------------------------------
            // BOSS
            // -----------------------------------------------------
            if (isBoss)
            {
                enemy->TakeDamage();

                m_damageCooldown = 1.5f;

                const float bossCenterX =
                    enemy->GetBounds().left +
                    enemy->GetBounds().width * 0.5f;

                const float pushDirection =
                    m_player->position().x < bossCenterX
                        ? -1.f
                        : 1.f;

                m_player->triggerBossKnockback(
                    pushDirection,
                    0.7f
                );

                if (!enemy->IsActive())
                {
                    GameEventManager::GetInstance().Notify(
                        {
                            GameEventType::EnemyDefeated,
                            2000,
                            "Boss defeated - the exit is open!"
                        }
                    );
                }

                continue;
            }

            // -----------------------------------------------------
            // BOUNCE MARIO
            // -----------------------------------------------------
            sf::Vector2f velocity =
                m_player->velocity();

            velocity.y = -330.f;

            m_player->setVelocity(
                velocity
            );

            // -----------------------------------------------------
            // KOOPA
            // -----------------------------------------------------
            if (koopa)
            {
                // First stomp:
                // Walking -> ShellIdle
                if (koopa->IsWalking())
                {
                    koopa->EnterShell();

                    continue;
                }

                // Shell is already idle.
                if (koopa->IsShellIdle())
                {
                    // During the short protection time after
                    // entering the shell, do absolutely nothing.
                    if (!koopa->CanKickShell())
                    {
                        continue;
                    }

                    // Second separate stomp:
                    // ShellIdle -> ShellMoving
                    const float playerCenter =
                        bounds.left +
                        bounds.width * 0.5f;

                    const float koopaCenter =
                        enemyBounds.left +
                        enemyBounds.width * 0.5f;

                    const int direction =
                        playerCenter < koopaCenter
                            ? 1
                            : -1;

                    koopa->KickShell(
                        direction
                    );

                    continue;
                }

                // Never deactivate a moving shell because
                // Mario stomped it.
                if (koopa->IsShellMoving())
                {
                    continue;
                }
            }

            // -----------------------------------------------------
            // NORMAL ENEMY
            // -----------------------------------------------------
            enemy->Deactivate();

            GameEventManager::GetInstance().Notify(
                {
                    GameEventType::EnemyDefeated,
                    200,
                    "Enemy defeated"
                }
            );

            continue;
        }

        // =========================================================
        // SIDE COLLISION WITH IDLE KOOPA SHELL
        // =========================================================
        if (
            koopa &&
            koopa->IsShellIdle()
        )
        {
            // Mario may still overlap the shell immediately
            // after the first stomp.
            // Ignore this overlap until the kick delay expires.
            if (!koopa->CanKickShell())
            {
                continue;
            }

            if (!m_heldShell)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
                    m_heldShell = koopa;
                    koopa->PickUp();
                } else {
                    const float playerCenter = playerBounds().left + playerBounds().width * 0.5f;
                    const float koopaCenter = koopa->GetBounds().left + koopa->GetBounds().width * 0.5f;
                    const int direction = (playerCenter < koopaCenter) ? 1 : -1;
                    koopa->KickShell(direction);
                }
            }

            continue;
        }

        // =========================================================
        // NORMAL SIDE DAMAGE
        // =========================================================
        if (stompedAny)
        {
            continue;
        }

        if (koopa && koopa->IsSafeFromPlayer())
        {
            continue;
        }

        if (m_damageCooldown > 0.f)
        {
            continue;
        }

        GameEventManager::GetInstance().Notify(
            {
                GameEventType::PlayerDamaged,
                0,
                ""
            }
        );

        return true;
    }

    return false;
}

void PlayState::handleMovingShellEnemyCollisions()
{
    for (auto& shellEnemy : m_enemies)
    {
        // Only a moving Koopa shell can attack other enemies.
        auto* shell =
            dynamic_cast<Koopa*>(shellEnemy.get());

        if (
            !shell ||
            !shell->IsActive() ||
            shell->IsFlung() ||
            !shell->IsShellMoving()
        )
        {
            continue;
        }

        const sf::FloatRect shellBounds =
            shell->GetBounds();

        for (auto& targetEnemy : m_enemies)
        {
            // Do not collide the shell with itself.
            if (targetEnemy.get() == shellEnemy.get())
            {
                continue;
            }

            if (
                !targetEnemy->IsActive() ||
                targetEnemy->IsFlung()
            )
            {
                continue;
            }

            if (
                !shellBounds.intersects(
                    targetEnemy->GetBounds()
                )
            )
            {
                continue;
            }

            // =====================================================
            // BOSS
            // =====================================================
            if (targetEnemy->IsBoss())
            {
                // Boss already has its own temporary hurt state,
                // so the shell cannot damage it every frame.
                if (!targetEnemy->IsHurt())
                {
                    targetEnemy->TakeDamage();

                    if (!targetEnemy->IsActive())
                    {
                        GameEventManager::GetInstance().Notify(
                            {
                                GameEventType::EnemyDefeated,
                                2000,
                                "Boss defeated by Koopa shell"
                            }
                        );
                    }
                }

                // Shell keeps moving after hitting the Boss.
                continue;
            }

            // =====================================================
            // NORMAL ENEMY
            // =====================================================
            targetEnemy->Deactivate();

            GameEventManager::GetInstance().Notify(
                {
                    GameEventType::EnemyDefeated,
                    200,
                    "Enemy defeated by Koopa shell"
                }
            );

            // IMPORTANT:
            // Do not deactivate shellEnemy here.
            // The shell continues moving and can hit more enemies.
        }
    }
}

bool PlayState::handlePlayerFall()
{
    if (
        m_player->position().y >
        m_tileMap.worldBounds().height + 180.f
    )
    {
        GameEventManager::GetInstance().Notify(
            {
                GameEventType::PlayerFell,
                0,
                "Player fell out of the level"
            }
        );
        return true;
    }

    return false;
}

void PlayState::handlePlayerDamage() {
    if (m_damageCooldown > 0.f || m_invincibilityTimeRemaining > 0.f) {
        return;
    }

    if (m_player && m_player->powerUpState() != Player::PowerUpState::Small) {
        m_player->shrinkPlayer();
        
        if (m_player->powerUpState() == Player::PowerUpState::Big) {
            m_saveData.powerUpState = "Mushroom";
        } else {
            m_saveData.powerUpState = "None";
        }
        
        m_damageCooldown = 1.5f;
        m_player->setInvincible(true);
        showStatus("Power-up absorbed the hit", 1.5f);
        updateHud();
        return;
    }

    triggerDeath();
}

void PlayState::updateTimedPowerUps(sf::Time timePerFrame) {
    const float deltaTime = timePerFrame.asSeconds();
    m_damageCooldown = std::max(0.f, m_damageCooldown - deltaTime);
    m_invincibilityTimeRemaining = std::max(
        0.f,
        m_invincibilityTimeRemaining - deltaTime
    );

    // Sync invincible flag on player for visual effects (rainbow)
    if (m_player) {
        m_player->setInvincible(m_invincibilityTimeRemaining > 0.f);
    }

    const float previousSpeedTime = m_speedBoostTimeRemaining;
    m_speedBoostTimeRemaining = std::max(
        0.f,
        m_speedBoostTimeRemaining - deltaTime
    );
    if (
        previousSpeedTime > 0.f &&
        m_speedBoostTimeRemaining <= 0.f &&
        m_player
    ) {
        m_player->setSpeedMultiplier(1.f);
        showStatus("Speed boost ended", 1.f);
    }
}

void PlayState::resetTransientEffects() {
    m_invincibilityTimeRemaining = 0.f;
    m_speedBoostTimeRemaining = 0.f;
    m_timeRemaining = 400.f;
    m_playerDamagePending = false;
    m_exitSequence = ExitSequence::None;
    m_heldShell = nullptr;

    m_enemies.clear();
    m_damageCooldown = 0.f;

    if (m_player) {
        m_player->setSpeedMultiplier(1.f);
    }
}

void PlayState::updateLevelTimer(sf::Time timePerFrame) {
    if (m_timeRemaining <= 0.f) {
        return;
    }

    m_timeRemaining -= timePerFrame.asSeconds();

    if (m_timeRemaining <= 0.f) {
        m_timeRemaining = 0.f;
        GameEventManager::GetInstance().Notify({
            GameEventType::TimeExpired,
            0,
            "Time's up!"
        });
    } else if (m_timeRemaining <= 30.f && m_timeRemaining + timePerFrame.asSeconds() > 30.f) {
        // Show warning exactly once when crossing the 30s threshold
        showStatus("Hurry up! Time is running out!", 2.5f);
    }
}

void PlayState::handleLevelExit() {
    if (m_exitSequence != ExitSequence::None) return;

    if (!m_tileMap.data().hasExit || !playerBounds().intersects(m_tileMap.exitBounds())) {
        return;
    }

    if (m_saveData.currentLevel == 3 && hasActiveBoss()) {
        showStatus("Defeat the boss before using the exit", 2.f);
        return;
    }

    m_exitSequence = ExitSequence::Sliding;
    m_player->setInputEnabled(false);
    
    // Snap to pole
    m_player->setPosition({m_tileMap.getPoleX() - m_player->width() / 2.0f, m_player->position().y});
    m_player->setVelocity({0.f, 0.f});
    m_player->forceState(Player::State::PoleSlide);
    m_player->setFacing(1); 
    
    // Score signifier
    float poleTop = m_tileMap.getPoleTopY();
    float poleBottom = m_tileMap.getPoleBottomY();
    float playerY = m_player->position().y + m_player->height();
    float heightPerc = 1.0f - std::clamp((playerY - poleTop) / (poleBottom - poleTop), 0.0f, 1.0f);
    
    int scorePoints = 100;
    if (heightPerc > 0.85f) scorePoints = 5000;
    else if (heightPerc > 0.6f) scorePoints = 2000;
    else if (heightPerc > 0.3f) scorePoints = 500;

    m_saveData.score += scorePoints;
    m_flagScoreText.setString(std::to_string(scorePoints));
    m_flagScoreText.setPosition(m_tileMap.getPoleX() + 16.f, m_player->position().y);
    m_showFlagScore = true;
    m_flagScoreTimer = 0.f;
}

void PlayState::finishLevelExit() {
    GameEventManager::GetInstance().Notify(
    {
        GameEventType::LevelCompleted,
        1000,
        "Level completed"
    }
    );
    // Time bonus: remaining seconds * 50
    const int timeBonus = static_cast<int>(m_timeRemaining) * 50;
    if (timeBonus > 0) {
        m_saveData.score += timeBonus;
        showStatus(
            "Time bonus: +" + std::to_string(timeBonus),
            2.f
        );
    }
    m_saveData.hasPlayerPosition = false;
    if (m_player) {
        if (m_player->isFireMario()) {
            m_saveData.powerUpState = "FireFlower";
        } else if (m_player->isBig()) {
            m_saveData.powerUpState = "Mushroom";
        } else {
            m_saveData.powerUpState = "None";
        }
    }

    if (m_saveData.currentLevel < 3) {
        const int nextLevel = m_saveData.currentLevel + 1;
        m_saveData.highestUnlockedLevel = std::max(m_saveData.highestUnlockedLevel, nextLevel);
        loadLevel(nextLevel, false);
        SaveManager::save(m_saveData);
        updateHud();
        showStatus(
            "Level " + std::to_string(nextLevel) + " - " +
            m_tileMap.data().name,
            2.5f
        );
    } else {
        SaveManager::save(m_saveData);
        GameManager::getInstance().pushState(std::make_unique<LevelCompleteState>());
    }
}

void PlayState::triggerDeath() {
    AudioManager::getInstance().stopMusic();
    AudioManager::getInstance().playEffect(SoundEffect::GameOver);
    if (m_player) {
        m_player->die();
    }
}

void PlayState::loseLife() {
    --m_saveData.remainingLives;
    m_saveData.powerUpState = "None";
    m_saveData.hasPlayerPosition = false;
    resetTransientEffects();

    if (m_saveData.remainingLives <= 0) {
        m_saveData.remainingLives = 0;
        AudioManager::getInstance().playEffect(SoundEffect::GameOver);
        GameManager::getInstance().pushState(std::make_unique<GameOverState>());
        return;
    }

    loadLevel(m_saveData.currentLevel, false);
    showStatus(
        "Life lost - " + std::to_string(m_saveData.remainingLives) +
        " remaining",
        2.f
    );
}

void PlayState::updateHud() {
    m_hud.setData({
        m_saveData.currentLevel,
        m_saveData.score,
        m_saveData.remainingLives,
        m_saveData.powerUpState,
        m_invincibilityTimeRemaining,
        m_speedBoostTimeRemaining,
        m_timeRemaining,
        m_saveData.coins
    });
}

void PlayState::updateCamera(sf::Time timePerFrame) {
    if (!m_player) {
        return;
    }

    const sf::Vector2f playerCenter = m_player->position() + sf::Vector2f(
        m_player->width() * 0.5f,
        m_player->height() * 0.5f
    );
    m_camera.update(
        playerCenter,
        m_player->velocity(),
        m_tileMap.worldBounds(),
        GameManager::getInstance().getGameView(),
        timePerFrame
    );
}

void PlayState::showStatus(const std::string& message, float duration) {
    m_hud.showStatus(message, duration);
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
                enemy->IsBoss()
            );
        }
    );
}

std::string PlayState::levelPath(int levelNumber) {
    return "levels/level" + std::to_string(levelNumber) + ".txt";
}

void PlayState::addBackgroundLayer(
    const std::string& path,
    float parallaxFactor,
    float driftAmplitude,
    float driftSpeed,
    bool required
)
{
    if (!std::filesystem::exists(path))
    {
        if (required)
        {
            throw std::runtime_error(
                "Missing required background layer: " + path
            );
        }

        return;
    }

    auto texture = std::make_shared<sf::Texture>();

    if (!texture->loadFromFile(path))
    {
        if (required)
        {
            throw std::runtime_error(
                "Failed to load background layer: " + path
            );
        }

        return;
    }

    BackgroundLayer layer;

    layer.texture = texture;
    layer.sprite.setTexture(*texture);
    layer.sprite.setColor(sf::Color(255, 255, 255, 255)); // Restore natural brightness

    layer.parallaxFactor = parallaxFactor;
    layer.driftAmplitude = driftAmplitude;
    layer.driftSpeed = driftSpeed;

    m_backgroundLayers.push_back(
        std::move(layer)
    );
}

void PlayState::loadBackgroundLayers()
{
    m_backgroundLayers.clear();
    m_clouds.clear();

    const std::string& difficulty = m_tileMap.data().difficulty;
    const bool isCastle = (m_saveData.currentLevel == 3 || difficulty == "Hard");
    const bool isLevel2 = (m_saveData.currentLevel == 2 || difficulty == "Medium");

    if (isCastle) {
        // Level 3 (Lâu đài): sử dụng bg2.png làm parallax background tràn viền
        m_parallaxBg.load("assets/sprites/bg2.png");
        addBackgroundLayer(
            "assets/sprites/bg2.png",
            0.20f,
            0.f,
            0.f,
            true
        );
    } else if (isLevel2) {
        // Level 2 (Athletic): sử dụng bg3.png làm parallax background tràn viền
        m_parallaxBg.load("assets/sprites/bg3.png");
        addBackgroundLayer(
            "assets/sprites/bg3.png",
            0.20f,
            0.f,
            0.f,
            true
        );
    } else {
        // Level 1 / Màn ngoài trời mặc định: sử dụng level_bg.png làm parallax tràn viền
        m_parallaxBg.load("assets/backgrounds/level_bg.png");
        addBackgroundLayer(
            "assets/sprites/long_background.png",
            0.20f,
            0.f,
            0.f,
            true
        );

        // Initialize Dynamic Clouds System
        m_cloudTexture = std::make_shared<sf::Texture>();
        if (m_cloudTexture->loadFromFile("assets/sprites/cloud.png"))
        {
            // Define bounding boxes for some good clouds from the new spritesheet
            std::vector<sf::IntRect> cloudRects = {
                sf::IntRect(43, 111, 373, 200),
                sf::IntRect(791, 126, 407, 232),
                sf::IntRect(17, 322, 497, 339),
                sf::IntRect(527, 340, 483, 431),
                sf::IntRect(1092, 653, 376, 308),
                sf::IntRect(379, 775, 299, 191),
                sf::IntRect(65, 805, 224, 143)
            };
            
            for (int i = 0; i < 30; ++i)
            {
                CloudEntity c;
                c.textureRect = cloudRects[rand() % cloudRects.size()];
                c.worldPosition = sf::Vector2f(
                    static_cast<float>(rand() % 15000) - 2000.f, // Spread from X=-2000 to X=13000
                    static_cast<float>(20 + (rand() % 400)) // Y between 20 and 420
                );
                c.scale = 0.3f + static_cast<float>(rand() % 50) / 100.f; // Scale 0.3 to 0.8
                c.driftSpeed = 5.f + static_cast<float>(rand() % 15); // Drift speed 5 to 20
                c.parallaxFactor = 0.5f + static_cast<float>(rand() % 30) / 100.f; // Parallax 0.5 to 0.8
                m_clouds.push_back(c);
            }
        }
    }
}

void PlayState::updateBackgroundLayers(
    sf::Time timePerFrame,
    const sf::View& view
)
{
    m_backgroundAnimationTime +=
        timePerFrame.asSeconds();

    const sf::Vector2f viewSize =
        view.getSize();

    const sf::Vector2f viewCenter =
        view.getCenter();

    const sf::FloatRect worldBounds =
        m_tileMap.worldBounds();

    const float cameraLeft =
        viewCenter.x - viewSize.x * 0.5f;

    const float availableTravel =
        std::max(
            1.f,
            worldBounds.width - viewSize.x
        );

    const float cameraProgress =
        std::clamp(
            (cameraLeft - worldBounds.left) /
                availableTravel,
            0.f,
            1.f
        );

    for (BackgroundLayer& layer :
         m_backgroundLayers)
    {
        if (!layer.texture)
        {
            continue;
        }

        const sf::Vector2u textureSize =
            layer.texture->getSize();

        if (
            textureSize.x == 0 ||
            textureSize.y == 0
        )
        {
            continue;
        }

        const float scaleX =
            viewSize.x /
            static_cast<float>(textureSize.x);

        const float scaleY =
            viewSize.y /
            static_cast<float>(textureSize.y);

        // Khớp chiều cao của background với chiều cao cửa sổ.
        const float scale = scaleY;

        layer.sprite.setScale(
            scale,
            scale
        );

        const float scaledWidth =
            static_cast<float>(textureSize.x)
            * scale;

        const float scaledHeight =
            static_cast<float>(textureSize.y)
            * scale;

        const float overflowX =
            std::max(
                0.f,
                scaledWidth - viewSize.x
            );

        // Camera trái:
        // ảnh hơi dịch sang phải.
        //
        // Camera phải:
        // ảnh hơi dịch sang trái.
        const float parallaxOffset =
            (0.5f - cameraProgress)
            * overflowX
            * layer.parallaxFactor;

        // Chuyển động nhẹ độc lập với camera.
        const float driftOffset =
            std::sin(
                m_backgroundAnimationTime
                * layer.driftSpeed
            )
            * std::min(
                layer.driftAmplitude,
                overflowX * 0.25f
            );

        layer.sprite.setPosition(
            viewCenter.x
                - scaledWidth * 0.5f
                + parallaxOffset
                + driftOffset,

            viewCenter.y
                - scaledHeight * 0.5f
        );
    }

    // Update dynamic clouds position
    for (auto& c : m_clouds) {
        c.worldPosition.x -= c.driftSpeed * timePerFrame.asSeconds();
        if (c.worldPosition.x < -2000.f) {
            c.worldPosition.x += 15000.f;
        }
    }
}

void PlayState::renderBackgroundLayers(
    sf::RenderWindow& window
)
{
    // Draw the main parallax background
    m_parallaxBg.update(window.getView(), 0.5f);
    m_parallaxBg.render(window);

    // Render original background layers (if any are still left)
    for (const auto& layer : m_backgroundLayers)
    {
        if (layer.texture)
        {
            window.draw(layer.sprite);
        }
    }
    
    // Draw dynamic clouds (nếu có)
    if (m_cloudTexture && !m_clouds.empty())
    {
        sf::Sprite cloudSprite(*m_cloudTexture);
        cloudSprite.setColor(sf::Color(255, 255, 255, 255)); // Restore natural brightness
        const sf::View& view = window.getView();
        const float cameraLeft = view.getCenter().x - view.getSize().x * 0.5f;

        for (const CloudEntity& c : m_clouds)
        {
            cloudSprite.setTextureRect(c.textureRect);
            cloudSprite.setScale(c.scale, c.scale);
            
            // Calculate position in world space with parallax applied
            float parallaxWorldX = c.worldPosition.x + (cameraLeft * (1.f - c.parallaxFactor));
            cloudSprite.setPosition(parallaxWorldX, c.worldPosition.y);
            
            window.draw(cloudSprite);
        }
    }
    
    // Draw atmospheric fog overlay (chỉ cho màn Level 1 ngoài trời)
    const bool isCastle = (m_saveData.currentLevel == 3 || m_tileMap.data().difficulty == "Hard");
    const bool isLevel2 = (m_saveData.currentLevel == 2 || m_tileMap.data().difficulty == "Medium");
    if (!isCastle && !isLevel2) {
        sf::RectangleShape fogRect(window.getView().getSize());
        fogRect.setPosition(window.getView().getCenter() - window.getView().getSize() * 0.5f);
        fogRect.setFillColor(sf::Color(255, 255, 255, 70)); // White overlay (paler opacity)
        window.draw(fogRect);
    }
}