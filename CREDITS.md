# Credits

Felucca is Leo Kuroshita's ([@kurogedelic](https://github.com/kurogedelic), [Hügelton Instruments](https://hugelton.com)). It uses and
thanks the work below; each keeps its own licence (see [LICENSE](LICENSE), [LICENSING.md](LICENSING.md) and [LICENSES/](LICENSES/)).

- **[Hügelton Instruments](https://hugelton.com)** (Leo Kuroshita, [@kurogedelic](https://github.com/kurogedelic)):
  Felucca itself; the PHASE engine's waveforms (a C port of the oscillator of
  [CrispyZebra](https://github.com/hugelton/CrispyZebra), GPL-3.0); the DRUM voices and kits; the Hügelton Sample
  Pack (the drum samples, GPL-3.0-only, not CC0); the [Fukiai](https://github.com/hugelton/Fukiai) icon
  font ([MIT](LICENSES/MIT-Fukiai.txt))
- Fonts: [Inter Tight](https://github.com/rsms/inter-tight) by The Inter Project Authors, [SIL OFL 1.1](LICENSES/OFL-InterTight.txt);
  the browser emulator's labels: [DotGothic16](https://github.com/fontworks-fonts/DotGothic16) by The DotGothic16 Project Authors, [SIL OFL 1.1](LICENSES/OFL-DotGothic16.txt)
- Samples: the built-in OUD (plucks by hammondman) and NEY (held tails from aliaref's phrase) sets, CC0 1.0 ([sources](assets/desert/sources.json)); [Versilian Studios](https://versilian-studios.com/) [VSCO-2 Community Edition](https://github.com/sgossner/VSCO-2-CE) and [VCSL](https://github.com/sgossner/VCSL), CC0 1.0: the SAMPLE sets, also SLICE's PIANO and PIANO HD ([attribution](assets/samples-cc0/ATTRIBUTION.txt))
- VOICE engine: after [klattsch](https://github.com/tgies/klattsch) by Tony Gies (MIT); formant data from Klatt (1980) and Hillenbrand et al. (1995)
- PHYS engine: models ported from [DaisySP](https://github.com/electro-smith/DaisySP) by Electrosmith and Emilie Gillet ([MIT](LICENSES/MIT-DaisySP.txt)) and from Emilie Gillet's [eurorack](https://github.com/pichenettes/eurorack) code ([MIT](LICENSES/MIT-Rings.txt))
- FM6 engine: msfa from [Dexed](https://github.com/asb2m10/dexed) by Google Inc. and Pascal Gauthier ([Apache-2.0](LICENSES/Apache-2.0-msfa.txt))
- Browser emulator: after [X0X](https://github.com/charlesvestal/fm1-x0x) by [charlesvestal](https://github.com/charlesvestal) (GPL-3.0), a Felucca fork whose browser build showed the way
- Second core, COMB filter, ENV LOOP, ANALOG INT, GRAIN delay: ported from [JIANT](https://github.com/juanjiant-bit/JIANT) (GPL-3.0), a Felucca fork; its second-core start and guards after [Melodee](https://github.com/keremimo/melodee) by keremimo and X0X (GPL-3.0)
- Supply startup (the stock V15 rails before the second core, `hal/fm1_power.h`, its test): from [Melodee](https://github.com/keremimo/melodee) 1.3.6 by Kerem Kilic (GPL-3.0), as it came
- From Felucca 1.6.1: the second core held with the bus traps off before a reset or update, a package for another board refused before any erase, MENU > LARGE's one face per knob (#251); from JIANT 0.6.74: a voice given up fades over 4 ms
- Package format and boot files: [JieLi AC79 SDK](https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK) ([Apache-2.0](LICENSES/Apache-2.0.txt); three of its files are in every package, none in this tree)
- Contributions: [keremimo](https://github.com/keremimo) (white-key scales, #2), [ChanceTheMaker](https://github.com/ChanceTheMaker)
  (TRS MIDI, bend, sustain and clock, palettes, favourites, editor display settings: #8, #10, #11, #12),
  [andreahaku](https://github.com/andreahaku) (sample recording and trim, #29; SLICE manual slices and tests, #27, #22),
  [spinkham](https://github.com/spinkham) (the boot fix for 0.9 projects, #111; the emulator's MIDI knobs, #182),
  [ugai](https://github.com/ugai) (the Try page's horizontal knob drag, #240),
  [zednaked](https://github.com/zednaked) (ratchets, #100),
  [jasonpersinger](https://github.com/jasonpersinger) (the ROOM reverb click fix, #121),
  [renebohne](https://github.com/renebohne) (MIDI Program Change, #179)
