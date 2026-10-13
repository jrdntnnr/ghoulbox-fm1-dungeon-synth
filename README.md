# GHOULBOX: FM-1 Dungeon Synth

**SARDAB 2.1 development fork:** Ghoulbox 2.1 features + Felucca 1.5.1 and SLOOP, 42 factory scales, editable maqam tunings, Scala import and tuned chord generation. [Controls and compatibility](docs/MICROTONAL.md) · [Build/validation status](docs/INTEGRATION-STATUS.md). Hardware validation is pending.

![GHOULBOX 1.0: boot, HOME, PRESETS, MENU](docs/ghoulbox-1.0.png)

**GHOULBOX** is dungeon-synth firmware for the **M-VAVE FM-1**: a fork of
[Felucca](https://github.com/hugelton/Felucca) by Hügelton Instruments (1.0.5.2), tuned for dark ambient,
medieval and fantasy synth music, from Old Tower and Hole Dweller to Mortiis-era dungeon synth.

**SARDAB 2.1 development preview.** [Install it](https://jrdntnnr.github.io/ghoulbox-fm1-dungeon-synth/) ·
[watch the original GHOULBOX demo](https://github.com/jasonpersinger/ghoulbox-fm1-dungeon-synth/releases/download/ghoulbox-v1.3/ghoulbox-demo-1.3.mp4) ·
[read the guide (PDF)](docs/guide/GHOULBOX-User-Guide-1.4.pdf)

## What's new

### SARDAB 2.1 — DESERT section and Ghoulbox integration

Open **PRESETS → LIST (knob 4) → DESERT** for all nine of our synthesized
sounds plus **USR1 / USR2 / USR3**. The USR entries play the sample banks you
have uploaded; an empty slot is silent.

Also includes USB rescue, twelve shared voices, drone latch, optional
layer/split, GURDY crank, tuned note labels, temperaments and CAVE/TORCH
textures. Our maqam tuning, chord generation, Scala import and saved-data
migration remain intact. [Controls and compatibility](docs/GHOULBOX-2-INTEGRATION.md)
· [Flashing instructions](docs/FLASHING.md).

### Dungeons of the Desert — microtonal preview 2

**CITADEL**, a Bayati-on-D desert dungeon scene, plus DESERT OUD, BRASS QANUN,
DUNE NEY, DUNE DRUMS and SAND WIND. Three optional **CC0 Oud / Qanun / Ney Air**
banks load from the editor's Samples tab. Includes further oscillator and
sample-chord tuning fixes. [Playing guide](docs/DUNGEONS-OF-THE-DESERT.md) ·
[Samples and credits](assets/desert/README.md).

The current source adds an optional **Persian Tar** bank (Berklee BISA, CC BY):
choose **Tar → Prepare Tar → Upload** in any editor sample slot. The emulator
also offers **Load Tar on selected track**. [Tar credits and instructions](assets/desert/README.md#persian-tar--cc-by).

Four synthesized voices are also included in the current source: **SANTUR** and
**DUSK SANTUR** in PHYS, plus **MIJWIZ** and **ARGHUL** in TRIO. These are compact
adaptations of published Quilcom instrument designs using our existing DSP;
they use no sample slots. [Controls, provenance and limitations](docs/SOURCED-SYNTHS.md).

### 1.4

- **Power-on scenes.** Hold HOME for the menu and pick **SYSTEM > SCENE** to choose what the FM-1 plays
  when you switch it on:

  | Scene | What plays |
  |---|---|
  | **DUNGEON** | The default: hurdy-gurdy, crypt pad, monks, drone. 72 BPM. |
  | **CRYPT** | Low drone, crypt choir, cave wind, slow tomb drums. 60 BPM. |
  | **TAVERN** | Dance gurdy, harp arpeggio, lute, a light drum kit. 96 BPM. |
  | **CHAPEL** | Cathedral organ, monks, a recorder lament, the vielle. 66 BPM. |
  | **BLANK** | DUNGEON's four sounds with no notes, for starting from nothing. |

  The firmware editor protocol exposes SCENE at menu setting ID 25.
- **A hurdy-gurdy icon** for the GURDY engine, with its wheel, keybox and crank.

### 1.3

- **The hurdy-gurdy, reworked.** The trompette's buzz now cuts through the drones, and the six GURDY
  sounds each have their own character: HURDY GURDY, DRONE WHEEL, DANCE GURDY, TROMPETTE, VIELLE and
  BAGPIPE.
- **MIDI control of the effects and the LFO**: distortion send, LFO rate, vibrato, fade-in, filter and
  tremolo depth, reverb size and damping, delay time, feedback, tone and mix, chorus rate and depth.
  The full CC table is in the guide (chapter 20).
- **A 48-page user guide** with dark pages, for beginners and old hands alike.
- **A [2½-minute demo](https://github.com/jasonpersinger/ghoulbox-fm1-dungeon-synth/releases/download/ghoulbox-v1.3/ghoulbox-demo-1.3.mp4)**
  in four styles (crypt, tower, tavern, crypt again), rendered by the firmware's own sound and screen code.

### 1.2

- **CRYPT drums**: frame drum, rope snare, hand slap, shaker, jingles, timpani, stick and gong, with the
  slow TOMBBEAT pattern.
- **MIDI CC control** of level, pan, envelope, sends, the engine knobs, CRSH, TAPE and a master filter.
- **Five new patterns**, two new sounds (LOW DRONE and SHAWM), and USB SERIAL off by default.

Legacy projects and presets are migrated; settings backups need the explicit conversion described in [compatibility](docs/MICROTONAL.md). Full notes are on the
[Releases](https://github.com/jrdntnnr/ghoulbox-fm1-dungeon-synth/releases) page.

## What GHOULBOX adds to Felucca

Felucca's sequencer, song mode, effects, editor and backups all work as they do there. On top of them:

- **A dungeon at power-on.** The FM-1 starts in D minor at 72 BPM, ready to play: press PLAY. Four other
  scenes are in MENU > SYSTEM > SCENE.
- **HALL reverb**: a third REVERB TYPE beside ROOM and SPRING, 1.5 s stone chamber to 12 s cathedral, on at
  power-on; it borrows the delay's buffer, so it needs no extra RAM.
- **TAPE and CRSH** (FX > TAPE): the mix through a worn cassette (wow, flutter, saturation, hiss) and a
  cheap sampler's bits and rate.
- **GURDY**, a hurdy-gurdy engine: a bowed melody string, drones you tune, and the trompette's buzzing
  bridge struck in time, in six sounds.
- **Real recordings**: a bowed psaltery, a Renaissance chamber organ, a baroque tenor recorder and a folk
  harp, sampled from public-domain (CC0) libraries, on the SAMPLE engine.
- **45 dungeon sounds**: crypt pads, string machines, monk, crypt and glass choirs, a low drone voice, cathedral,
  reed and pipe organs, lute, harpsichord and virginal, a dungeon harp, tower and lo-fi flutes, recorder, French
  horn, mournful and grim brass, tuba, dark strings, cave wind, torch crackle, a bagpipe, a shawm and five
  hurdy-gurdies, two crypt drum kits, plus the four recordings; seven of them are new 6-operator FM patches.
- **CRYPT drums**: a dungeon kit on the DRUM engine with the TOMBBEAT pattern.
- **Nine dungeon patterns** (SEQ > PHRASES), in D minor over the power-on scene's chords: DIRGE, BALLAD,
  CHANT, PEDAL, MARCH, LAMENT, ARPEGGIO, ANTIPHON and TOMBBEAT.
- **MIDI CC control**: a keyboard or DAW can turn level, pan, envelope, sends, the four engine knobs, the
  LFO, the reverb, delay and chorus settings, CRSH, TAPE and a master low/high-pass filter.
- **A curated list**: 59 sounds across 13 engines. The pop, techno and percussion presets and the SLICE
  engine are retired from the menus; projects and user presets that use them still load and play them.
- **USB SERIAL off by default**: the FM-1 shows up as plain audio and MIDI; turn the developer
  console on in the menu if you want it.
- **An old-RPG screen**: VT323 pixel type, stone windows, a sword cursor, flickering wall torches, and the
  CRYPT palette (stone, bone, torch amber, blood red), a blackletter wordmark and a skull at boot.

Version 1.4 (HOME held > ABOUT). A hobby project, tested on one FM-1: **install at your own risk.**

**[User guide (PDF)](docs/guide/GHOULBOX-User-Guide-1.4.pdf):** a 48-page manual: the FM-1's hardware and
installing; synth basics in plain words (oscillators, filters, envelopes, LFOs, effects, sequencing), each
with a hands-on exercise; a full reference to every page and all 59 sounds; a first piece built step by
step, a minor-key chord cheat sheet and dungeon recipes; recording, troubleshooting and a glossary.

## Install

In Chrome or Edge on a computer, with the FM-1 on a USB **data** cable and no other music app open:

**[jrdntnnr.github.io/ghoulbox-fm1-dungeon-synth](https://jrdntnnr.github.io/ghoulbox-fm1-dungeon-synth/)**

Before installing, save anything you need to keep. On firmware that supports it,
use the editor's **Complete backup → Save backup**, then close its MIDI connection.
The normal **Install** button does not automatically create a backup; stock M-VAVE
firmware does not support this editor's complete backup protocol.
Press **Install** and allow MIDI/SysEx access. Keep USB connected until **Done** and
the FM-1 restarts. If the installer reports that it stopped in update mode, replug
the USB cable and press Install again: the FM-1 stays in update mode until the
install is finished.

To go back, install Felucca from [its installer](https://hugelton.github.io/Felucca/) and restore the
backup in its editor; its "Return to official V15" returns to M-VAVE's firmware. The package (and the user
guide) are also on the [Releases](https://github.com/jrdntnnr/ghoulbox-fm1-dungeon-synth/releases) page.

Something wrong, or a sound you want? [Open an issue](https://github.com/jrdntnnr/ghoulbox-fm1-dungeon-synth/issues/new); plans are in the [roadmap](ROADMAP.md).

To build it yourself (JieLi toolchain and AC79 SDK, see [BUILDING.md](BUILDING.md)):

```
./build.sh
python3 tools/ghoulbox_serve.py    # http://localhost:8000/webapp/installer/
```

## Sound auditions

The firmware's own audio and screen code runs on a computer, so a change can be heard and seen before
flashing:

```
gcc -O2 -w -Ibuild/gen -Ifirmware/src -o build/host/dungeon_demo tests/dungeon_demo.c -lm
build/host/dungeon_demo build/dungeon_demo     # the power-on scene, passages, every GHOULBOX preset
gcc -O2 -w -Ibuild/gen -Ifirmware/src -o build/host/preset_preview tests/preset_preview.c -lm
build/host/preset_preview build/preset_preview # every factory preset on its own pattern
gcc -O1 -w -Ibuild/gen -Ifirmware/src -Itests -o build/host/ghoulbox_shots tests/ghoulbox_shots.c -lm
build/host/ghoulbox_shots build/gb_shots       # the screenshots above (PPM)
```

## Credits and licence

GHOULBOX changes © 2026 Jason Persinger ([@jasonpersinger](https://github.com/jasonpersinger)),
GPL-3.0-only, like Felucca. It is built entirely on **Felucca** by Leo Kuroshita / Hügelton Instruments
(GPL-3.0-only); please support the original ([GitHub Sponsors](https://github.com/sponsors/hugelton),
[itch.io](https://hugelton.itch.io/felucca)). GHOULBOX adds the VT323 font (Peter Hull, SIL OFL 1.1) and
the Pirata One font (Rodrigo Fuenzalida, Nicolas Massi; SIL OFL 1.1); see [LICENSING.md](LICENSING.md).
M-VAVE and FM-1 are trademarks of their owners; GHOULBOX is not affiliated with or endorsed by them or by
Hügelton Instruments.

Felucca's own README follows.

---

# Felucca

[![License: GPL-3.0-only](https://img.shields.io/badge/license-GPL--3.0--only-blue.svg)](LICENSE)

![Felucca 1.0](docs/felucca-1.0.png)

Multi-engine synthesizer firmware for the M-VAVE FM-1, **version 1.6**. Connect the FM-1 to a computer by USB, open the
[web installer](https://hugelton.github.io/Felucca/) in Chrome or Edge and press Install; no extra hardware is needed.
Installing is at your own risk: M-VAVE's updater or the installer's **Return to official V15** takes you back.
Want to look around first? [Try it in your browser](https://hugelton.github.io/Felucca/webapp/try/), no FM-1 needed.

- Install: [web installer](https://hugelton.github.io/Felucca/), or `tools/fm1_install.py` from a terminal
- Edit: [web editor](https://hugelton.github.io/Felucca/webapp/editor/) (developed in [Felucca-WebApp](https://github.com/hugelton/Felucca-WebApp))
- Read: the manual is in the [wiki](https://github.com/hugelton/Felucca/wiki/Home): [Installation](https://github.com/hugelton/Felucca/wiki/Installation) ·
  [Controls](https://github.com/hugelton/Felucca/wiki/Controls) · [Menu](https://github.com/hugelton/Felucca/wiki/Menu) ·
  [Engines](https://github.com/hugelton/Felucca/wiki/Engines) · [Release notes](https://github.com/hugelton/Felucca/wiki/Release-Notes) ·
  [Troubleshooting](https://github.com/hugelton/Felucca/wiki/Troubleshooting)
- Build: [BUILDING.md](BUILDING.md)

<a href="https://hugelton.itch.io/felucca"><img src="https://static.itch.io/images/badge-color.svg" alt="Available on itch.io" width="74"></a>

## What it is

Thirteen engines with factory presets, four tracks (8 voices shared), a 64-step sequencer (chords, ties, chance, conditions,
ratchets, nudge, locks, automation, live and step recording), SEQ TOOLS, songs, effects, a MOD matrix, MIDI (USB and TRS,
per-track channels, MIDI LEARN) and USB audio. More: [Features](https://github.com/hugelton/Felucca/wiki/Features).

## New in 1.6

- **The SEQ piano roll follows the play position** when the pattern is longer than one screen
- **Conditional steps** (1:2 … 4:4, FILL), **EUCLID, DOUBLE and HALF**, **a MIDI channel per track**, PHASE FOLD, three more drum kits and humanize in the MOD matrix (1.5.5 and 1.5.6 were never released: 1.6 carries them)
- ⚠ A project with conditional steps does not open on 1.5 or 1.5.1; projects saved by 1.4 or later do not open on 1.1.5.x

All changes and what to know before going back: [Release notes](https://github.com/hugelton/Felucca/wiki/Release-Notes) · [Upgrading from 1.1.5](https://github.com/hugelton/Felucca/wiki/Upgrading-from-1.1.5).

## Support and contributing

## New in 1.4.1

- **Engine quick actions on the EDIT layer's black keys.** Hold EDIT: the black keys do what the selected track's
  engine offers, and the map shows it. On a DRUM track, black keys 1–8 (F#3 … A#4) mute its lanes KICK, SNARE, CLAP,
  HATCL, HATOP, TOM, RIM and BELL, one press to mute, the next to unmute. The LEDs show which lanes sound, the drum
  grid dims a muted lane. A muted lane keeps playing unheard, so unmuting brings it back in time. The mutes are for
  live play: never saved, not recorded, not undone; a sound or project load clears them, OCT− in the layer puts them
  back as they were when it opened. Other engines have no quick actions yet.
- **FM6 voice bank** (Discussion #168). The web editor's 6-OP FM tab sends a whole 32-voice SysEx bank (the standard
  4096-byte file) to the FM-1 in one go, with its name. On an FM6 track SLOT (EDIT 2, KNOB 4) now goes F1–F8, OWN,
  then **B1–B32**, the bank's voices; the algorithm chart shows the voice's name. Picking a voice copies it into the
  track, as F1–F8 do, so a project or user preset saved with it keeps that sound after you send another bank (it
  loads with SLOT OWN). With no bank sent, SLOT stops at OWN as before. The bank has its own flash sector and is only
  replaced once a new one arrived whole; projects, presets, samples and settings are not touched. **The full backup
  does not include the bank: keep the .syx file.**
- 1.4.1 opens and saves everything 1.4 does, in the same formats.

## ⚠ Coming from 1.1.5

1.4 is the first release since 1.1.5.1 and holds a lot (all of this holds for 1.4.1 and 1.5 too). Everything you saved loads and sounds as before, except
where marked here:

- **Projects are saved in a new format (FUN10).** 1.4 opens every older project, but **1.1.5.x and older cannot
  open a project saved by 1.4 or later**: they show its slot as empty (the data stays in flash until you save something into
  that slot there), and refuse a backup that holds one. User presets saved by 1.4 load on 1.1.5.x without the five
  new values (LFO 2's SYNC, TRIG and POL, QUANTIZE, SPREAD).
- **ARP ORD now plays the held notes in the order you pressed them** (up to 1.1.5 it played them as UP): a sound
  or project using ORD sounds different when its notes were pressed in another order than low to high.
- **SAMPLE's PIANO** was a lighter lo-fi piano in 1.4 and 1.4.1; since 1.5 it is the full piano of 1.1.5 again.
- **The pages moved** (Discussion #153): see Controls below. In short: SEQ is STEP and AUTOMATION; **SEQ held is SEQ
  TOOLS on every page** (it no longer opens SONG); **GLO opens SONG**; PHRASES is on SAVE; HOME goes round HOME,
  MIXER and CLOCK (BPM, swing, clock source); TUNE is MENU > AUDIO, MIDI IN (ROUT) is MENU > MIDI, and the USB state
  and CPU load are MENU > SYSTEM > INFO. REC held is the REC layer as before.
- **SONG has a lane per track**: older songs load with all four tracks on each row's slot and play as before; its
  knobs are KNOB 1 SECTION, 2 TRACK, 3 PAT, 4 REPS, and on SONG the white keys A3 B3 C4 D4 set the slot.
- **A fresh start uses the HALL reverb** (up to 1.1.5: ROOM). Saved projects keep their reverb TYPE.
- **BASS+ (MENU > SPEAKER EQ) is softer:** its added harmonics are at half the level, so a full-scale kick no longer
  distorts (#180).
- **The web editor** needs its update for the new features; use the one on the site.

## Controls

![FM-1 controls with Felucca](docs/controls.jpg)

- Tap a page button for its page, again for the next; HOME returns home. Page buttons act when let go
- Hold FX, GLO, SCL, EDIT, REC or SEQ for its quick layer; hold SAVE to undo, HOME for the menu; hold LFO and turn a
  knob for a MOMENTARY change
- Double-tap a layer's button to keep its layer open without holding; tap it again to close
- On SEQ > STEP, hold a step and turn KNOB 1–4 to lock those parameters on that step (PRESETS: its NUDGE)
- On action pages and in dialogs, OCT+ does it and OCT− goes back; in the menu, OCT+ / OCT− change the value and HOME closes it
- Save a sound: stop, tap SAVE, pick a slot with KNOB 1, then OCT+ and OCT+ again (name it with the keys)

The pages by button:

| Button | Taps go round | Held |
|---|---|---|
| HOME | HOME, MIXER, CLOCK (BPM, swing, clock source) | the menu |
| SEQ | STEP (the piano roll, or the drum grid), DETAIL, AUTOMATION | SEQ TOOLS, on every page |
| GLO | SONG | the GLO layer (mutes, solos, levels, TAP tempo, D4: MIDI LEARN) |
| SAVE | USER, PRESETS, PHRASES (the pattern loader), PROJECT, TOOLS | UNDO |
| LFO | LFO, LFO 2, LFO DEST, MOD | MOMENTARY (with a knob) |
| EDIT | EDIT 1, EDIT 2, VOICE, VOICE 2, VOICE 3 (FM6 adds OPERATOR, OP ENV, OPERATOR 2; DRUM LANES; SLICE SLICES; ANALOG FILTER) | the EDIT layer (the engines; on DRUM the black keys mute lanes) |
| FX, SCL, ENV, ARP | their pages (FX: INSERT and INSERT 2 after the SLICER) | FX and SCL: their layers |
| REC | arms the track | the REC layer (CLEAR, CLICK, COUNT-IN, CLICK LEVEL) |

## Menu

Hold **HOME** for the menu, in five tabs: **DISPLAY** (COLOR, STYLE, LARGE, ANIM, LEDS, SCREEN OFF, SCOPE, HOME),
**CONTROL** (HOLD, KNOB ACCEL, FX LATCH, BPM LOCK, SCALE LEDS, STEP PREVIEW, CHORD ENTRY), **AUDIO** (SPEAKER EQ,
USB LEVEL, CLICK, CLICK LEVEL, COUNT-IN, TUNE), **MIDI** (MIDI IN, LEARN CLEAR) and **SYSTEM** (USB SERIAL,
RESTORE LAST, HELP, HARDWARE CALIBRATION, INFO, ABOUT).
ALGORITHM moves between the tabs and PRESETS through the rows of one; any of KNOB 1–4, or OCT+ / OCT−,
changes the value (OCT+ opens HARDWARE CALIBRATION, INFO and ABOUT); press HOME to close the menu. Every new
setting defaults to the earlier behaviour. The web editor's Settings tab reads and changes them too, saved the same
way as from the menu, in the same tabs.

- **COLOR:** the palette. GREY (grayscale, called MONO up to 1.0.1), GREEN, AMBER, ICE, VIOLET, ROSE,
  PAPER (light), HI-CON (high contrast), NIGHT (the 0.9 look: true black, green-tinted text) and MONO
  (black and white)
- **STYLE:** FLAT (filled cards) or LINE (areas divided by thin lines)
- **LARGE:** OFF or ON: on HOME and the value pages the four knob cards grow tall, marked K1–K4, with
  larger labels and values about twice the size; the graph below becomes a strip. Lists, the piano roll,
  the drum grid and the quick layers keep their layout with larger labels
- **ANIM:** ON, OFF or IDLE. OFF shows every change at once, without rolling digits, a gliding piano roll or
  the LED sweep at power-on. IDLE is ON plus a gentle LED animation after a minute without input: a soft light drifts
  over the keys and the buttons breathe; any key, button or knob ends it and does what it always does. It never runs
  while playing, with a track armed, in a layer, the menu or a dialog
- **LEDS:** OFF (no glow), DIM LO, DIM HI (default) or INV (the idle LEDs lit, the active ones dark).
  What can be pressed breathes up to about 60 % of a lit LED, about 30 % with DIM LO
- **SCREEN OFF:** NEVER (default), 5 MIN, 15 MIN, 30 MIN or 60 MIN: after that long with no button, key or knob
  touched, the screen goes dark; the sound, the sequencer, MIDI and USB go on. The backlight stays on (on the FM-1
  its line also enables the buttons and keys). The next button, key or knob only turns the screen back on
- **SCOPE:** OUT (default) or MIX: what HOME's oscilloscope shows. OUT the sound after MASTER (the picture shrinks
  with the volume), MIX the mix before MASTER, the same size at any volume
- **HOME:** SCOPE (default), TRACKS or LEVELS: under HOME's four cards, the oscilloscope or the four tracks, a row
  each with its sound, its steps playing, MUTE and a meter. With TRACKS only the look changes; with LEVELS KNOB 1–4
  set the LEVEL of T1–T4 (the engine's main parameters are on EDIT)
- **HOLD:** how long a layer's button is held before its map shows
- **KNOB ACCEL:** OFF (one step per click) or ON: a fast, steady turn of a wide value moves 2 to 4 steps
  per click, up to 8 on values of more than 64 steps, the FX and GLO layers' knobs included; lists never
  jump. ON or OFF, every click of a fast turn counts
- **FX LATCH:** ON, FX + an effect key turns the effect on until pressed again, the knob macros stay
  where you leave them, and FX + OCT− turns everything off
- **BPM LOCK:** ON, SELECT no longer changes the tempo; hold GLO and turn SELECT, tap F4 in the GLO
  layer or use HOME > CLOCK instead
- **SCALE LEDS:** OFF or ON: the keys show the selected track's scale all the time (the root a little
  brighter); not on DRUM or SLICE tracks
- **STEP PREVIEW:** OFF (default) or ON: on SEQ > STEP with the transport stopped, moving the cursor plays the step
  it lands on once (its notes or hits, through the track's sound)
- **CHORD ENTRY:** HOLD (default) or ADD: how step recording writes. HOLD: the keys held together become the step,
  letting go moves on. ADD: each key adds its note to the step, up to 4 (a key whose note is there takes it out),
  and KNOB 1 moves on, so a chord goes in one finger at a time
- **SPEAKER EQ:** FLAT, LOWCUT or BASS+, a tone setting for the built-in speaker (not a switch). It also
  shapes the headphone out and USB audio, so keep it on FLAT when recording. The firmware cannot turn
  the speaker off, but headphones in the jack do
- **USB LEVEL:** MASTER (the MASTER knob sets the USB audio level too) or FIXED (USB always at full
  level, MASTER sets only the speaker and headphones)
- **CLICK:** OFF (default), REC (while a track is armed) or ON: a click on every beat while playing, the
  bar's first beat higher. **CLICK LEVEL:** LOW, MID (default) or HIGH; MASTER sets it too
- **COUNT-IN:** OFF (default), 1 BAR or 2 BARS: with a track armed, PLAY counts in first (clicking even with
  CLICK OFF), and a note played in the last half beat lands on step 1. Not with an external clock
- **TUNE:** the global tuning, −50 to +50 cents. It is the project's, saved and loaded with it
- **MIDI IN:** CH1-4 (default), SEL, CH5-8, CH9-12 or CH13-16 (see MIDI above). The project's too
- **LEARN CLEAR:** how many CCs MIDI LEARN has set (NONE, 1 CC … 16 CC); OCT+ clears them all
- **USB SERIAL:** ON or OFF, applied when the menu closes (the FM-1 reconnects). OFF leaves out the
  serial console, a developer tool, so the FM-1 is a plain audio + MIDI device; this lets macOS 13–15
  see its USB audio input. MIDI, the editor and the installer work either way
- **RESTORE LAST:** ON (default) or OFF. ON, the FM-1 starts with the music you left (the header says
  RESTORED), kept in an autosave apart from the four projects; it is written when the music changed, the
  transport is stopped and nothing has been touched for 10 s, at most once a minute. OFF starts with the
  power-on sounds
- **HELP:** OFF (default) or ON: a short hint in the footer for about 2 s when a page opens, and under a quick
  layer's map while it shows
- **HARDWARE CALIBRATION**, **INFO** (the version, the USB link and the CPU load) and **ABOUT**

## Engines

In the order the device lists them:

- **ANALOG**: virtual analog; two oscillators (WAVE SYNC: the second hard-synced to the first; SUB: a square an
  octave down), noise, drive, a resonant filter: low-pass, band-pass or high-pass (EDIT > FILTER)
- **FM6**: classic 6-operator FM (Dexed-based): 32 algorithms, a full patch per track, edited in the
  web editor (which imports .syx files) or on the device: EDIT > OPERATOR (ratio or fixed frequency, fine, level),
  OP ENV (each stage's rate and level, drawn as the envelope) and OPERATOR 2 (mode, detune, velocity), one
  operator at a time on the algorithm chart; macros on top; SLOT picks a factory patch (F1–F8), the track's own (OWN) or a voice of the bank the web editor sent (B1–B32)
- **PHASE**: phase distortion (ported from CrispyZebra)
- **LOFI**: chiptune; pulse, triangle, saw, noise and a 4-bit wave RAM, stepped envelope, sweep, arpeggio
- **SAMPLE**: multisampled instruments (PIANO, FLUTE, SAX) and 3 user sample slots
- **VOICE**: formant oscillator, sung vowels
- **TRIO**: 3 oscillators with ring modulation and sync, multimode filter
- **WHEEL**: tonewheel-style organ; drawbar registrations, percussion, key click, drive, rotary speaker
- **GRAIN**: granular textures from the built-in samples or a user slot
- **PHYS**: physical models: modal resonators, strings, struck membranes, sympathetic strings
- **NOISE**: noise from analog to digital: colours, crackle, shift-register and metallic tones
- **SLICE**: a drum break, a piano note (both built in) or your own sample cut into slices, one per key;
  set the slices by hand on the SLICES page
- **DRUM**: an 8-lane kit of Felucca's own drum voices on the General MIDI key map. KIT picks the
  standard kit or one of five virtual-analog (VA) kits in the manner of
  classic analog drum machines (the numbers are a hint), every lane its own voice: **80** (deep sine kick with a long decay, noisy snare,
  six-square hats), **10** (swept kick, white-noise snare, long-tailed claps, a cymbal), **66** (soft
  round kick, bright snare, noise hats, a conga), **55** (dropping kick, high metal hats, a metal bell)
  and **77** (swelling kick, multi-burst claps, claves, a cymbal). All of them are synthesized by
  Felucca, no samples. EDIT > LANES and LANES 2 set each lane's level

The DIGITAL engine of 0.9 has been replaced by FM6: projects and presets with DIGITAL sounds load
as FM6 sounds converted from them. The SAMPLE engine's PERC kit was removed in 1.0.2: sounds and
projects that used it load as the DRUM engine's kit, on the same key map. FM6's patch bank (the B
slots) was removed in 1.0.3: user presets keep their own FM6 patch, and presets that used a B slot get
that patch on the first start of 1.0.3. 1.4.1 brings B slots back as a voice bank sent from the web editor
(B1–B32): a voice picked from it becomes the track's own patch, so nothing saved depends on the bank. SLICE gained a second built-in sound in 1.0.4, PIANO (the SAMPLE
engine's middle C as it was then), next to BREAK. Since 1.0.4 a missing sample (an empty user slot, or a set missing
from the build) plays a plain sine at the note's pitch on SAMPLE, GRAIN and SLICE, and the screen says
NO SAMPLE once. DRUM's KIT variants HAND, CYM and H+CYM were retired in 1.0.5: sounds and projects that
used them play the 66, 10 and 77 kits. SAMPLE's PIANO was a lo-fi piano in 1.4 and 1.4.1 (the full one installed into a user slot from the web editor as
PIANO HD); 1.5 brings the full piano back.

**SLICER** (FX page, every track): a tempo-synced 16-step gate or stutter, with 16 patterns.

**Reverb** (FX > REVERB, TYPE): **HALL** (since 1.4, the default on a fresh start: eight delay lines feeding each
other with slowly drifting taps, stereo, SIZE the decay up to about 4.5 s, DAMP how fast the highs die), **ROOM** (the
default up to 1.1.5) and **SPRING**. A project plays the TYPE it was saved with.

## Scale keyboard

On **SCL**, choose **ROOT**, **MAQAM** and **MAP**. **CHR** keeps the familiar chromatic keyboard with real per-note tuning. **WHITE** maps successive scale degrees onto white keys and leaves black keys silent. **ALL** maps successive degrees onto every key, including black keys, extending the melodic range of the small keyboard. WHITE and ALL anchor the physical C4 key to ROOT; TRN and the octave buttons shift the resulting notes. ALL leaves notes outside MIDI's range silent.

**SNAP** rounds keys down into the scale; **SEQ** also snaps sequencer playback without changing the stored steps. Neighboring keys can converge in these modes. Keep **CHORD OFF** for independent melody notes, and sequence generated chords on another track. Existing QNT OFF projects display **MAP CHR**; saved IDs and tuning are unchanged. See the [mapping and tuning guide](docs/MICROTONAL.md).

Available scales: chromatic (CHR), major (MAJ), natural minor (MIN), Dorian (DOR),
Mixolydian (MIX), major pentatonic (PEN), minor pentatonic (MPEN), harmonic minor
(HARM), Phrygian (PHRY), Lydian (LYD), Locrian (LOC), ascending melodic minor (MEL),
minor blues (BLUES), whole tone (WHOLE), half-whole diminished (DIMHW), and
whole-half diminished (DIMWH). Scales with other than seven notes continue across
the white keys without repeating notes; their roots need not fall on every C key.
Drum kits and incoming MIDI keep their own note mapping.

Press **SCL** again for the **CHORD** page: CHRD picks the chord keys (OFF, the scale's triads or
sevenths, or a fixed shape) and VOIC the voicing. DIA3/DIA7 show a **CHORD SNAP** message when they move a played root into the scale, even with MAP CHR.

## Layout

| Path | What |
| --- | --- |
| `firmware/` | firmware sources: `src/` app, `hal/` hardware layer, `loader/` update loader |
| `tools/` | build script, generators, package maker, installer and sample uploader |
| `assets/` | UI font, icon names, CC0 instrument samples |
| `web/` | the web installer, the earlier editor and the browser emulator (`web/emu/`); the new editor: [Felucca-WebApp](https://github.com/hugelton/Felucca-WebApp) |
| `tests/` | tests that run on the build machine |
| `LICENSES/` | licence texts of the bundled fonts, icons, ported DSP and SDK files |

## If the FM-1 does not start

If an update is interrupted and the FM-1 stays black, check whether a computer sees it as a USB device named
**WL80UBOOT** (or a USB mass-storage device with ID 4C4A:8057). That is the chip's built-in boot mode, and
the FM-1 can be brought back:

- First try another USB data cable, and close every other app that uses MIDI, then run the web installer again.
- If it stays in boot mode, [FM-1 Transporter](https://github.com/kurogedelic/FM-1-transporter) reads and
  writes the FM-1's flash from a Mac through a Seeed XIAO RP2040 (three wires to the FM-1's USB lines). Back up
  the flash first, then write the official firmware (M-VAVE's FM-1.fwsc).
- Questions: [Issues](https://github.com/hugelton/Felucca/issues).

## Support

If Felucca is useful to you, [sponsoring on GitHub](https://github.com/sponsors/hugelton) or a donation
on [itch.io](https://hugelton.itch.io/felucca) helps keep its development going.

Issues are for reproducible bugs (one per issue). Ideas and requests go to
[Discussions](https://github.com/hugelton/Felucca/discussions), and feature requests posted as issues will be
moved there. Pull requests are welcome: see [CONTRIBUTING.md](CONTRIBUTING.md).

## AI disclaimer

Felucca is developed with the assistance of AI coding agents (code, tests, documentation, translation, maintenance). The
design, features, sound design and direction are the maintainer's or the community's, and no generative AI is used for music,
icons or artwork. See [On AI-Assisted Development and Responsibility](https://github.com/hugelton/Felucca/discussions/166).

## Credits and licence

Free software: [GPL-3.0-only](LICENSE), the Hügelton Sample Pack included; the bundled fonts and ported DSP keep their own licences
([LICENSES/](LICENSES/), [LICENSING.md](LICENSING.md)). Thanks to everyone whose work and contributions are in it: [CREDITS.md](CREDITS.md).

M-VAVE and FM-1 are trademarks of their respective owners. Felucca is not affiliated with or endorsed by them.

Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments
