#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Measure firmware-rendered fundamentals and simultaneous microtonal chords.

Requires NumPy. Run after sourced_synth_test; zero-spread single notes must be
within 3 cents, shipped beating courses within 8 cents at each chord tone.
"""
import sys
import wave
from pathlib import Path
import numpy as np


def spectrum(path):
    with wave.open(str(path)) as w:
        rate = w.getframerate()
        x = np.frombuffer(w.readframes(w.getnframes()), dtype="<i2")[::2].astype(float)
    x = x[int(.04 * rate):int(.65 * rate)]
    size = 1 << 20
    return abs(np.fft.rfft(x * np.hanning(len(x)), size)), rate / size


def pitch_error(spec, spacing, note):
    cents = -37 if note % 12 == 4 else -50 if note % 12 == 11 else 0
    expected = 440 * 2 ** ((note - 69 + cents / 100) / 12)
    lo, hi = int(expected * .96 / spacing), int(expected * 1.04 / spacing)
    k = lo + np.argmax(spec[lo:hi])
    assert lo < k < hi - 1 and spec[k] > 100, (note, "no measurable fundamental")
    a, b, c = np.log(spec[k-1:k+2] + 1e-15)
    delta = .5 * (a - c) / (a - 2 * b + c)
    return 1200 * np.log2((k + delta) * spacing / expected)


directory = Path(sys.argv[1])
worst = 0
for instrument in range(4):
    for note in (48, 52, 60, 64, 71, 76, 84):
        spec, spacing = spectrum(directory / f"{instrument}_note_{note}.wav")
        error = pitch_error(spec, spacing, note)
        assert abs(error) < 3, (instrument, note, error)
        worst = max(worst, abs(error))
    spec, spacing = spectrum(directory / f"{instrument}_chord.wav")
    errors = [pitch_error(spec, spacing, note) for note in (60, 64, 67)]
    assert max(map(abs, errors)) < 8, (instrument, errors)
    print(f"instrument {instrument}: simultaneous Rast C/E-37c/G errors {errors}")
print(f"28 single-note audio checks C3–C6: worst {worst:.3f} cents; all four microtonal chords pass")
