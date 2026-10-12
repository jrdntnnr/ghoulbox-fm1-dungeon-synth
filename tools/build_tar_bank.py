#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Build the optional CC BY Persian tar bank (numpy, ffmpeg required).

Sources: assets/desert/tar-sources.json. Download each public HLS preview with
  ffmpeg -i PREVIEW -c:a copy -f mpegts SOURCE_FILE
Then run: python3 tools/build_tar_bank.py build/tar-sources
The source_file hashes verify the downloaded streams before decoding.
Existing Oud/Qanun/Ney banks are preserved. No hardware writes.
"""
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import wave

import numpy as np
from build_desert_banks import OUT, pitch_frames
from sampleio import read_any_wav, user_slot, SLOT_RATE, SLOT_MAX_DATA


def main(directory):
    directory = Path(directory)
    roots = {'tar-C3': 48, 'tar-C4': 60, 'tar-Ab4': 68, 'tar-Db5': 73}
    zones, files = [], []
    for source in json.loads((OUT / 'tar-sources.json').read_text()):
        original = directory / source['source_file']
        if hashlib.sha256(original.read_bytes()).hexdigest() != source['sha256']:
            raise ValueError(f"Source checksum mismatch: {original}")
        decoded = directory / (source['id'] + '.wav')
        subprocess.run(['ffmpeg', '-v', 'error', '-y', '-i', str(original),
                        '-ac', '1', '-ar', str(SLOT_RATE), str(decoded)], check=True)
        sr, x = read_any_wav(decoded)
        x = np.array(x)
        start = max(0, int(np.flatnonzero(abs(x) > .04 * np.max(abs(x)))[0]) - 44)
        z = x[start:start + round(1.7 * sr)]
        z -= np.mean(z)
        root = roots[source['id']]
        cents = float(np.median(pitch_frames(z, root)))
        if abs(cents) > 40:
            raise ValueError(f"Unexpected source pitch: {source['id']}: {cents} cents")
        original_note = z.copy()
        # Re-measure after conversion; correct interpolation / detector bias
        # from the original each time, avoiding repeated resampling losses.
        for _ in range(4):
            step = 2 ** (-cents / 1200)
            z = np.interp(np.arange(0, len(original_note) - 1, step), np.arange(len(original_note)), original_note)
            correction = float(np.median(pitch_frames(z, root)))
            if abs(correction) < .5:
                break
            cents += correction
        fade = round(.002 * sr)
        z[:fade] *= np.linspace(0, 1, fade)
        fade = round(.15 * sr)
        z[-fade:] *= np.linspace(1, 0, fade)
        z = np.rint(z * (28000 / np.max(abs(z)))).astype('<i2')
        residual = pitch_frames(z.astype(float), root)
        if abs(float(np.median(residual))) >= 3 or float(np.ptp(residual)) >= 20:
            raise ValueError(f"Unstable calibrated note: {source['id']}: {residual}")
        name = source['id'] + '.wav'
        with wave.open(str(OUT / name), 'wb') as wav:
            wav.setparams((1, 2, sr, len(z), 'NONE', 'not compressed'))
            wav.writeframes(z.tobytes())
        files.append(dict(file=name, root=root, source=source['id'], start=start/sr,
                          frames=len(z), calibration_cents=round(-cents, 3),
                          median_error_cents=round(float(np.median(residual)), 3),
                          natural_pitch_span_cents=round(float(np.ptp(residual)), 2),
                          sha256=hashlib.sha256((OUT / name).read_bytes()).hexdigest()))
        zones.append((z.tolist(), root, None, None))
    header, data = user_slot('TAR', zones)
    assert len(data) <= SLOT_MAX_DATA
    (OUT / 'tar.hdr').write_bytes(header)
    (OUT / 'tar.bin').write_bytes(data)
    bank = dict(name='TAR', slot=None, files=files, bytes=len(data), header='tar.hdr', data='tar.bin')
    banks = json.loads((OUT / 'banks.json').read_text())
    banks = [b for b in banks if b['name'] != 'TAR'] + [bank]
    (OUT / 'banks.json').write_text(json.dumps(banks, indent=2) + '\n')
    print(f'TAR: {len(zones)} zones, {len(data)} / {SLOT_MAX_DATA} bytes')


if __name__ == '__main__':
    main(sys.argv[1])
