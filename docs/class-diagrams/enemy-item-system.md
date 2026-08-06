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
        -Sprite m_sprite
        -unique_ptr~MovementStrategy~ m_movementStrategy
        -float m_speed
        -float m_animationTime
        -int m_currentFrame
        -bool m_active
        +Goomba(position, speed, movementStrategy)
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
        +GetEffect() ItemEffect
        +Collect() void
        +IsCollected() bool
    }

    class FloatingItem {
        -Vector2f m_basePosition
        -float m_floatingDistance
        -float m_floatingSpeed
        -bool m_collected
        +Update(timePerFrame) void
        +Collect() void
        +IsCollected() bool
        #SetVisualPosition(position) void
        #Animate(timePerFrame) void
    }

    class Coin {
        -Sprite m_sprite
        -int m_value
        +Coin(position, value)
        +Render(window) void
        +GetBounds() FloatRect
        +GetEffect() ItemEffect
        +GetValue() int
    }

    class Mushroom
    class FireFlower
    class OneUpMushroom
    class Star
    class SpeedBoost

    class PlayState {
        -vector~unique_ptr~Enemy~~ m_enemies
        -vector~unique_ptr~Item~~ m_items
        +PlayState()
        +Input(event) void
        +Update(timePerFrame) void
        +Render(window) void
    }

    Enemy <|-- Goomba
    Item <|-- FloatingItem
    FloatingItem <|-- Coin
    FloatingItem <|-- Mushroom
    FloatingItem <|-- FireFlower
    FloatingItem <|-- OneUpMushroom
    FloatingItem <|-- Star
    FloatingItem <|-- SpeedBoost

    PlayState *-- Enemy : owns
    PlayState *-- Item : owns
```

## Design Summary

`Enemy` and `Item` are abstract base classes that define common interfaces for all enemies and collectible items.

`Goomba` implements the `Enemy` interface. It moves horizontally within configured patrol boundaries and uses a two-frame walking animation.

`FloatingItem` applies the Template Method Pattern: it owns collection and bobbing
animation state while concrete items supply their visual positioning, rendering,
collision bounds, and `ItemEffect`. Coin, Mushroom, FireFlower, 1-Up, Star, and
SpeedBoost therefore share lifecycle code without losing runtime polymorphism.

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
