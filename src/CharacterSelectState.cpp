#include "CharacterSelectState.hpp"
#include "GameManager.hpp"
#include "LevelSelectState.hpp"
#include "PlayState.hpp"
#include "commands/MenuCommands.hpp"
#include "resources/ResourceManager.hpp"

#include <stdexcept>

// ── Layout constants ─────────────────────────────────────────────────────────
static constexpr float kScreenW     = 1920.f;
static constexpr float kScreenH     = 1080.f;
static constexpr float kMarioX      = 620.f;
static constexpr float kLuigiX      = 1300.f;
static constexpr float kCharY       = 560.f;   // was 400 – moved down
static constexpr float kSpriteScale = 5.5f;
// ─────────────────────────────────────────────────────────────────────────────

CharacterSelectState::CharacterSelectState() {
    const std::string fontPath =
        "assets/fonts/ro-spritendo-font/RoSpritendoSemiboldBeta-vmVwZ.otf";

    m_font = ResourceManager::getInstance().getFont(fontPath);

    // ── Load Mario sprite ─────────────────────────────────────────────────────
    sf::Image marioImg;
    if (!marioImg.loadFromFile("assets/sprites/player/mario.png"))
        throw std::runtime_error("CharacterSelectState: failed to load mario.png");
    marioImg.createMaskFromColor(sf::Color::White);
    if (!m_marioTexture.loadFromImage(marioImg))
        throw std::runtime_error("CharacterSelectState: failed to create mario texture");

    m_marioSprite.setTexture(m_marioTexture);
    m_marioSprite.setTextureRect(sf::IntRect(160, 15, 35, 47));   // stand frame
    m_marioSprite.setOrigin(17.5f, 47.f);
    m_marioSprite.setScale(kSpriteScale, kSpriteScale);
    m_marioSprite.setPosition(kMarioX, kCharY);

    // ── Load Luigi sprite (fallback to Mario tinted green) ───────────────────
    sf::Image luigiImg;
    bool dedicatedLuigiLoaded = luigiImg.loadFromFile("assets/sprites/player/luigi.png");
    bool luigiLoaded = dedicatedLuigiLoaded;
    if (!luigiLoaded)
        luigiLoaded = luigiImg.loadFromFile("assets/sprites/player/mario.png");
    if (!luigiLoaded)
        throw std::runtime_error("CharacterSelectState: failed to load luigi or fallback");
    luigiImg.createMaskFromColor(sf::Color::White);
    if (!m_luigiTexture.loadFromImage(luigiImg))
        throw std::runtime_error("CharacterSelectState: failed to create luigi texture");

    m_luigiSprite.setTexture(m_luigiTexture);
    m_luigiSprite.setTextureRect(sf::IntRect(160, 15, 35, 47));
    m_luigiSprite.setOrigin(17.5f, 47.f);
    m_luigiSprite.setScale(kSpriteScale * 0.80f, kSpriteScale * 1.10f); // ốm đi 20%, cao hơn 10%
    m_luigiSprite.setPosition(kLuigiX, kCharY);
    if (!dedicatedLuigiLoaded) {
        m_luigiSprite.setColor(sf::Color(140, 230, 140));  // green tint if using Mario fallback
    }

    // ── Title ─────────────────────────────────────────────────────────────────
    m_titleText.setFont(m_font);
    m_titleText.setString("SELECT CHARACTER");
    m_titleText.setCharacterSize(60);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setOutlineColor(sf::Color(40, 40, 40));
    m_titleText.setOutlineThickness(3.f);
    {
        sf::FloatRect tb = m_titleText.getLocalBounds();
        m_titleText.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        m_titleText.setPosition(kScreenW / 2.f, 160.f);
    }

    // ── Character labels ──────────────────────────────────────────────────────
    auto makeLabel = [&](sf::Text& t, const std::string& str, float x) {
        t.setFont(m_font);
        t.setString(str);
        t.setCharacterSize(36);
        t.setFillColor(sf::Color::White);
        t.setOutlineColor(sf::Color(40, 40, 40));
        t.setOutlineThickness(2.f);
        sf::FloatRect lb = t.getLocalBounds();
        t.setOrigin(lb.left + lb.width / 2.f, lb.top + lb.height / 2.f);
        t.setPosition(x, kCharY + 20.f);  // below sprite
    };
    makeLabel(m_marioLabel, "MARIO",  kMarioX);
    makeLabel(m_luigiLabel, "LUIGI",  kLuigiX);

    // ── Hint text ─────────────────────────────────────────────────────────────
    m_hintText.setFont(m_font);
    m_hintText.setString("Arrow keys to select   |   Enter or click to confirm");
    m_hintText.setCharacterSize(24);
    m_hintText.setFillColor(sf::Color(200, 200, 200));
    {
        sf::FloatRect hb = m_hintText.getLocalBounds();
        m_hintText.setOrigin(hb.left + hb.width / 2.f, hb.top + hb.height / 2.f);
        m_hintText.setPosition(kScreenW / 2.f, kScreenH - 80.f);
    }

    // ── Highlight box ─────────────────────────────────────────────────────────
    m_highlight.setSize({220.f, 280.f});
    m_highlight.setOrigin(110.f, 280.f);
    m_highlight.setFillColor(sf::Color(255, 220, 0, 40));
    m_highlight.setOutlineColor(sf::Color(255, 220, 0));
    m_highlight.setOutlineThickness(4.f);

    // ── Confirm button ────────────────────────────────────────────────────────
    m_confirmButton = std::make_unique<Button>(
        "PLAY",
        m_font,
        sf::Vector2f(kScreenW / 2.f, 840.f),
        sf::Vector2f(300.f, 70.f),
        32
    );
    m_confirmButton->setColors(
        sf::Color(255, 160, 50, 220),   // normal  – orange
        sf::Color(255, 215, 0,  255),   // hover   – yellow
        sf::Color::White
    );
    m_confirmButton->setCommand(std::make_unique<LambdaCommand>([this]() {
        confirmSelection();
    }));
}

