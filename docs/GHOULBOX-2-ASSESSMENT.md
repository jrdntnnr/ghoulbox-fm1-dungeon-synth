# Ghoulbox 2.x assessment — 2026-10-13

Recommendation: integrate selected 2.1 features into our existing fork, keeping
its Felucca 1.5.1 base, microtuning, instruments and storage formats. The
upstream binary is not a compatible replacement for this development build.

## Versions checked

- Our local and fetched `publish/microtonal`: `b9d47b7`.
- Ghoulbox 2.0: `df78c85` (`ghoulbox-v2.0`).
- Ghoulbox 2.1: `eaf04bd` (`ghoulbox-v2.1`), already released on October 12.
- Fetched upstream branch: `2f001b1`; its two post-2.1 commits concern links.
- Shared ancestor: `b61a931` (our earlier Ghoulbox integration).

Release sources: [2.0](https://github.com/jasonpersinger/ghoulbox-fm1-dungeon-synth/releases/tag/ghoulbox-v2.0),
[release list including 2.1](https://github.com/jasonpersinger/ghoulbox-fm1-dungeon-synth/releases).
The findings below also come from direct inspection of those Git tags.

## What we gain

| Feature | Value to our desert instrument |
| --- | --- |
| USB rescue (2.0) | Highest priority. A small recovery mode keeps the MIDI installer usable after failed boots or with OCT− held at power-on. Our current failed-boot path goes directly to ROM UBOOT. |
| Drone latch (2.0) | Hold a chord/drone, release the keys, and play another track. Especially useful on the short keyboard. Requires tests with our tuned chords and note-release ownership. |
| Shared voice budget 8 → 12 (2.0) | More room for accompaniment and melody. Requires target CPU/RAM tests; PHYS remains capped at three voices per track and GURDY at one. |
| GURDY crank (2.0) | SELECT controls the wheel's pressure/speed. Useful expression for dungeon textures; preserve our drone/melody tuning when porting it. |
| Temperaments (2.0) | Pythagorean, quarter-comma meantone and just intonation. These are additional tuning options, not our maqam implementation. Integrate into the central pitch path with explicit defaults and composition rules. |
| Cave ambience/crackle (2.0) | Useful sound design, but overlaps our existing SLOOP TEXTURE sources and controls. Prefer integrating useful source algorithms into TEXTURE over maintaining two independent ambience systems. |
| Layer/split (2.1) | Layering two instruments is useful. Keep splitting optional and off by default, consistent with the user's sequenced-accompaniment workflow. |
| Note/chord names (2.1) | Useful feedback, but adapt it to reflect microtonal alterations rather than showing an unqualified conventional chord name. |
| Restore last session (2.1) | Review settings migration and preserve the owner's choice; our newer Felucca base already includes autosave/restore infrastructure. |

The three advertised dungeon scales are already represented here:
Phrygian dominant = our HIJAZ key pattern, Hungarian minor = HUNG MIN,
double harmonic = HIJAZKAR. These key-pattern equivalences do not imply identical
traditional melodic practice.

## Compatibility work is required

A non-checkout merge preview (`git merge-tree --write-tree HEAD ghoulbox-v2.1`)
reports **57 conflicting files**, including tuning/voice paths, storage,
sequencer, UI, editor, build tooling and tests. It did not modify our branch or
working files. This is a substantive integration, not a clean pull.

Concrete collisions found in the tagged sources:

- Track parameter **103** is their `P_TEMP` and our `P_TC0`. Their engine
  controls start at 104; ours start at 117. Count-based import alone is unsafe.
- Global IDs **27–32** mean ambience and keyboard layer/split upstream, but
  our six SLOOP TEXTURE controls use those IDs. Both have `G_COUNT == 33`, so
  matching counts do not establish compatible meanings.
- Their scale IDs **16–18** are PHDOM/HUNG/DHARM; ours are RAST/RAST NH/BAYATI.
  Existing IDs must be preserved and imported scales translated explicitly.
- Our project magic is **GBM2**, presets are versions **8/9**, and settings
  are **GBS1**. Upstream uses **FUNA**, preset versions **4/5**, and **PER4**.
  Its readers do not accept our current formats. Its promise to load older
  Ghoulbox projects does not cover this fork.
- Their seven reserved Felucca 1.5 parameters are active features here
  (inserts, filter type and synchronized envelopes). Preserve their behavior.

Flashing upstream would replace our maqam/Scala implementation and added
synthesized voices. Our sample recordings remain reusable assets, but that is
separate from project/preset compatibility. Backups need an explicit migration
path; do not assume either fork can safely import the other's new records.

## Resource constraints and proposed order

Our last verified application is **579,484 bytes**, with only **2,080 bytes**
free in the application slot. Static RAM is 52,724/73,728 bytes and the SDRAM
pool 332,132/368,640 bytes. No combined 2.x build has been made, so its flash
fit or device CPU headroom is not established.

1. Port USB rescue and its boot/update tests, then build and measure.
2. Add drone latch and evaluate twelve shared voices with maqam chords,
   engine voice caps, sustained notes and worst-case FX loads.
3. Add microtonal-aware note labels, optional layer/split and crank behavior.
4. Add temperaments through the existing tuning system; reconcile ambience
   with TEXTURE rather than replacing it.
5. Add frozen migration fixtures for our existing projects/presets/settings
   and upstream 2.x records. Check editor protocol, sample-slot persistence,
   autosave, restart and release behavior before publishing a combined build.

Preserve SARDAB branding, our fork's deployment URLs and our sound assets when
bringing in web/build changes. Upstream now writes its own custom-domain CNAME.

No firmware was merged, rebuilt or flashed during this assessment. The existing
installer/package remains the `b9d47b7` build. The merge preview is retained at
`build/assessment/ghoulbox-2/merge-preview.txt`.

Implementation followed this assessment; see [SARDAB 2.1 integration](GHOULBOX-2-INTEGRATION.md) for the resulting changes and limitations.
