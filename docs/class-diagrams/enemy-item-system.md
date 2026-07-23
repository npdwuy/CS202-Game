# Enemy and Item Base System

## Class Diagram

```mermaid
classDiagram
    class Enemy {
        <<abstract>>
        +~Enemy()
        +Update(timePerFrame) void
        +Render(window) void
        +GetBounds() FloatRect
        +IsActive() bool
        +Deactivate() void
    }

    class Goomba {
        -Texture m_texture
        -Sprite m_sprite
        -float m_speed
        -float m_leftBoundary
        -float m_rightBoundary
        -float m_animationTime
        -int m_currentFrame
        -int m_direction
        -bool m_active
        +Goomba(position, speed, leftBoundary, rightBoundary)
        +Update(timePerFrame) void
        +Render(window) void
        +GetBounds() FloatRect
        +IsActive() bool
        +Deactivate() void
    }

    class Item {
        <<abstract>>
        +~Item()
        +Update(timePerFrame) void
        +Render(window) void
        +GetBounds() FloatRect
        +Collect() void
        +IsCollected() bool
    }

    class Coin {
        -Texture m_texture
        -Sprite m_sprite
        -float m_baseY
        -float m_animationTime
        -int m_value
        -bool m_collected
        +Coin(position, value)
        +Update(timePerFrame) void
        +Render(window) void
        +GetBounds() FloatRect
        +Collect() void
        +IsCollected() bool
        +GetValue() int
    }

    class PlayState {
        -vector~unique_ptr~Enemy~~ m_enemies
        -vector~unique_ptr~Item~~ m_items
        +PlayState()
        +Input(event) void
        +Update(timePerFrame) void
        +Render(window) void
    }

    Enemy <|-- Goomba
    Item <|-- Coin

    PlayState *-- Enemy : owns
    PlayState *-- Item : owns
```

## Design Summary

`Enemy` and `Item` are abstract base classes that define common interfaces for all enemies and collectible items.

`Goomba` implements the `Enemy` interface. It moves horizontally within configured patrol boundaries and uses a two-frame walking animation.

`Coin` implements the `Item` interface. It has a collection state, point value, sprite rendering, and a floating idle animation.

`PlayState` stores enemies and items through `std::unique_ptr` collections. Update and render operations are called through base-class pointers, demonstrating runtime polymorphism.

Inactive enemies and collected items are removed safely from their corresponding vectors using `std::remove_if`.

## Enemy Movement Strategy

The enemy system applies the Strategy Pattern to separate movement behavior
from concrete enemy classes.

```mermaid
classDiagram
    class Enemy {
        <<interface>>
        +Update(timePerFrame)
        +Render(window)
        +GetBounds()
        +IsActive()
        +Deactivate()
    }

    class MovementStrategy {
        <<interface>>
        +Update(sprite, speed, timePerFrame)
    }

    class PatrolStrategy {
        -float leftBoundary
        -float rightBoundary
        -int direction
        +Update(sprite, speed, timePerFrame)
    }

    class FlyingStrategy {
        -float topBoundary
        -float bottomBoundary
        -int direction
        +Update(sprite, speed, timePerFrame)
    }

    class Goomba
    class Koopa
    class FlyingEnemy

    Enemy <|.. Goomba
    Enemy <|.. Koopa
    Enemy <|.. FlyingEnemy

    MovementStrategy <|.. PatrolStrategy
    MovementStrategy <|.. FlyingStrategy

    Goomba *-- MovementStrategy
    Koopa *-- MovementStrategy
    FlyingEnemy *-- MovementStrategy
```

### Current Enemy Behaviors

- `Goomba` uses `PatrolStrategy` and moves horizontally.
- `Koopa` uses `PatrolStrategy` but moves more slowly than Goomba.
- `FlyingEnemy` uses `FlyingStrategy` and moves vertically.
- Movement behavior is injected through `std::unique_ptr<MovementStrategy>`.
- Concrete enemy classes do not contain patrol boundary logic directly.

### Strategy Pattern Purpose

The Strategy Pattern allows each enemy to use a reusable movement algorithm.
New movement strategies can be added without changing the `Enemy` interface or
rewriting existing enemy classes.

For example:

- Ground enemies can use `PatrolStrategy`.
- Flying enemies can use `FlyingStrategy`.
- Future enemies may use `ChaseStrategy` or `BossStrategy`.