# Demo video outline

Target duration: 6–8 minutes.

## 1. Introduction — 0:00–0:30

- Show the title screen and team list.
- State the project goal: an OOP platform game using C++17 and SFML.
- Briefly name the State, Factory, Strategy, Singleton, and DTO patterns.

## 2. Menu and settings — 0:30–1:10

- Open Options and demonstrate BGM/SFX volume controls.
- Return to the menu.
- Point out the separate Play and Load entries.

## 3. Level 1 — 1:10–2:10

- Start a new game.
- Show the HUD: level, score, lives, character, power-up, save/load keys.
- Demonstrate tile collision, movement, jumping, coin collection, Mushroom
  collection, and Goomba/Koopa interaction.
- Let jump, coin, power-up, enemy, and background sounds be audible.
- Reach the exit.

## 4. Level 2 — 2:10–3:00

- Show pits and the life-loss/reset behavior.
- Cross the broken ground using platforms.
- Collect the FireFlower and show the HUD state changing.
- Reach the exit to demonstrate automatic progression and autosave.

## 5. Save/load — 3:00–3:45

- Press F5 at a recognizable position.
- Move elsewhere or lose a life.
- Press F9 and show the level, score, lives, power-up, and position restored.
- Return to the menu and use the Load button as a second loading path.

## 6. Level 3 and boss — 3:45–4:45

- Show the harder platform layout.
- Try the exit while the boss is active and show that it remains locked.
- Stomp the boss, hear the enemy effect, and use the now-open exit.
- Show the victory prompt.

## 7. Code and design — 4:45–6:30

- Open one level text file and explain the symbols.
- Show `LevelLoader` validation and `LevelSpawnRequest`.
- Show the `LevelObjectFactory` cases and Member 3 integration boundary.
- Show `TileMap::resolveCollision`.
- Show the versioned `savegame.txt`.
- Show `ResourceManager` and `AudioManager`.
- Display the Mermaid diagrams in `docs/class-diagrams/`.

## 8. Closing — 6:30–7:00

- Summarize each member's subsystem.
- Mention the automated data test and successful gameplay checklist.
- End on the victory or title screen.

## Recording notes

- Record at 1920×1080 or 1280×720, 30 FPS or higher.
- Capture game audio directly and keep microphone narration above it.
- Hide unrelated windows and notification pop-ups.
- Use short code zoom-ins rather than scrolling through entire files.
- Retake any segment where collision behavior or audio cannot be observed.
