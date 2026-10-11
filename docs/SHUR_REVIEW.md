# Shur review package

Material for a Persian classical musician to review the experimental **SHUR1** and **SHUR2** tunings ([MICROTONAL.md](MICROTONAL.md#persian-shur-experimental)) without a device. The audio comes from the firmware's own synthesis code on the host.

## Make it

From the repository root, after `sh tests/run_tuning_tests.sh` has generated `build/gen`:

```sh
cc -O2 -w -Ibuild/gen -Ifirmware/src -o build/host/shur_review tests/shur_review.c -lm
mkdir -p build/shur-review && build/host/shur_review build/shur-review
```

Send the musician the contents of `build/shur-review/` and this page.

## What is in it

All clips are on **D**. Clips marked with a drone play a sine D3 underneath. **12TET_ref** is the same seven notes in equal temperament (D natural minor), for comparison.

| File | What to listen for |
| --- | --- |
| `scale_SHUR1_sine.wav` | D4 up to D5 and back, 0.6 s a note, over the drone. E is 50 cents low (koron); A is natural |
| `scale_SHUR2_sine.wav` | The same, with A also 50 cents low |
| `scale_12TET_ref_sine.wav` | The same seven notes, equal-tempered |
| `scale_*_oud.wav` | The same three scales on the plucked oud model, **without** a drone (see below) |
| `dyads_SHUR1.wav` / `_SHUR2` / `_12TET_ref` | D4 + E4 held 3 s, then D4 + A4 held 3 s, over the drone |
| `fifth_switch_1_2_1.wav` | A4 held while the scale switches 1 → 2 → 1 every 2 s. Shows that a held note retunes when the scale changes |
| `shur1.scl`, `shur2.scl` | Scala files of both tunings, for the musician's own software |

| Note above D | E | F | G | A | B♭ | C |
| --- | --- | --- | --- | --- | --- | --- |
| SHUR1, cents | 150 | 300 | 500 | 700 | 800 | 1000 |
| SHUR2, cents | 150 | 300 | 500 | **650** | 800 | 1000 |

Measured from the rendered files: the sine and dyad clips match these values to within measurement error (E −50, A −50 in SHUR2). The oud model plays every note about **20 cents sharp**, in all three scales including the reference. Its intervals are correct, but it disagrees with a true-pitch drone, which is why its clips have none. This is a property of the PHYS engine, not of the Shur values.

## Questions for the reviewer

1. Which tradition, radif or performer should be the reference for Shur?
2. Where should the koron second sit? 150 cents is a 24-EDO approximation; give a value or a range.
3. Is a lowered fifth (SHUR2) a justified variant of Shur? If so, at what pitch, and in which gushehs or contexts?
4. Should the two fifths be separate scales (as here) or one eight-note collection with both?
5. Are other degrees context-dependent enough to need their own variant?
6. The device labels are SHUR1 and SHUR2 (the screen shows five characters). Should a variant be named for its context instead?

The answers decide the cent values in `assets/scales.json`. Rows 41 and 42 can change freely until a release ships them.
