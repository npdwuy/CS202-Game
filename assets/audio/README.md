# Audio assets

The WAV files in this directory are original synthesized chiptune tones generated
for this course project by `tools/generate_audio_assets.py`. They do not sample
or reproduce audio from a commercial Mario title.

Run the generator with:

```bash
python tools/generate_audio_assets.py
```

The game streams `background.wav` and caches the five short effects through
`ResourceManager`.
