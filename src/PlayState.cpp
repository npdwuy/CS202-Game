#include "PlayState.hpp"
#include "MenuState.hpp"
#include "GameManager.hpp"
#include "PauseState.hpp"
#include "GameOverState.hpp"
#include "LevelCompleteState.hpp"
#include "commands/MenuCommands.hpp"

#include "audio/AudioManager.hpp"
#include "entities/enemies/BossEnemy.hpp"
#include "entities/player/Mario.hpp"
#include "entities/player/Entity.hpp"
#include "persistence/LoadManager.hpp"
#include "persistence/SaveManager.hpp"
#include "resources/ResourceManager.hpp"
#include "events/GameEventManager.hpp"

#include <filesystem>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

PlayState::PlayState(bool loadSavedGame)
    : m_hud(ResourceManager::getInstance().getFont(
          "assets/fonts/ro-spritendo-font/RoSpritendoSemiboldBeta-vmVwZ.otf"
      )) {
    loadBackgroundLayers();

    const sf::Font& hudFont = ResourceManager::getInstance().getFont(
        "assets/fonts/ro-spritendo-font/RoSpritendoSemiboldBeta-vmVwZ.otf"
    );

    if (!m_buttonTexture.loadFromFile("assets/sprites/button/btn_transparent.png")) {
        throw std::runtime_error("Failed to load assets/sprites/button/btn_transparent.png");
    }
    
    m_flagScoreText.setFont(hudFont);
    m_flagScoreText.setCharacterSize(16);
    m_flagScoreText.setFillColor(sf::Color::White);
    m_flagScoreText.setOutlineColor(sf::Color::Black);
    m_flagScoreText.setOutlineThickness(1.f);
    
    m_menuButton = std::make_unique<Button>("...", hudFont, m_buttonTexture, sf::Vector2f(24.f, 10.f), sf::Vector2f(40.f, 40.f), 24);
    m_menuButton->setColors(sf::Color::White, sf::Color(255, 230, 200, 255), sf::Color(245, 222, 179));
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
        loadLevel(1, false);
        showStatus(
            "Level 1 - Green Hill Start",
            2.5f
        );
    }

    updateHud();

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
            loseLife();
            break;
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
            if (m_player->position().y + m_player->height() >= m_tileMap.getPoleBottomY() - 1.f) {
                m_player->setPosition({m_player->position().x, m_tileMap.getPoleBottomY() - m_player->height()});
                m_player->forceState(Player::State::AutoWalk);
                m_player->setFacing(1);
                m_exitSequence = ExitSequence::WalkingRight;
            } else {
                sf::Vector2f pos = m_player->position();
                pos.y += 100.f * timePerFrame.asSeconds();
                m_player->setPosition(pos);
            }
            m_tileMap.updateFlagAnimation(timePerFrame, 200.f);
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
    updateTimedPowerUps(timePerFrame);
    updateLevelTimer(timePerFrame);
    m_hud.update(timePerFrame);
    
    if (m_player) {
        m_player->update(timePerFrame);
    }
    
    m_tileMap.update(timePerFrame);

    if (m_player->consumeJumpEvent()) {
        AudioManager::getInstance().playEffect(SoundEffect::Jump);
    }
    updateCamera(timePerFrame);

    for (auto& enemy : m_enemies) {
        enemy->Update(timePerFrame);
    }

    for (auto& item : m_items) {
        item->Update(timePerFrame);
    }

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

    if (m_player) {
        m_player->Render(window);
    }

    if (m_showFlagScore) {
        window.draw(m_flagScoreText);
    }

    if (m_levelStarting || m_exitSequence == ExitSequence::IrisWipe) {
        float rawProgress = 0.f;
        if (m_levelStarting) {
            rawProgress = std::clamp(m_startTimer / 1.0f, 0.0f, 1.0f); 
            // Cubic Ease-In for opening (mirroring closing wipe)
            rawProgress = std::pow(rawProgress, 3.0f);
        } else {
            rawProgress = std::clamp(1.0f - (m_exitTimer / 1.0f), 0.0f, 1.0f); 
            // Cubic Ease-In for closing
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
        m_menuButton->setPosition(sf::Vector2f(left + 24.f, top + 10.f));
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
    if (levelNumber < 1 || levelNumber > 3) {
        throw std::out_of_range("Level number must be between 1 and 3.");
    }

    handleLevelStart();

    m_saveData.currentLevel = levelNumber;
    m_playerDamagePending = false;
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

    m_player = std::make_unique<Mario>(spawnPosition);
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
                        if (m_invincibilityTimeRemaining > 0.f) {
                            // Star power: phá ngay 1 hit
                            m_tileMap.breakBlock(row, col);
                            AudioManager::getInstance().playEffect(SoundEffect::EnemyDefeated);
                        } else if (m_player->isSuper()) {
                            // Mushroom/Fire: cần 2 hit
                            bool destroyed = m_tileMap.hitBlock(row, col);
                            if (destroyed) {
                                AudioManager::getInstance().playEffect(SoundEffect::EnemyDefeated);
                            } else {
                                AudioManager::getInstance().playEffect(SoundEffect::Jump);
                            }
                        }
                    }
                }
            });

            // Constrain player position to camera's left edge
            const sf::FloatRect cameraBounds = m_camera.visibleBounds();
            sf::Vector2f position = character.position();
            if (position.x < cameraBounds.left) {
                position.x = cameraBounds.left;
                character.setPosition(position);

                sf::Vector2f velocity = character.velocity();
                if (velocity.x < 0.f) {
                    velocity.x = 0.f;
                    character.setVelocity(velocity);
                }
            }
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
                GameEventManager::GetInstance().Notify(
                    {
                        GameEventType::PowerUpCollected,
                        500,
                        "Mushroom"
                    }
                );
                m_player->getMushroom();
                break;

            case ItemEffectType::EnableFirePower:
                GameEventManager::GetInstance().Notify(
                    {
                        GameEventType::PowerUpCollected,
                        500,
                        "FireFlower"
                    }
                );
                m_player->up2Fire();
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

    for (auto& enemy : m_enemies)
    {
        if (
            !enemy->IsActive() ||
            !bounds.intersects(enemy->GetBounds())
        )
        {
            continue;
        }

        const sf::FloatRect enemyBounds =
            enemy->GetBounds();

        const bool stomped =
            m_player->velocity().y > 0.f &&
            bounds.top + bounds.height <=
                enemyBounds.top +
                enemyBounds.height * 0.65f;

        const bool isBoss =
            dynamic_cast<const BossEnemy*>(
                enemy.get()
            ) != nullptr;

        if (m_invincibilityTimeRemaining > 0.f)
        {
            enemy->Deactivate();
            GameEventManager::GetInstance().Notify(
                {
                    GameEventType::EnemyDefeated,
                    isBoss ? 2000 : 200,
                    isBoss
                        ? "Boss defeated - the exit is open!"
                        : "Enemy defeated by Star power"
                }
            );
            continue;
        }

        if (stomped)
        {
            enemy->Deactivate();

            sf::Vector2f velocity =
                m_player->velocity();

            velocity.y = -330.f;
            m_player->setVelocity(velocity);

            GameEventManager::GetInstance().Notify(
                {
                    GameEventType::EnemyDefeated,
                    isBoss ? 2000 : 200,
                    isBoss
                        ? "Boss defeated - the exit is open!"
                        : "Enemy defeated"
                }
            );
        }
        else
        {
            if (m_damageCooldown > 0.f) {
                continue;
            }

            sf::Vector2f knockback = m_player->velocity();
            const float playerCenter = bounds.left + bounds.width * 0.5f;
            const float enemyCenter = enemyBounds.left + enemyBounds.width * 0.5f;
            knockback.x = playerCenter < enemyCenter ? -240.f : 240.f;
            knockback.y = -320.f;
            m_player->setVelocity(knockback);

            GameEventManager::GetInstance().Notify(
                {
                    GameEventType::PlayerDamaged,
                    0,
                    ""
                }
            );

            return true;
        }
    }

    return false;
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

    if (m_saveData.powerUpState != "None") {
        m_saveData.powerUpState = "None";
        if (m_player) {
            m_player->shrinkPlayer();
        }
        m_damageCooldown = 1.5f;
        showStatus("Power-up absorbed the hit", 1.5f);
        updateHud();
        return;
    }

    loseLife();
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
    m_damageCooldown = 0.f;
    m_playerDamagePending = false;

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

    if (!playerBounds().intersects(m_tileMap.exitBounds())) {
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
        SaveManager::save(m_saveData);
        GameManager::getInstance().pushState(std::make_unique<LevelCompleteState>());
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
                dynamic_cast<const BossEnemy*>(enemy.get()) != nullptr
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

    // Base background - bắt buộc phải có.
    addBackgroundLayer(
        "assets/backgrounds/level_bg.png",
        0.20f,
        0.f,
        0.f,
        true
    );

    // Optional cloud layer.
    // Nếu chưa có file thì hệ thống tự bỏ qua.
    addBackgroundLayer(
        "assets/backgrounds/clouds.png",
        0.35f,
        35.f,
        0.35f,
        false
    );

    // Optional foreground layer.
    addBackgroundLayer(
        "assets/backgrounds/foreground.png",
        0.75f,
        8.f,
        0.20f,
        false
    );
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

        // Cover toàn màn hình nhưng không làm méo ảnh.
        const float scale =
            std::max(scaleX, scaleY);

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
}

void PlayState::renderBackgroundLayers(
    sf::RenderWindow& window
)
{
    for (const BackgroundLayer& layer :
         m_backgroundLayers)
    {
        if (layer.texture)
        {
            window.draw(layer.sprite);
        }
    }
}