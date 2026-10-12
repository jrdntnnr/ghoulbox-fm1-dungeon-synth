# Santur, mijwiz and arghul adaptations

These are **new FM1 implementations of published instrument designs**, using the
existing PHYS/TRIO DSP. They are neither imported Quilcom plugins/presets nor
recorded samples. The source designs are Quilcom / Spogg's SIM-SANTUR v1.00 and
SIM-MIJWIZ v1.00, available from <https://flowstoners.com/quilcom>.

The downloaded user guides, factory preset tables and embedded FlowStone DSP
text were inspected. Both packages' plugin-info files expressly allow using
their parts in other projects. No standard open-source licence was supplied;
we do **not** describe Quilcom as MIT/GPL. No Quilcom code, waveforms, binaries,
manuals or preset tables are bundled here. Our adaptation code/presets are
GPL-3.0-only; the existing DaisySP/Rings DSP retains its MIT notices.
Download URLs, versions and SHA-256 hashes are in
[`assets/desert/synth-sources.json`](../assets/desert/synth-sources.json).

## Find the sounds

Select the engine, then browse its presets (ALL category if a filter is active):

| Engine | Preset | Character |
| --- | --- | --- |
| PHYS | SANTUR | Bright struck three-string course |
| PHYS | DUSK SANTUR | Softer strike, longer decay and more room |
| TRIO | MIJWIZ | Two nearly unison reed-like pipes with gentle beating |
| TRIO | ARGHUL | Melody pipe over a quieter, fixed tonic drone |

These are built into the updated firmware/emulator, consume no USR sample
slots, and require a firmware update on hardware. Loading a sound preserves
the track's selected maqam, root, trims and keyboard mapping.

## What was adapted

**Santur:** Quilcom documents two to four detuned strings per struck note,
adjustable hammer hardness/position, decay and body coloration. Our reduced
version directly excites three existing Rings-derived string loops with a
DaisySP filtered burst. The source's independent string tuning, body formants,
velocity-dependent buzz and pedal-selectable decay are not ported. The dark
variant is our own sound design built on the same adaptation.

PHYS's new `SANT` model has `DTN / BRIT / DAMP / POS / ACC / BOW / EXC`.
DTN spreads the outer strings symmetrically by 0–12 cents (percentage control);
the middle string stays at the tuned note. Set DTN to zero for pitch checks.
The web editor's static PHYS descriptor still labels this control STRC;
on SANT it controls detuning, as DTN does on the device/emulator.
BOW adds the existing random sustained excitation, an experimental texture,
not a physical bow or traditional santur technique. EXC mixes the dry strike.
The useful register is C3–C6. Below the delay-line range, low notes fold up an
octave, as in the existing sympathetic-string engine. Three-note polyphony
is unchanged: use DIA3; a simultaneous DIA7/+OCT can steal voices. Arpeggiation
can play the larger chord one note at a time.

**Mijwiz:** retains the published paired-pipe/unison-beating arrangement.
TRIO `DUAL` uses two band-limited pulses and the existing filter, replacing
Quilcom's nine custom reed waveforms and comb coloration. INT2 changes the
second pipe interval; DTN sets its offset in cents; INT3 is inactive in DUAL.
The original preset's keyboard offsets are not copied: our central maqam and
per-note tuning controls determine the pitches without double-retuning.

**Arghul:** retains the published melody-plus-quieter-fixed-drone arrangement.
TRIO `DRONE` puts pipe 2 at **C3 + ROOT + INT2**, regardless of melody note.
Its own note's tuning trim and global TUNE apply; melody pitch bend/LFO do not.
DTN offsets only the drone in cents; INT3 is inactive. Both pipes stop with the
voice envelope. These presets default to POLY for the chord generator; an
arghul chord layers one drone per voice. Choose MONO/LEGATO for a single melody
and drone, or sequence a separate drone track for independent articulation.

These are stylized approximations for dungeon synth, not validated acoustic
reconstructions. No claim of Persian performance authenticity is made.

## Compatibility and checks

Old engine/preset IDs are unchanged; new presets and TRIO modes are appended.
PHYS model 4 remains reserved for legacy DRUM migration; SANT uses 5 and the
knobs skip 4. Projects and user presets keep the new values in the existing
format. No extra synthesis state memory is allocated.

From the repository root, after generating `build/gen`:

```sh
mkdir -p build/assessment/sourced-synths
cc -O2 -w -Ibuild/gen -Ifirmware/src -o build/assessment/sourced-synths/test tests/sourced_synth_test.c -lm
build/assessment/sourced-synths/test build/assessment/sourced-synths
python3 tests/check_sourced_audio.py build/assessment/sourced-synths
```

The Python measurement needs NumPy. The native test checks real chord input,
note-off/release, project and preset values, and an independently tuned drone.
The audio test checks 28 single notes C3–C6 with detuning disabled, including
Rast half flats and cent trims, plus all three simultaneous chord tones in
each preset. The shipped presets deliberately beat by a few cents. Host DSP
tests include parameter sweeps, model switching and CPU comparisons. Hardware
listening and CPU validation still require the physical FM1.
