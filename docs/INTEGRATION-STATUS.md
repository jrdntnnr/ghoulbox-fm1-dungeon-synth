# Microtonal development preview — 2026-10-11

This fork combines **GHOULBOX 1.4** (`b61a931`) with **Felucca 1.5.1**
(`a4d7ca2`). GHOULBOX advanced by 25 commits after the initial 1.1 assessment;
those changes are integrated. Both histories are retained as merge parents.
The public source and releases belong to
[jrdntnnr/ghoulbox-fm1-dungeon-synth](https://github.com/jrdntnnr/ghoulbox-fm1-dungeon-synth).

**Status: buildable development preview, not a hardware-validated release.**
No FM-1 has been flashed or acoustically tested in this work. The broader suite
still reports UI layout and static target-cost failures described below.

## Implemented

- 40 factory scales, including editable Rast/Bayati variants, plus CUSTOM.
- Per-track, root-relative cent trims applied independently to sounding voices.
- Built-in DIA3/DIA7 chords, all five voicings, and altered-note indicators.
- Octave Scala `.scl` import (1–12 degrees; ratios or cents; bounded keyboard
  mapping); no `.kbm`, non-octave periods or more than twelve keys per octave.
- Compact GBM2 projects and preset versions 8/9, with legacy migration tests.
- GHOULBOX scenes, CRYPT drums, revised GURDY, dungeon samples/presets,
  cassette effects and MIDI controls alongside Felucca's modern sequencer,
  stereo HALL, inserts and FM6 bank support.
- Separate settings storage for MIDI learn and GURDY favorites; explicit
  source selection when converting ambiguous old PER4 settings backups.

See [MICROTONAL.md](MICROTONAL.md) for controls, scales and compatibility.
GURDY retains its one-voice cap; use a polyphonic engine for full chords.
External MIDI output remains ordinary note numbers.

## Build evidence

The verified JieLi `jieli-linux-toolchains-20260730.1` compiler runs in an
amd64 Debian container via an Apple-silicon Lima VM with Rosetta. The three
required AC79 SDK packaging assets are SHA-256 checked by the builder.

| Item | Final preview build |
| --- | --- |
| Application | 573,152 bytes |
| Static RAM | 50,548 / 73,728 bytes |
| SDRAM pool | 332,132 / 368,640 bytes |
| RAM-resident interrupt code | 916 instructions, no calls |
| Loader | 6,930 bytes |
| Firmware package | 610,086 bytes; identity `FM-1_900` |
| Package SHA-256 | `33b26b830218839ec99bb85adab79c081d219bebc93869a2426ff6be5496708e` |

The target profile omits the already-hidden SLICE engine to fit the original
application slot. Engine ID 13 stays reserved, but old SLICE tracks cannot
play in this package. SAMPLE, GRAIN, GURDY, DRUM and the SLICER effect remain.
Host tests retain the full SLICE implementation. [Build instructions](../BUILDING.md).

## Verified on the host

- **7,560 chord cases:** nine maqam presets × twelve roots × seven degrees ×
  triads/sevenths × five voicings. Simultaneous oscillator phase measurements,
  with custom trims, stay within 0.5 cents. Dedicated ASan/UBSan run passes.
- Keyboard WHITE mapping, transposed Bayati MIDI input, CHORD/UP arpeggiation,
  chord recording/playback, releases, custom scale masks and tuned chord labels.
- Fine pitch math, rendered sine frequencies, SAMPLE/GRAIN playback rates,
  independent GURDY melody/drone tuning, track isolation and live retuning.
- **139 audio golden renders:** zero changed hashes, health failures,
  voice/routing failures, CPU-budget failures or crashes. These golden files
  match GHOULBOX 1.4; they were not regenerated to hide integration failures.
- **720 engine-switch comparisons:** zero differences from the existing
  Felucca reference. Tests explicitly select the original DRUM KIT because
  GHOULBOX added two presets where the reference firmware had only one.
- Project/preset migration, compact storage, settings conversion, persistence,
  MIDI controls, editor protocol and web backup/Scala tests.
- Legacy PIANO HD in USR1 matches the built-in piano bit-for-bit at all twelve
  tested keys. The sample test now addresses the fixed USR1 ID rather than
  mistaking GHOULBOX’s expanded factory-set count for a user-slot ID.
- Native UI functional tests, including the optional legacy FM4 build, pass.
  Text reference and font-spacing checks pass.
- Update entry and loader simulations pass with the final package, including
  interrupted transfers, refused damaged images and write-window boundaries.

## Remaining findings

The full suite is **not green**. No thresholds were raised or failed tests
silently disabled:

- UI render audit: **580 layout findings and 1,154 alignment findings** across
  221 screens, eleven palettes and the parameter sweep. Examples include
  clipped wide numeric values, splash geometry and FM6 chart spacing. The
  audit reports zero text-on-identical-background errors and zero off-palette
  GREY/MONO pixels. The Rast chord screen has also been visually inspected.
- Target static cost: `noise_render` scores **1,008 vs 259** and the audio ISR
  **45,394 vs 37,502**, exceeding the existing +10% limit. These are compiler
  disassembly estimates, not measured device timings. Host CPU checks pass;
  that does not establish FM-1 audio headroom.
- Physical installation, USB enumeration/audio, save/reboot/recovery and
  worst-case playback must still be tested on an FM-1. No connected device
  was available. A browser-driven visual editor test was not performed;
  protocol, parser and deployment-file checks are automated.

Release assets include selected validation logs and checksums. The full-suite
log retains two SAMPLE failures from stale upstream test assumptions; the
corrected test’s separate `sample-final.log` passes. Local complete
logs live under ignored `build/assessment/`; UI audit reports are in
`build/ui_new/`.

## Maintenance judgment

This is worth maintaining for a dungeon-synth instrument: GURDY, the sample
sets, scenes, cassette treatment and musical presets remain useful additions
to Felucca. The expensive part is keeping project formats, parameter IDs,
settings and engine state compatible across two active histories. This merge
adds frozen migration fixtures and pitch/chord tests to make those changes
reviewable. Keep upstream merges explicit and use this preview as the basis
for hardware testing before relying on it as a performance instrument.
