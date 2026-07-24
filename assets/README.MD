# Game Assets

This directory contains the visual and audio resources used in the Super Mario game project.

## Directory Structure

```text
assets/
├── README.md
├── CREDITS.md
└── sprites/
    └── raw/
        ├── mario_sheet.png
        ├── smw_enemies_sheet.png
        └── coin_sheet.png
```

## Raw Sprite Sheets

The `sprites/raw` directory stores the original sprite sheets before cropping, resizing, or extracting animation frames.

Current raw sprite sheets:

- `mario_sheet.png`: Mario character sprites.
- `smw_enemies_sheet.png`: Super Mario World-style enemy sprites.
- `coin_sheet.png`: Coin animation sprites.

## Processed Assets

Processed sprites will later be stored in separate directories:

```text
assets/sprites/player/
assets/sprites/enemies/
assets/sprites/items/
```

The original sprite sheets inside `sprites/raw` should not be modified directly.

Cropped, resized, or edited sprites should be saved as separate files.

## Usage

These assets are used only for a non-commercial educational project.

All original characters, visual designs, names, and related intellectual property belong to their respective copyright holders.