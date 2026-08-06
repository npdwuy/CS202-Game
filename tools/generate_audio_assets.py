from __future__ import annotations

import math
import struct
import wave
from pathlib import Path


SAMPLE_RATE = 44100
OUTPUT_DIRECTORY = Path(__file__).resolve().parents[1] / "assets" / "audio"


def square_wave(phase: float) -> float:
    return 1.0 if math.sin(phase) >= 0.0 else -1.0


def triangle_wave(phase: float) -> float:
    return 2.0 / math.pi * math.asin(math.sin(phase))


def oscillator(waveform: str, phase: float) -> float:
    if waveform == "square":
        return square_wave(phase)
    if waveform == "triangle":
        return triangle_wave(phase)
    return math.sin(phase)


def render_sequence(
    notes: list[tuple[float, float, float]],
    waveform: str = "square",
) -> list[float]:
    samples: list[float] = []

    for frequency, duration, volume in notes:
        sample_count = max(1, int(SAMPLE_RATE * duration))
        attack_count = max(1, min(int(SAMPLE_RATE * 0.008), sample_count // 3))
        release_count = max(1, min(int(SAMPLE_RATE * 0.025), sample_count // 3))

        for index in range(sample_count):
            if frequency <= 0.0:
                value = 0.0
            else:
                phase = 2.0 * math.pi * frequency * index / SAMPLE_RATE
                value = oscillator(waveform, phase)

            envelope = 1.0
            if index < attack_count:
                progress = index / attack_count
                envelope *= math.sin(progress * math.pi / 2.0) ** 2
            if index >= sample_count - release_count:
                progress = (sample_count - index - 1) / release_count
                envelope *= math.sin(max(0.0, progress) * math.pi / 2.0) ** 2

            samples.append(value * volume * envelope)

    return samples


def mix_tracks(*tracks: list[float]) -> list[float]:
    length = max((len(track) for track in tracks), default=0)
    mixed = [0.0] * length
    for track in tracks:
        for index, sample in enumerate(track):
            mixed[index] += sample
    return mixed


def write_samples(filename: str, samples: list[float]) -> None:
    peak = max((abs(sample) for sample in samples), default=1.0)
    gain = min(1.0, 0.92 / peak) if peak > 0.0 else 1.0
    encoded = [
        int(max(-1.0, min(1.0, sample * gain)) * 32767)
        for sample in samples
    ]

    OUTPUT_DIRECTORY.mkdir(parents=True, exist_ok=True)
    with wave.open(str(OUTPUT_DIRECTORY / filename), "wb") as output:
        output.setnchannels(1)
        output.setsampwidth(2)
        output.setframerate(SAMPLE_RATE)
        output.writeframes(
            b"".join(struct.pack("<h", sample) for sample in encoded)
        )


def write_sequence(
    filename: str,
    notes: list[tuple[float, float, float]],
    waveform: str = "square",
) -> None:
    write_samples(filename, render_sequence(notes, waveform))


def main() -> None:
    write_sequence(
        "jump.wav",
        [(330.0, 0.06, 0.26), (440.0, 0.06, 0.28), (660.0, 0.09, 0.25)],
        waveform="triangle",
    )
    write_sequence(
        "coin.wav",
        [(988.0, 0.07, 0.22), (1319.0, 0.12, 0.22)],
        waveform="triangle",
    )
    write_sequence(
        "power_up.wav",
        [
            (392.0, 0.07, 0.22),
            (523.0, 0.07, 0.22),
            (659.0, 0.07, 0.22),
            (784.0, 0.16, 0.24),
        ],
        waveform="triangle",
    )
    write_sequence(
        "one_up.wav",
        [
            (523.0, 0.07, 0.20),
            (659.0, 0.07, 0.21),
            (784.0, 0.07, 0.22),
            (1047.0, 0.18, 0.24),
        ],
        waveform="triangle",
    )
    write_sequence(
        "invincibility.wav",
        [
            (784.0, 0.05, 0.16),
            (988.0, 0.05, 0.17),
            (1175.0, 0.05, 0.18),
            (1319.0, 0.05, 0.18),
            (1568.0, 0.14, 0.20),
        ],
    )
    write_sequence(
        "speed_boost.wav",
        [
            (330.0, 0.045, 0.18),
            (440.0, 0.045, 0.18),
            (587.0, 0.045, 0.19),
            (784.0, 0.045, 0.20),
            (1047.0, 0.12, 0.21),
        ],
        waveform="triangle",
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
