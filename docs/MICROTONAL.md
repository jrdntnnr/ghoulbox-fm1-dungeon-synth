# Microtonal GHOULBOX

This branch combines GHOULBOX 1.4 (`b61a931`) and Felucca 1.5.1 (`a4d7ca2`) with 40 factory scales, one custom scale and real per-voice tuning. The JieLi target build succeeds with the hidden SLICE engine omitted. Physical FM-1 validation remains pending; see [integration status](INTEGRATION-STATUS.md).

## Play Rast or Bayati

1. Select the track and its sound.
2. Open **SCL**. Choose **ROOT C**, **SCL RAST**. E and B sound 50 cents below their normal pitches in every octave; C and G stay unchanged.
3. Choose **QNT WHITE** to put successive scale notes on the white keys. **OFF** keeps the normal chromatic keyboard while applying the selected tuning; **SNAP** snaps keyboard notes into the scale; **SEQ** also snaps sequencer notes. The scale's tuning applies to played notes in every mode.
4. Move through the SCL pages to **TUNE 1**, **TUNE 2**, **TUNE 3**. These adjust the twelve chromatic keys relative to ROOT, in one-cent steps from −63 to +63.
5. Save the **project** to retain the scale, root and trims for each track.

For Bayati, choose **ROOT D**, **SCL BAYATI**. Its second degree, E, is half-flat. The playable white-key layout follows the tonic. Keys in the scale diagram have a downward arrow when flattened, an upward arrow when sharpened (normal display mode).

The tuning pages show **additional trims**, not the factory offset. In C Rast, the control **3** at `0 ct` means E is already −50 cents. Set it to `+13 ct` for E at −37 cents, or `−13 ct` for E at −63 cents. Set it to `+50 ct` to restore equal-tempered E. Trims remain when you change scales or load another sound; return all twelve to zero for an unmodified factory tuning.

| Page | Controls (chromatic intervals above ROOT) |
| --- | --- |
| TUNE 1 | 1, b2, 2, b3 |
| TUNE 2 | 3, 4, b5, 5 |
| TUNE 3 | b6, 6, b7, 7 |

The selected root rotates both factory offsets and trims. Transpose moves note identities first; the resulting notes use the current root's tuning map. Recorded steps keep those note identities, so changing a track's tuning also retunes its existing sequence. Different tracks can use different tunings. Sound presets keep the track's tuning, like the existing SCL settings; use projects to recall a complete tuning setup. The web editor exposes the same controls when connected to this firmware.

## Built-in chord generator

Choose a polyphonic engine such as ANALOG or FM6. GURDY retains its one-voice instrument limit; it cannot sound a complete generated chord. Set **VOICE POLY**, then **SCL > CHORD > DIA3** (triads) or **DIA7** (sevenths). Each generated note uses the selected scale's own tuning plus its cent trim, independently and in every octave. CLOSE, OPEN, INV1, INV2 and +OCT keep that tuning. MONO, LEGATO and UNISON retain Felucca's root-only chord behavior.

For C Rast, DIA7 on C plays C, E half-flat, G and B half-flat. Set the **3** trim to +13 ct and only E changes from −50 to −37 cents. For D Bayati, a generated chord on E includes the half-flat E; the tonic D triad does not contain that second degree. The screen says **C TUNED** when a voiced note has a cent offset, and arrows mark altered keys. This avoids calling a neutral-third chord ordinary C major.

MAJ/MIN/DOM7 and the other fixed shapes retain their fixed key intervals, with the track's tuning applied afterwards. Use DIA3/DIA7 for scale-derived harmonization. The generator is a creative chord tool, not a model of traditional maqam harmony or melodic development.

Keyboard, incoming MIDI, chord/UP arpeggiation, chord recording and sequencer playback are covered by automated tests. The test renders every simultaneous oscillator voice across 7,560 combinations of nine maqam presets, twelve roots, seven degrees, two chord types and five voicings, with custom trims. All measured errors are below 0.5 cents. The dedicated test also passes ASan/UBSan. External MIDI output remains ordinary note numbers.

## Scala import

In the web editor, connect to this firmware, select a pitched track and choose **Import Scala tuning**. An octave-repeating `.scl` with 1–12 degrees is mapped to distinct keyboard identities and cent trims, then selects CUSTOM. ROOT stays as selected; choose WHITE to play successive scale degrees on white keys. The imported tuning is saved in the project.

The importer accepts ratios and decimal cents. It validates the entire file before writing, rejects non-octave periods, more than twelve notes and mappings requiring a trim outside ±63 cents, and rounds accepted values to integer cents. `.kbm` files and arbitrary-period tuning are not supported. A disconnected transfer is reported as incomplete and should be retried.

## Included scales

The original sixteen scale IDs are unchanged: CHR, MAJ, MIN, DOR, MIX, PEN, MPEN, HARM, PHRY, LYD, LOC, MEL, BLUES, WHOLE, DIMHW and DIMWH.

The six presets below use editable quarter-tone starting points. They describe pitch collections; they do not model a maqam's melodic development, modulation or context-dependent intonation.

