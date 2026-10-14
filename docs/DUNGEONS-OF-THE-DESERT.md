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

## Rhythm, ornament, names

**Iqa'at.** SAVE > PHRASES holds five Arabic rhythms for DUNE DRUMS (dum on FRAME, tek on
SLAP, ka on ROPE), one 4/4 bar of 16ths each: MAQSUM, BALADI, SAIDI, AYYUB and MALFUF (the
last two are 2/4, twice). Odd meters (samai 10/8, karsilama 9/8) do not fit a 16-step phrase.

**BREAK.** SLICE's built-in loop is a darbuka maqsum now (dum, tek, ka synthesised at build
time, `tools/gen_waves.py`), the same 22 KB as the rock beat it replaces. CHOP and STUTTER chop
it; projects keep their slice points.

**RISHA.** DESERT OUD's suggested phrase: CARAVAN with every beat plucked twice (RATCH x2),
the oud's down-up stroke. Set a step's RATCH to 2 on any phrase for the same effect.

**TRILL.** ARP MODE TRILL plays each held note, then its upper neighbour in the track's scale
(on BAYATI the half-flat E over D; on CHR a semitone). RATE 1/32 is a trill, 1/16 a slow one;
GATE short for a flutter.

**DRONE.** ARP MODE DRONE keeps the lowest key held sounding, a tanbur's open string, while
the other keys arpeggiate UP over it. The drone moves with the lowest key and ends with the
keys; GATE shapes the notes above it only. Best on POLY.

**CANON.** The ARP family's third page: a second playhead plays the track's pattern half its
LEN behind (one step at least), transposed by a scale interval. 3RD 5TH 7TH OCT and their
negatives count scale degrees (OCT twelve semitones), so on BAYATI a 3RD above D is the
half-flat F. R 3RD .. R -OCT play the pattern backwards from its end, a crab canon. The answer
takes each step's notes at three quarters of their velocity for GATE; TIE holds it, REST and
empty steps let go; RATCH, NUDGE, CHANCE and the drum hits belong to the first playhead. Not on a
kit. Saved with the project (older projects load OFF).

**STRINGS.** FX > REVERB TYPE has a fourth model, STRINGS: sympathetic strings behind what is
played, as an oud's or a tanbur's open courses ring. Four tuned lines follow the lowest note held
on any pitched track, at that note and its third, fifth and seventh degrees of that track's scale
(the maqam's own intervals); with nothing held the last tuning stays. A new lowest note bends the
ringing to its tuning. SIZE is the strings' sustain (about a third of a second up to several), DAMP
their brightness. Mono. Send a little of each track; a lot of a low pad turns into a drone.

**TAPE.** FX > DLY TIME and the tempo no longer jump the echoes: the read head glides to the new
time (two poles, ~90 ms each), so a change bends the repeats' pitch like a tape machine's motor.
The GRAIN delay glides the same way.

**COMB.** An ANALOG track's FILTER TYPE has a fourth setting, COMB: a tuned comb on the
whole part instead of the filter, pitched at the last note. CUT detunes it (64 = the note),
RES sets how long it rings (120 and up: a long metallic ring that outlasts the note). Sitar
and santur colours on a plain saw.

**LOOP.** ENV 2 > LOOP ON restarts a held note's envelope from its sustain each time it gets
there: slow ATK / DEC for a breathing drone, fast for a tremolo.

**INT.** ANALOG's EDIT 2 knob 4 is INT, oscillator 2's interval in semitones (−24..24): fifths
and octaves for a tanbur drone, a fourth on SYNC for a harder qanun. Cutoff key tracking is
fixed (what KTR 64 gave); older sounds load INT as 0.

**GRAIN delay.** FX > DLY 2: PITCH (semitones) and SPRAY turn the delay into a grain delay.
PITCH +12 with low feedback shimmers an oud or ney an octave up; SPRAY smears the repeats
into a cloud. Both 0: the plain delay.

**Names.** With NOTE NAMES on, a maqam row names a single held note the Arabic way: DUKAH,
SIKAH, NAWA … ([the rows](MICROTONAL.md#more-maqamat-ghoulbox-desert)). SABA, KURD, NAHAWAND,
SIKAH, JIHARKAH, ATH KURD and NAWATHAR join RAST, BAYATI, HIJAZ and the others under SCL.

## Recorded instruments

The oud plucks and the ney-air tails are built into the firmware as the SAMPLE sets **OUD**
(SET 3, three zones G2 A2 C3) and **NEY** (SET 2, looped, three zones B4 C5 E5), in place of
Felucca's FLUTE and SAX; a project saved with those plays the desert sets at the same values.
The optional [CC0 sample pack](../assets/desert/README.md) has the same oud and ney tails,
six qanun notes and the tar for the USR slots. Open the
[web editor](https://jrdntnnr.github.io/sardab/webapp/editor/),
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
