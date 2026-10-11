# Dungeons of the Desert — optional CC0 banks

These small, mono banks complement the built-in modeled desert presets. Uploading
firmware does not install them. In the web editor's Samples tab, use **Prepare Oud**,
**Prepare Qanun**, or **Prepare Ney Air**, preview the notes, then **Upload**. Upload
replaces only the selected USR slot, after confirmation. Back up your existing
samples first if you want to keep them. On a SAMPLE track choose SET USR1 (Oud),
USR2 (Qanun), or USR3 (Ney Air). Set ROOT and SCALE on that track; the bank itself
is calibrated to ordinary MIDI roots and the engine applies the maqam offsets.

GRAIN can use the same user banks for sustained, spectral textures. SAMPLE plays
these recordings as one-shots; no sustain loops are provided. The CITADEL scene
uses modeled voices and works with empty user slots. To use a recording in it,
switch the desired track to SAMPLE and select its user SET, then set Bayati / D.
The modeled presets are interpretations, not acoustic emulations.

| Bank | Suggested slot | Recording |
| --- | --- | --- |
| OUD | USR1 | Three oud plucks, G2 / A2 / C3, by hammondman |
| QANUN | USR2 | Six notes from a Turkish kanun recording by barisbozkurt |
| NEY AIR | USR3 | Three short held-note tails from aliaref's ney phrase |

All source recordings are CC0 1.0. [sources.json](sources.json) records author,
original page, public HQ preview URL, license link and source SHA256. We used the
public MP3 previews, not the original uncompressed recordings. Edits: mono
22,050 Hz, isolated notes, DC removal, median-pitch calibration, fades, peak
normalization, and IMA ADPCM encoding. Derived audio is also dedicated under
[CC0 1.0](https://creativecommons.org/publicdomain/zero/1.0/).

Ney Air preserves audible breath and pitch movement from a performance. Its
median is calibrated, but it is not a vibrato-free tuning reference. DUNE NEY
is the built-in, steady-pitch alternative. Wide transpositions of any sparse bank
change its timbre; these are compact creative resources, not full multisamples.

[banks.json](banks.json) records cuts, roots, calibration and measured pitch span.
Rebuild with `python3 tools/build_desert_banks.py build/desert-sources` after
downloading and decoding the verified sources (instructions in the script).
The `.hdr` / `.bin` files use the existing FM-1 user-slot format. WAV files can
also be imported manually into the editor; root names follow C4 = MIDI 60.
