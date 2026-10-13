# Dungeons of the Desert

Preview 2 adds a desert palette to GHOULBOX's existing dungeon instruments.

The current source also includes an optional **Persian Tar** bank: four calibrated
plucks from Berklee's BISA archive, licensed CC BY. In **Samples**, choose **Tar**
in any USR slot's bank menu, **Prepare Tar**, preview, then **Upload**. Select that
slot on a SAMPLE track. The browser emulator has **Load Tar on selected track**
below its keyboard. [Credits, processing and slot limits](../assets/desert/README.md#persian-tar--cc-by).

## Start playing

Hold HOME, open **SYSTEM > SCENE**, select **CITADEL**, then restart to load the
scene. Scene selection chooses the power-on arrangement; it does not replace
the current song immediately. CITADEL (Buried Citadel) runs at 78 BPM, with
**Bayati on D** on its three pitched tracks: DESERT OUD, an FM6 pad/drone,
DUNE NEY, and DUNE DRUMS. The E keys sound 50 cents flat. All old scene and
preset IDs retain their meanings.

Open **PRESETS**, turn **knob 4 (LIST)** to **DESERT**, then turn the
**PRESETS knob** to browse all nine sounds below plus **USR1, USR2, USR3**.
The USR entries play whichever banks are stored in those slots; empty slots
are silent. The firmware update does not upload or replace sample banks.
In the web editor, use **Library → Device presets → Show → DESERT**.

The built-in presets are:

| Engine | Preset | Character |
| --- | --- | --- |
| PHYS | DESERT OUD | Short, woody string pluck |
| PHYS | BRASS QANUN | Bright, ringing string pluck |
| PHYS | SANTUR / DUSK SANTUR | Struck three-string courses, bright / softer |
| TRIO | MIJWIZ | Paired reed-like pipes |
| TRIO | ARGHUL | Reed-like melody with a fixed tonic drone |
| WHEEL | DUNE NEY | Soft flute lead |
| DRUM | DUNE DRUMS | Frame drum, hand slap and shaker from the CRYPT kit |
| NOISE | SAND WIND | Slow, filtered wind texture |

These are synthesized interpretations using existing engines. CARAVAN, DUNES
and SANDSTEP are three new sequencer patterns. The melodic patterns use Bayati
on D in CITADEL; loading a pattern elsewhere keeps that track's existing scale.

## Recorded instruments

The optional [CC0 sample pack](../assets/desert/README.md) includes three oud
plucks, six qanun notes and three short ney tails. Open the
[web editor](https://jrdntnnr.github.io/ghoulbox-fm1-dungeon-synth/webapp/editor/),
connect, and select **Samples**. Each USR slot has its own **Prepare** button.
It fills the draft only; preview the notes and press **Upload** when ready to
replace that slot. The firmware update itself leaves these slots alone.

Select SAMPLE, then SET **USR1** for Oud, **USR2** for Qanun, or **USR3** for Ney
Air. The same banks work as GRAIN sources. Keep ROOT D / SCALE Bayati to fit
CITADEL, or choose another maqam and adjust the twelve cent trims to taste.
The recordings are calibrated to integer roots; the synth applies their scale
and trim offsets independently for each note.

These compact banks are mono one-shots, not large, looped acoustic libraries.
Ney Air retains performance bends and breath; use DUNE NEY for steady pitch.

The santur and reed sounds are compact adaptations of published Quilcom
designs using our existing DSP, not plugin imports or recorded samples.
See [sources, controls and limitations](SOURCED-SYNTHS.md). They are built in
and do not occupy sample slots. Arghul's drone follows ROOT, independently of
the melody; its INT2 shifts that drone relative to C3 + ROOT.
The qanun source is a Turkish kanun. Sources, CC0 links, cuts, input hashes and
measured pitch calibration are included with the pack.

## Chords and tuning

SAMPLE supports the built-in DIA3/DIA7 chord generator with these banks.
PHYS supports three voices, so use DIA3 in CLOSE/OPEN/INV1/INV2 for its plucks; DIA7
and +OCT can exceed that cap. DUNE NEY defaults to LEGATO for a lead;
change VCE to POLY for chords. An FM6 pad or SAMPLE track is the best place for
full sevenths. Maqam chord generation is a creative arrangement feature, not
a claim that Western-style chords are part of every maqam tradition.

Preview 2 fixes lost fine tuning in ANALOG's detuned SUB oscillator and NOISE's
LFSR clock, and prevents a high pitched-noise clock from wrapping to a low rate.
SAMPLE/GRAIN now use precise rates for every note on a newly added scale or a
track with cent trims, including unaltered chord members. Original scales 0–15
with zero trims retain their previous audio output.

## Validation and limits

Native tests load all three actual bank files, render Bayati sevenths with
independent cent trims, verify sample advancement within one cent, and release
all chord notes. The web tests decode the shipped WAVs and check roots, hashes,
capacity and download failures. The source performance's natural pitch motion
is separate from the engine's playback-rate accuracy.

A 16-second CITADEL audition is rendered by the firmware's native DSP in the
release assets. Five new presets have audio regression hashes; the previous
139 hashes remain unchanged. Hardware audition/flash validation is pending.
See [integration status](INTEGRATION-STATUS.md) for existing layout and target
CPU-estimate limitations.
