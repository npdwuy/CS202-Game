# Member 4 design documentation

## Scope

Member 4 owns external level data, map rendering and collision, resource and
audio management, gameplay persistence, and final-project documentation. The
implementation deliberately keeps parsing, object creation, gameplay rules, and
file I/O in separate classes so each area can be changed independently.

## Runtime flow

1. `PlayState` selects `levels/levelN.txt`.
2. `TileMap` asks `LevelLoader` to parse and validate the file.
3. `LevelLoader` returns tiles, the player start, exit position, and neutral
   `LevelSpawnRequest` records.
4. `PlayState` passes each request to `LevelObjectFactory`.
5. The factory creates current enemy/item implementations and returns them
   through `Enemy` or `Item` pointers.
6. During updates, `TileMap` resolves player collision, while `PlayState`
   processes item, enemy, pit, and exit interactions.

This is a two-stage loading design: parsing answers *what the file requests*;
the factory answers *which C++ class fulfils that request*.

## Map validation

`LevelLoader` performs validation before gameplay state is mutated:

- the file must be readable and contain `@map`;
- `tile_size` must be between 16 and 128;
- every row must have the same non-zero width;
- only `#.PCMFGKEBXLSV` are accepted;
- exactly one `P` and one `X` must exist.

Invalid files throw a descriptive exception containing the source path. This
prevents partially loaded maps and makes level-authoring mistakes visible
immediately.

## Tile collision

Tiles are represented by world-space rectangles. Character movement is resolved
one axis at a time:

1. Apply horizontal velocity.
2. Push the character outside any intersecting solid tile and clear horizontal
   velocity.
3. Apply vertical velocity.
4. Push the character above/below an intersecting tile, clear vertical velocity,
   and update `onGround` or `hitRoof`.

Axis separation avoids diagonal corner penetration and preserves the player's
existing coyote-time and jump-buffer logic. `Character` accepts an injected
collision resolver, so it remains usable in isolated tests with its former
fallback ground.

## Factory coordination with Member 3

`LevelLoader` has no includes for `Goomba`, `Koopa`, `Coin`, Mushroom, or
FireFlower. It emits only a symbol and world position. All concrete dependencies
are confined to `LevelObjectFactory`.

The item factory constructs dedicated classes for Mushroom, FireFlower, 1-Up,
Star, and SpeedBoost pickups. Floating collectables inherit their animation and
collection lifecycle from `FloatingItem`; a new effect only needs a concrete
visual/effect class and one registered map symbol:

```cpp
case 'S':
    return std::make_unique<Star>(position);
```

No `TileMap` change is required once the new symbol is registered with the
loader and factory.

## Resource and audio management

`ResourceManager` is a lazy cache keyed by file path. It owns textures, fonts,
and sound buffers for the lifetime of the application and returns stable
references. Repeated requests therefore avoid duplicate disk reads and keep
SFML resources alive as long as sprites, text, or sounds reference them.

`AudioManager` owns the streaming background `sf::Music` and pooled `sf::Sound`
voices. It reads BGM/SFX volume from `SettingsManager`, fades music changes over
time, and lets repeated effects overlap without restarting a single voice. An
audio-load failure disables audio with a console message instead of terminating
gameplay.

| Event | Audio file |
|---|---|
| Jump starts | `jump.wav` |
| Coin collected | `coin.wav` |
| Power-up collected | `power_up.wav` |
| Extra life collected | `one_up.wav` |
| Star collected | `invincibility.wav` |
| Speed boost collected | `speed_boost.wav` |
| Enemy/boss stomped | `enemy_defeated.wav` |
| Lives reach zero | `game_over.wav` |
| Gameplay active | `background.wav` |

All audio is generated locally by `tools/generate_audio_assets.py`.

## Save/load design

`SaveData` is a plain data-transfer object. `SaveManager` and `LoadManager`
separate writing and reading responsibilities.

The writer validates values, writes a `.tmp` file, flushes it, and only then
renames it to `savegame.txt`. A failed write therefore does not leave a
half-written primary save. The loader parses the complete file into temporary
storage, requires all fields, checks ranges and enumerated strings, and returns
`std::optional<SaveData>`.

```text
version=1
currentLevel=2
score=2400
remainingLives=2
selectedCharacter=Mario
hasPlayerPosition=true
playerX=144
playerY=288
powerUpState=FireFlower
```

The version field provides an explicit migration point if future members extend
the save schema.

## Design patterns

| Pattern | Classes | Purpose |
|---|---|---|
| State | `GameState`, menu/play/pause/options states | Separate screen behavior |
| Factory | `LevelObjectFactory` | Map symbols create polymorphic objects |
| Strategy | `MovementStrategy`, patrol/flying strategies | Change enemy movement independently |
| Singleton | `GameManager`, `ResourceManager`, `AudioManager` | One shared application service |
| Cache/Flyweight-like resource sharing | `ResourceManager` | Avoid duplicate heavy SFML resources |
| Data Transfer Object | `LevelData`, `LevelSpawnRequest`, `SaveData` | Move validated data between subsystems |
| Dependency injection | `Character::setCollisionResolver` | Decouple player movement from a concrete map |
| Template method | `FloatingItem` and concrete pickups | Share animation/collection flow while varying visuals and effects |

## Extension points

- Add more `FloatingItem` subclasses and register their level symbols.
- Add a Luigi class and select it using `SaveData::selectedCharacter`.
- Replace rectangle tiles with a texture atlas without changing level files.
- Add a camera and levels wider than 40 tiles.
- Introduce save-format version 2 for checkpoints or per-level collectibles.
