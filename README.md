# CS202 - Programming System

## Group 5 - Course Project

### Project Details
* **Game Name:** Super Mario Flashback
* **Repository:** [GitHub Repository](https://github.com/npdwuy/CS202-Game.git)

### Team Members

| No. | Student ID | Full Name | Group |
| :---: | :---: | :--- | :---: |
| 8 | 25125015 | Nguyen Pham Duc Huy | 5 |
| 9 | 25125018 | Nguyen Minh Khang | 5 |
| 16 | 25125034 | Tran Quyet Thang | 5 |
| 22 | 25125052 | Vo Thanh Dat | 5 |

## Enemy and Item Base System

This milestone introduces the initial enemy and collectible item architecture.

### Implemented Features

- Abstract `Enemy` interface
- Abstract `Item` interface
- `Goomba` enemy implementation
- `Coin` item implementation
- Goomba horizontal patrol behavior
- Two-frame Goomba walking animation
- Coin floating idle animation
- Polymorphic enemy and item collections in `PlayState`
- Automatic removal of inactive enemies and collected items
- SFML runtime DLL setup for Windows

### Test Controls

After starting the game, press `Enter` to enter `PlayState`.

- Press `C` to collect the first Coin.
- Press `K` to deactivate the first Goomba.

These controls are temporary and will later be replaced by player collision handling.

### Project Structure

- `include/entities/Enemy.hpp`: Base enemy interface
- `include/entities/Item.hpp`: Base item interface
- `include/entities/enemies/Goomba.hpp`: Goomba declaration
- `include/entities/items/Coin.hpp`: Coin declaration
- `src/entities/enemies/Goomba.cpp`: Goomba implementation
- `src/entities/items/Coin.cpp`: Coin implementation
- `docs/class-diagrams/enemy-item-system.md`: Class diagram and design summary

### Object-Oriented Design

`Goomba` inherits from `Enemy`, while `Coin` inherits from `Item`.

`PlayState` owns enemies and items using `std::unique_ptr`. Update and render operations are executed through base-class pointers, demonstrating abstraction, inheritance, encapsulation, and runtime polymorphism.