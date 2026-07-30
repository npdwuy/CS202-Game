from __future__ import annotations

import math
import struct
import wave
from pathlib import Path


SAMPLE_RATE = 22050
OUTPUT_DIRECTORY = Path(__file__).resolve().parents[1] / "assets" / "audio"


def square_wave(phase: float) -> float:
    return 1.0 if math.sin(phase) >= 0.0 else -1.0


def write_sequence(
    filename: str,
    notes: list[tuple[float, float, float]],
    waveform: str = "square",
) -> None:
    samples: list[int] = []

    for frequency, duration, volume in notes:
        sample_count = max(1, int(SAMPLE_RATE * duration))
        fade_count = max(1, min(int(SAMPLE_RATE * 0.012), sample_count // 3))

        for index in range(sample_count):
            if frequency <= 0.0:
                value = 0.0
            else:
                phase = 2.0 * math.pi * frequency * index / SAMPLE_RATE
                value = (
                    square_wave(phase)
                    if waveform == "square"
                    else math.sin(phase)
                )

            envelope = 1.0
            if index < fade_count:
                envelope *= index / fade_count
            if index >= sample_count - fade_count:
                envelope *= (sample_count - index - 1) / fade_count

            samples.append(
                int(max(-1.0, min(1.0, value * volume * envelope)) * 32767)
            )

    OUTPUT_DIRECTORY.mkdir(parents=True, exist_ok=True)
    with wave.open(str(OUTPUT_DIRECTORY / filename), "wb") as output:
        output.setnchannels(1)
        output.setsampwidth(2)
        output.setframerate(SAMPLE_RATE)
        output.writeframes(
            b"".join(struct.pack("<h", sample) for sample in samples)
        )


def main() -> None:
    write_sequence(
        "jump.wav",
        [(330.0, 0.06, 0.26), (440.0, 0.06, 0.28), (660.0, 0.09, 0.25)],
    )
    write_sequence(
        "coin.wav",
        [(988.0, 0.07, 0.22), (1319.0, 0.12, 0.22)],
    )
    write_sequence(
        "power_up.wav",
        [
            (392.0, 0.07, 0.22),
            (523.0, 0.07, 0.22),
            (659.0, 0.07, 0.22),
            (784.0, 0.16, 0.24),
        ],
    )
    write_sequence(
        "enemy_defeated.wav",
        [(220.0, 0.06, 0.25), (165.0, 0.06, 0.23), (110.0, 0.10, 0.20)],
    )
    write_sequence(
        "game_over.wav",
        [
            (392.0, 0.16, 0.22),
            (330.0, 0.16, 0.22),
            (262.0, 0.18, 0.22),
            (196.0, 0.35, 0.20),
        ],
        waveform="sine",
    )

    melody = [
        523.0, 659.0, 784.0, 659.0,
        587.0, 698.0, 880.0, 698.0,
        659.0, 784.0, 988.0, 784.0,
        587.0, 698.0, 880.0, 698.0,
        523.0, 659.0, 784.0, 659.0,
        494.0, 587.0, 698.0, 587.0,
        440.0, 523.0, 659.0, 523.0,
        392.0, 494.0, 587.0, 494.0,
    ]
    background_notes = [
        (frequency, 0.18, 0.10) for frequency in melody
    ]
    write_sequence("background.wav", background_notes)


if __name__ == "__main__":
    main()
