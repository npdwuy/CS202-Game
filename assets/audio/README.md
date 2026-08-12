# Audio assets

The WAV files in this directory are original synthesized chiptune tones generated
for this course project by `tools/generate_audio_assets.py`. They do not sample
or reproduce audio from a commercial Mario title.

Run the generator with:

```bash
python tools/generate_audio_assets.py
```

The game streams the layered `background.wav` theme and caches eight short
effects through `ResourceManager`. Each effect has a small playback pool so
rapid coins, jumps, and collisions can overlap without cutting one another off.

The generator renders at 44.1 kHz with smooth note envelopes, triangle/sine
layers, peak normalization, and distinct cues for 1-Up, Star, and Speed Boost.
