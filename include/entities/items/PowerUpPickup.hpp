#pragma once

#include "entities/Item.hpp"

#if __has_include("entities/items/ItemEffect.hpp")
#include "entities/items/ItemEffect.hpp"
#endif

enum class PowerUpKind {
    Mushroom,
    FireFlower
};

class PowerUpPickup : public Item {
public:
    PowerUpPickup(sf::Vector2f position, PowerUpKind kind);

    void Update(sf::Time timePerFrame) override;
    void Render(sf::RenderWindow& window) const override;
    sf::FloatRect GetBounds() const override;
    void Collect() override;
    bool IsCollected() const override;

#if __has_include("entities/items/ItemEffect.hpp")
    ItemEffect GetEffect() const override;
#endif

    PowerUpKind getKind() const;

private:
    sf::RectangleShape m_body;
    sf::CircleShape m_cap;
    PowerUpKind m_kind;
    float m_baseY;
    float m_animationTime = 0.f;
    bool m_collected = false;
};
