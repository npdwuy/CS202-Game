# Super Mario Flashback

Super Mario Flashback is Group 5's CS202 object-oriented programming project. It is a C++17 and SFML platform game built around state, strategy, factory, observer, and resource-management patterns.

## Team

| No. | Student ID | Full name | Group | Role |
|---:|:---:|---|:---:|---|
| 8 | 25125015 | Nguyen Pham Duc Huy | 5 | Core Systems, Architecture, Persistence, Procedural Generation |
| 9 | 25125018 | Nguyen Minh Khang | 5 | Level Design, Castle Sequence, Physics Edge Cases |
| 16 | 25125034 | Tran Quyet Thang | 5 | Enemy AI, Boss System, Tile Collision, Audio |
| 22 | 25125052 | Vo Thanh Dat | 5 | Koopa Shell Mechanics, Menu UI, Enemy Refactoring |

## Current Features

- **Game Flow**: Menu, options, character select, about, pause, gameplay, game-over, and victory states.
- **Player Mechanics**: Mario & Luigi with Coyote time, jump buffering, variable jump height, and pipe-warping.
- **Power-ups**: 3-state system (Small -> Big -> Fire) with animations and Fireball projectiles.
- **Enemies**: Goomba, Koopa (with kickable shell mechanics), Flying Enemy, HammerBro, and Bowser (Boss).
- **Levels**: Three handcrafted text-file levels + one procedurally generated Level 4 + hidden pipe sub-levels.
- **Engine**: AABB Two-pass tile collision, smooth clamped camera, and procedural colored terrain rendering.
- **Audio & Visuals**: Parallax backgrounds, batched vertex arrays, and custom synthesized chiptune SFX.
- **Architecture**: Decoupled systems using State, Strategy, Factory, Observer, Command, and Singleton patterns.
- **Persistence**: Safe atomic-write versioned save/load files, persisting position, score, and power-up state.

## Build and Run

The repository includes SFML 2 binaries for Visual Studio and MinGW on Windows.

### Visual Studio

```powershell
cmake -S . -B build
cmake --build build --config Release
.\build\Release\CS202-Group5.exe
```

### MinGW

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
.\build\CS202-Group5.exe
```

CMake copies `assets/`, `levels/`, and the required SFML runtime DLLs beside the executable after a successful build. Run the executable from that directory so relative resource paths resolve correctly.

## Controls

| Input | Action |
|---|---|
| A / D (rebindable) | Move left / right |
| W or Space (rebindable)| Jump / Throw Fireball |
| Down Arrow | Enter pipe |
| Escape | Pause game |
| F5 | Save the current game |
| F9 | Load `savegame.txt` |
| R | Restart after game over or victory |
| Enter | Return to menu |

## Levels and Procedural Generation

| Level | Difficulty | Theme & Content |
|---|---|---|
| Level 1: Green Hill Start | Easy | Flat ground, safe platforms, Goombas. Teaches movement. |
| Level 2: Broken Bridge Run | Medium | Pits, tighter platforms, Koopas, and Flying enemies. |
| Level 3: Bowser's Last Stand | Hard | Castle theme, alternating pits, HammerBros, Boss-gated exit. |
| Level 4: Procedural Mode | Random | Dynamically generated platforms, colors, and enemy density. |
| Pipe Sub-levels | N/A | Underground coin-rooms generated on the fly when entering pipes. |

## External Map Format & Factories

Levels are loaded via text files in `levels/`. `LevelLoader` strictly validates the map structure and delegates construction to `LevelObjectFactory`.

| Symbol | Object Created | Factory System |
|:---:|---|---|
| `#` / `.`| Solid Ground / Empty Space | `TileMap` |
| `P` / `X`| Player Start / Level Exit | `PlayState` |
| `G` | Goomba | `EnemyFactory` |
| `K` | Koopa (with shell mechanics) | `EnemyFactory` |
| `E` | Flying Enemy | `EnemyFactory` |
| `H` | HammerBro (throws projectiles) | `EnemyFactory` |
| `B` | Boss (Bowser, 5 HP) | `EnemyFactory` |
| `C` | Coin (+200 score) | `ItemFactory` |
| `M` / `F`| Mushroom / FireFlower | `ItemFactory` |
| `L` | 1-Up Mushroom (+1 Life) | `ItemFactory` |
| `S` | Star (8s Invincibility) | `ItemFactory` |
| `V` | SpeedBoost (8s 2x Speed) | `ItemFactory` |

## Core Design Patterns

- **State Pattern**: `GameManager` uses a state stack to seamlessly overlay `PauseState` over `PlayState`.
- **Strategy Pattern**: Enemies use `MovementStrategy` (`PatrolStrategy`, `FlyingStrategy`, `ChaseStrategy`, `BossChaseStrategy`) allowing dynamic behavior switching.
- **Factory Pattern**: Separates map parsing (`LevelLoader`) from object instantiation (`EnemyFactory`, `ItemFactory`).
- **Observer Pattern**: `GameEventManager` broadcasts events (`CoinCollected`, `EnemyDefeated`) to decouple game logic from audio and UI updates.
- **Template Method**: `FloatingItem` implements shared spawning animations for all power-ups.
- **Command Pattern**: UI buttons use `ICommand` to trigger actions dynamically without hardcoding logic.

## Validation & Testing

An automated CTest suite validates all levels and save/load integrity:

```powershell
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

## Documentation

- [CS202 Final Report](docs/CS202_Final_Report_Group5.md) - The complete documentation of features and patterns.
- `docs/diagrams/` - Contains PNG Mermaid class diagrams for all core architectures.