| Display | Starting pitches, cents above ROOT |
| --- | --- |
| RAST | 0, 200, 350, 500, 700, 900, 1050 |
| RAST NH | 0, 200, 350, 500, 700, 900, 1000 |
| BAYATI | 0, 150, 300, 500, 700, 800, 1000 |
| BAY RAST | 0, 150, 300, 500, 700, 850, 1000 |
| SUZNAK | 0, 200, 350, 500, 700, 800, 1100 |
| BAY SHUR | 0, 150, 300, 500, 600, 900, 1000 |

The two Rast and two Bayati entries offer alternative upper ajnas. References: [MaqamWorld Rast](https://www.maqamworld.com/en/maqam/rast.php), [Bayati](https://www.maqamworld.com/en/maqam/bayati.php), [Suznak](https://www.maqamworld.com/en/maqam/suznak.php), [Bayati Shuri](https://www.maqamworld.com/en/maqam/bayati_shuri.php). The quarter-tone cent values are this implementation's starting tunings, not a claim that a single fixed tuning covers performance practice.

The other additions are HIJAZ, HIJAZKAR, NIKRIZ, HARM MAJ, DOR b2, LYD AUG, LYD DOM, MIX b6, LOC #2, ALTERED, HUNG MIN, UKR DOR, MAJ BLUS, HIRAJOS, INSEN, IWATO, YO and EGYPT. Their initial offsets are zero; any key can be adjusted on the tuning pages.

## Limits of this milestone

- The model uses twelve MIDI key identities per repeating octave, with scale masks and cent offsets. Scala `.scl` import is supported as described above; `.kbm`, arbitrary periods, more than twelve independent keys per octave, named tuning banks and scale favorites are not.
- Pitched voices receive their own tuning, including keyboard, incoming MIDI, chords, arpeggios and sequence playback. DRUM and SLICE retain their trigger maps. Fixed internal intervals and inharmonic resonances remain characteristics of their instruments. GURDY's drones use their own root-relative offsets.
- **MIDI output still sends ordinary note numbers.** External synths will not reproduce the tuning automatically. MPE, MIDI Tuning Standard and per-note outgoing pitch bends are not implemented.
- Live changes follow the existing glide mechanism for coarse pitch, with the sub-step correction applied each render block. This is not a new continuous high-resolution glide engine. Sample playback and granular pitch still have the engines' existing range limits.
- HALL uses Felucca 1.5.1's implementation; the dungeon TAPE/CRSH effects remain.

## File compatibility

Projects use numeric magic `0x47424D32` (GBM2), 3,840 bytes. The compact parameter encoding fits the existing flash sector and preserves Felucca's FM6 patch, song chain, automation, drum lanes and step details. There are 125 parameters per track: cent trims 103–114, CUSTOM mask 115–116, engine parameters 117–124. New presets use versions 8/9, still 192 bytes.

Loaders support original GHOULBOX FUN8, our frozen GBM1 prototype and upstream Felucca FUNA projects, plus older supported FUN formats and presets. Missing tuning defaults to zero. Older firmware cannot read GBM2 or preset versions 8/9; keep original backups.

Settings use GBS1, separating MIDI-learn mappings from GURDY favorites and SCENE from screen timeout/metronome settings. An untagged PER4 record read on boot is treated as GHOULBOX. Felucca used the same record identifier for different fields: its source cannot reliably be inferred. Before restoring an older archive, explicitly convert its settings into a new file:

```sh
python3 tools/migrate_settings.py --source ghoulbox original.fm1backup converted.fm1backup
# For a backup made by upstream Felucca, use --source felucca instead.
```

The converter verifies object checksums, changes only settings and refuses to overwrite an existing output. Projects and sample objects stay unchanged.

## Build and verify

From the repository root, with a C compiler, Python 3 (Pillow and fontTools) and Node.js:

```sh
sh tests/run_tuning_tests.sh
```

The command generates missing local headers, checks the scale catalog, runs native pitch/input/project/UI/editor tests, compares the audio reference renders, checks UI layouts, and runs the web protocol and backup tests. It does not need the JieLi SDK, create a firmware package, or flash a device. Logs are in `build/assessment/`; UI renders are in `build/ui_tuning/`.

The pitch test measures actual sine output for quarter-tones and fine adjustments, checks simultaneous C/E/G tuning, and sweeps all 40 factory scales, twelve roots, MIDI notes 12–115 and every permitted trim through the oscillator pitch calculation. The measured worst calculation error is below 0.5 cents. Additional tests inspect SAMPLE/GRAIN playback rates at ±1-cent and quarter-tone settings and GURDY's independent melody/drone phases. This is not an acoustic accuracy guarantee for every sampled or physical instrument.

The catalog lives in `assets/scales.json`. IDs are append-only. Regenerate `firmware/src/scales.h` with `python3 tools/gen_scales.py`; update the web mock's names too. The web test compares those names with the catalog and the firmware descriptors. The firmware build checks the generated header for staleness.
