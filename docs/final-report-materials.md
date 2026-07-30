# Final report materials

## Member 4 contribution summary

Member 4 implemented a validated external level format, tile rendering and
collision, three difficulty-graded levels, map-driven factories, game
progression, a versioned save/load system, cached SFML resources, music and sound
effects, automated data tests, and the project documentation/demo plan.

## Evidence and screenshot checklist

Capture these images from the final merged build and place them in the report:

1. Main menu with Play and Load buttons.
2. Level 1 showing flat terrain, HUD, coins, and an enemy.
3. Level 2 showing at least one pit and elevated platform.
4. Level 3 showing the boss and exit in the same frame.
5. Options screen showing BGM and SFX controls.
6. `savegame.txt` beside an in-game screenshot at the saved position.
7. Game-over prompt.
8. Victory prompt after the boss is defeated.
9. Rendered Member 4 class diagram.
10. Console or IDE test output showing `All Member 4 data tests passed.`

Use native screenshots without cropping out the HUD, window title, or relevant
objects. Add a one-sentence caption explaining what each image proves.

## Functional test matrix

| ID | Test | Expected result |
|---|---|---|
| M4-01 | Launch Play | Level 1 loads from `levels/level1.txt` |
| M4-02 | Change a row width | Loader rejects the malformed level |
| M4-03 | Walk/jump into blocks | Player cannot pass through solid tiles |
| M4-04 | Fall into Level 2 pit | One life is removed and level restarts |
| M4-05 | Collect coin | Coin disappears, score rises, coin SFX plays |
| M4-06 | Collect M/F | Power state and score update, power-up SFX plays |
| M4-07 | Stomp enemy | Enemy deactivates and enemy SFX plays |
| M4-08 | Touch enemy from side | Life is removed |
| M4-09 | Reach Level 1/2 exit | Next level loads and progress autosaves |
| M4-10 | Reach Level 3 exit before boss | Exit remains locked |
| M4-11 | Defeat boss and exit | Victory prompt appears |
| M4-12 | Press F5 then F9 | Saved data and position are restored |
| M4-13 | Corrupt save version | Loader rejects save without crashing |
| M4-14 | Adjust BGM/SFX before Play | Gameplay audio uses chosen volumes |

## Diagrams collected

- `docs/class-diagrams/player.md`
- `docs/class-diagrams/enemy-item-system.md`
- `docs/class-diagrams/member4-systems.md`

Before submission, export the Mermaid diagrams to PNG or SVG if the report
format cannot render Mermaid directly.

## Design-pattern explanation for the report

- **State:** each menu/game overlay implements `GameState`.
- **Factory:** symbols become polymorphic enemies/items without parser coupling.
- **Strategy:** enemy movement behavior is supplied independently.
- **Singleton:** shared managers have one application-wide instance.
- **Resource cache:** SFML resources are loaded once and reused.
- **DTO:** `LevelData`, `LevelSpawnRequest`, and `SaveData` carry validated data.
- **Dependency injection:** `Character` receives a map collision resolver.

## Final merge checklist

- Merge Member 3's power-up changes and update only the `M`/`F` factory cases if
  the concrete class names differ.
- Confirm all branches use the same `Item` interface before resolving conflicts.
- Build Release with the team's chosen Windows compiler.
- Run CTest and the 14 manual cases above.
- Replace placeholder report screenshots with final merged-build captures.
- Verify font and sprite licences in `assets/CREDITS.MD`.
- Record the demo using `docs/demo-video-outline.md`.
