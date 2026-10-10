# Microtonal tuning — GHOULBOX 1.2-dev

This development branch adds 40 scale entries and real per-voice microtonal pitch to the GHOULBOX 1.1 sound engine. It preserves the dungeon presets, samples, GURDY and effects. It does **not** yet incorporate Felucca 1.5. Native audio and UI tests run without hardware; a JieLi target build and an FM-1 listening test are still required before installation. No new installable firmware is supplied here.

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

- The model uses twelve MIDI key identities per repeating octave, with scale masks and cent offsets. Scala `.scl`/`.kbm` import, arbitrary period sizes, more than twelve independent keys per octave, named custom tuning banks and scale favorites are future work.
- Pitched voices receive their own tuning, including keyboard, incoming MIDI, chords, arpeggios and sequence playback. DRUM and SLICE retain their trigger maps. Fixed internal intervals and inharmonic resonances remain characteristics of their instruments. GURDY's drones use their own root-relative offsets.
- **MIDI output still sends ordinary note numbers.** External synths will not reproduce the tuning automatically. MPE, MIDI Tuning Standard and per-note outgoing pitch bends are not implemented.
- Live changes follow the existing glide mechanism for coarse pitch, with the sub-step correction applied each render block. This is not a new continuous high-resolution glide engine. Sample playback and granular pitch still have the engines' existing range limits.
- The fork's existing HALL delay-length/CPU concerns from the assessment are unchanged.

## File compatibility

Projects now use numeric magic `0x47424D31` (GBM1), 3,648 bytes, 103 parameters per track. Tuning parameters occupy 83–94; engine parameters move to 95–102. New presets use record versions 6 (notes) and 7 (drum grid), with the existing 192-byte record size.

Original GHOULBOX 1.1 FUN8 projects (3,584 bytes), older FUN1–FUN7 projects, and old user presets load with zero extra trims. Engine parameter values and their motion-event IDs are remapped. Original scale IDs remain stable. A frozen project and preset written by commit `c7891e0` test this migration independently of the new writer.

Older firmware does not read the new project magic or new preset versions. Keep original backups if you need to return to 1.1. Compatibility with newer upstream Felucca project layouts is not claimed. Flash storage is read and repacked on load; a RAM-only retained cache may lose slots whose offsets move after an update.

## Build and verify

From the repository root, with a C compiler, Python 3 (Pillow and fontTools) and Node.js:

```sh
sh tests/run_tuning_tests.sh
```

The command generates missing local headers, checks the scale catalog, runs native pitch/input/project/UI/editor tests, compares the 134 original audio reference renders, checks UI layouts, and runs the web protocol and backup tests. It does not need the JieLi SDK, create a firmware package, or flash a device. Logs are in `build/assessment/`; UI renders are in `build/ui_tuning/`.

The pitch test measures actual sine output for quarter-tones and fine adjustments, checks simultaneous C/E/G tuning, and sweeps all 40 scales, twelve roots, MIDI notes 12–115 and every permitted trim through the oscillator pitch calculation. The measured worst calculation error is below 0.5 cents. Additional tests inspect SAMPLE/GRAIN playback rates at ±1-cent and quarter-tone settings and GURDY's independent melody/drone phases. This is not an acoustic accuracy guarantee for every sampled or physical instrument.

The catalog lives in `assets/scales.json`. IDs are append-only. Regenerate `firmware/src/scales.h` with `python3 tools/gen_scales.py`; update the web mock's names too. The web test compares those names with the catalog and the firmware descriptors. The firmware build checks the generated header for staleness.
