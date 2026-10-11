#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Rebuild the optional CC0 banks from verified public HQ previews.
Download URLs and SHA256s are in assets/desert/sources.json. Decode each MP3:
  ffmpeg -i NAME.mp3 -ac 1 -ar 22050 NAME.wav
Then: python3 tools/build_desert_banks.py build/desert-sources
Requires numpy. Does not download or write to hardware.
"""
import hashlib
import json
import math
from pathlib import Path
import sys
import wave
import numpy as np
from sampleio import read_any_wav, yin, user_slot, SLOT_RATE

ROOT = Path(__file__).resolve().parent.parent
OUT = ROOT / 'assets/desert'
# source, approximate search start, search end, root, duration. Plucks locate their
# attack within the search window. Ney extracts only a held tail, after ornaments.
CUTS = {
    'OUD': [('oud-g2', 0, 1.5, 43, 1.4), ('oud-a2', 0, 2.8, 45, 2.0), ('oud-c3', 0, 1.6, 48, 1.5)],
    'QANUN': [('qanun', 118.5, 120.3, 53, 1.05), ('qanun', 85.3, 87, 60, 1.05),
              ('qanun', 63.2, 65, 65, 1.05), ('qanun', 49.3, 51, 69, 1.05),
              ('qanun', 36, 38, 74, 1.05), ('qanun', 28.8, 30.5, 77, 1.05)],
    'NEY AIR': [('ney', 12.72, 13.4, 71, .68), ('ney', 8.65, 9.35, 72, .70), ('ney', 3.95, 4.6, 76, .65)],
}

def pitch_frames(x, root):
    hz = 440 * 2 ** ((root - 69) / 12)
    n = int(.10 * SLOT_RATE)
    result = []
    for a in range(int(.08 * SLOT_RATE), min(len(x)-n, int(.65*SLOT_RATE)), int(.04*SLOT_RATE)):
        f = yin(x[a:a+n:2].tolist(), SLOT_RATE/2, fmin=hz*.8, fmax=hz*1.2)
        result.append(1200 * math.log2(f / hz))
    return np.array(result)

def main(directory):
    directory = Path(directory)
    sources = json.loads((OUT/'sources.json').read_text())
    for source in sources:
        assert hashlib.sha256((directory/(source['id']+'.mp3')).read_bytes()).hexdigest() == source['sha256']
    banks = []
    for slot, (name, cuts) in enumerate(CUTS.items(), 1):
        zones, files = [], []
        for source, a, b, root, duration in cuts:
            sr, x = read_any_wav(directory/(source+'.wav'))
            assert sr == SLOT_RATE
            x = np.array(x)
            first, last = round(a*sr), round(b*sr)
            if source != 'ney':
                z = x[first:last]
                first += max(0, int(np.flatnonzero(abs(z) > .04*np.max(abs(z)))[0])-44)
            z = x[first:min(len(x), first+round(duration*sr))]
            z = z - np.mean(z)
            cents = float(np.median(pitch_frames(z, root)))
            # A-rate resampling calibrates the recorded note to its integer root.
            step = 2 ** (-cents/1200)
            z = np.interp(np.arange(0,len(z)-1,step), np.arange(len(z)),z)
            fade = min(round(sr*(.025 if source=='ney' else .002)),len(z)//4)
            z[:fade] *= np.linspace(0,1,fade)
            fade = min(round(sr*.06),len(z)//4)
            z[-fade:] *= np.linspace(1,0,fade)
            z = np.rint(z * (28000 / np.max(abs(z)))).astype('<i2')
            residual = pitch_frames(z.astype(float),root)
            assert abs(float(np.median(residual))) < 3, (name, root, residual)
            note = ['C','Cs','D','Ds','E','F','Fs','G','Gs','A','As','B'][root%12]+str(root//12-1)
            file = name.lower().replace(' ','-')+'-'+note+'.wav'
            with wave.open(str(OUT/file),'wb') as w:
                w.setparams((1,2,sr,len(z),'NONE','not compressed'));w.writeframes(z.tobytes())
            files.append(dict(file=file,root=root,source=source,start=first/sr,frames=len(z),
                              calibration_cents=round(-cents,3),median_error_cents=round(float(np.median(residual)),3),
                              natural_pitch_span_cents=round(float(np.ptp(residual)),2),
                              sha256=hashlib.sha256((OUT/file).read_bytes()).hexdigest()))
            zones.append((z.tolist(),root,None,None))
        hdr,data = user_slot(name,zones)
        slug = name.lower().replace(' ','-')
        (OUT/(slug+'.hdr')).write_bytes(hdr);(OUT/(slug+'.bin')).write_bytes(data)
        banks.append(dict(name=name,slot=slot,files=files,bytes=len(data),header=slug+'.hdr',data=slug+'.bin'))
        print(name,len(files),'zones',len(data),'of 81408 bytes')
    (OUT/'banks.json').write_text(json.dumps(banks,indent=2)+'\n')

if __name__ == '__main__':
    main(sys.argv[1])