void CharacterSelectState::confirmSelection() {
    // Write selected character into a fresh SaveData and launch play
    GameManager::getInstance().getSettings().setSelectedCharacter(
        m_selected == 0 ? "Mario" : "Luigi"
    );
    GameManager::getInstance().changeState(
        std::make_unique<LevelSelectState>()
    );
}

void CharacterSelectState::Input(const sf::Event& event) {
    sf::RenderWindow& window = GameManager::getInstance().getWindow();

    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mp = window.mapPixelToCoords(
            sf::Vector2i(event.mouseMove.x, event.mouseMove.y),
            GameManager::getInstance().getGameView()
        );
        m_confirmButton->update(mp);

        // Click on Mario portrait → select Mario
        if (m_marioSprite.getGlobalBounds().contains(mp)) m_selected = 0;
        if (m_luigiSprite.getGlobalBounds().contains(mp)) m_selected = 1;
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        sf::Vector2f mp = window.mapPixelToCoords(
            sf::Vector2i(event.mouseButton.x, event.mouseButton.y),
            GameManager::getInstance().getGameView()
        );
        if (m_confirmButton->handleClick(event, mp)) return;
        // Click directly on portrait = instant confirm
        if (m_marioSprite.getGlobalBounds().contains(mp)) { m_selected = 0; confirmSelection(); return; }
        if (m_luigiSprite.getGlobalBounds().contains(mp)) { m_selected = 1; confirmSelection(); return; }
    }

    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::Left:
            case sf::Keyboard::A:
                m_selected = 0; break;

            case sf::Keyboard::Right:
            case sf::Keyboard::D:
                m_selected = 1; break;

            case sf::Keyboard::Return:
            case sf::Keyboard::Space:
                confirmSelection(); break;

            case sf::Keyboard::Escape:
                GameManager::getInstance().popState(); break;

            default: break;
        }
    }
}

void CharacterSelectState::Update(sf::Time /*timePerFrame*/) {}

void CharacterSelectState::Render(sf::RenderWindow& window) {
    window.setView(GameManager::getInstance().getGameView());

    // ── Dark background ───────────────────────────────────────────────────────
    sf::RectangleShape bg({kScreenW, kScreenH});
    bg.setFillColor(sf::Color(20, 30, 60, 230));
    bg.setPosition(0.f, 0.f);
    window.draw(bg);

    // ── Highlight around selected character ───────────────────────────────────
    float hx = (m_selected == 0) ? kMarioX : kLuigiX;
    m_highlight.setPosition(hx, kCharY);
    window.draw(m_highlight);

    // ── Character sprites ─────────────────────────────────────────────────────
    window.draw(m_marioSprite);
    window.draw(m_luigiSprite);

    // ── Labels ────────────────────────────────────────────────────────────────
    window.draw(m_marioLabel);
    window.draw(m_luigiLabel);

    // ── Title & hint ──────────────────────────────────────────────────────────
    window.draw(m_titleText);
    window.draw(m_hintText);

    // ── Confirm button ────────────────────────────────────────────────────────
    m_confirmButton->render(window);
}
