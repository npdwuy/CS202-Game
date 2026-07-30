\# Technical Progress Report: Character Movement \& Animation System



\## 1. Executive Summary

This report outlines the successful implementation of a robust 2D character movement and animation system using C++ and the SFML library. The recent development cycle established a strong, scalable class hierarchy defining core entity properties, physics, advanced platformer input mechanics, and state-driven sprite animations. 



\## 2. Architecture Overview

The system relies heavily on an \*\*Object-Oriented Programming (OOP)\*\* design pattern, specifically utilizing deep inheritance to separate concerns across different layers of abstraction. 



The directory structure strictly separates interfaces from implementations:

\*   \*\*Headers:\*\* `include/entities/player/` (.hpp files)

\*   \*\*Implementations:\*\* `src/entities/player/` (.cpp files)



\## 3. Class Diagram 





```mermaid

classDiagram

&#x20;   class Entity {

&#x20;       <<abstract>>

&#x20;       + Render(window: sf::RenderWindow)\* void

&#x20;       + update(timePerFrame: sf::Time)\* void

&#x20;       + position() sf::Vector2f

&#x20;       + velocity() sf::Vector2f

&#x20;       + setPosition(position: sf::Vector2f) void

&#x20;       + setVelocity(velocity: sf::Vector2f) void

&#x20;   }



&#x20;   class Character {

&#x20;       + onGround() bool

&#x20;       + setOnGround(onGround: bool) void

&#x20;       + facing() int

&#x20;       + setFacing(facing: int) void

&#x20;       + moveCharacter(dt: sf::Time) void

&#x20;   }



&#x20;   class Player {

&#x20;       + update(timePerFrame: sf::Time) void

&#x20;       + moveLeft() void

&#x20;       + moveRight() void

&#x20;       + jump() void

&#x20;   }



&#x20;   class Mario {

&#x20;       + Render(window: sf::RenderWindow) void

&#x20;       + update(timePerFrame: sf::Time) void

&#x20;   }



&#x20;   Entity <|-- Character

&#x20;   Character <|-- Player

&#x20;   Player <|-- Mario

```



\## 4. Key Implementations



\*   \*\*Physics \& Movement Mechanics:\*\*

&#x20;   \*   \*\*Gravity \& Kinematics:\*\* Integrated semi-implicit Euler integration for velocity and position updates in `Character::moveCharacter`. Applied a dynamic gravity scale (`0.88f` vs `1.0f`) to create a heavier "fall" feel compared to the upward jump arc.

&#x20;   \*   \*\*Friction:\*\* Applied horizontal damping (`velocity\_.x \*= 0.78f`) when no movement keys are pressed, allowing the character to slide smoothly to a halt.

\*   \*\*Advanced Player Inputs (Game Feel):\*\*

&#x20;   \*   \*\*Coyote Time:\*\* Implemented a `0.12f` second window allowing the player to execute a jump shortly after leaving a ledge, improving responsiveness.

&#x20;   \*   \*\*Jump Buffering:\*\* Buffered jump inputs for `0.14f` seconds, ensuring that jumps inputted just before hitting the ground are registered and executed immediately upon landing.

\*   \*\*State Machine Management:\*\*

&#x20;   \*   Created a finite state machine using an `enum class State` consisting of `Stand`, `Walk`, `Jump`, `Fall`, `HitRoof`, and `TransitionStand`.

&#x20;   \*   State transitions are dynamically calculated per frame based on velocity vectors and flags (e.g., `hitRoof\_`).

\*   \*\*Animation \& Rendering:\*\*

&#x20;   \*   \*\*Texture Processing:\*\* Employed `sf::Image::createMaskFromColor` at instantiation to programmatically strip the white background (`sf::Color::White`) from the sprite sheet.

&#x20;   \*   \*\*Frame Data:\*\* Mapped animation sequences (Stand, Walk, Jump, Fall, HitRoof) using vectors of `sf::IntRect` to slice the sprite sheet accurately.

&#x20;   \*   \*\*Sprite Flipping:\*\* Dynamically adjust the sprite's scale (`1.5f` or `-1.5f`) and origin based on horizontal velocity to face the character in the direction of movement.

