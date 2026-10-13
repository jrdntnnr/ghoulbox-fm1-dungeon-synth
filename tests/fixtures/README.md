# Frozen GHOULBOX 1.1 tuning-migration fixtures

`ghoulbox-1.1.fun8` and `ghoulbox-1.1.up4` were written by the original firmware at commit `c7891e0253ba8902c7df234f28d6cd3c47b4ccdc`, using `generate_ghoulbox_1_1.c`. They contain four track roots/scales, engine values, a two-note step, FM6 factory patches and an engine-parameter motion event. No tuning parameters exist in these source formats.

To reproduce from the repository root (after generating `build/gen`):

```sh
mkdir -p build/legacy-fixture
 git archive c7891e0 firmware tests/hostsim.c | tar -x -C build/legacy-fixture
cc -O2 -w -Ibuild/gen -Ibuild/legacy-fixture/tests -Ibuild/legacy-fixture/firmware/src \
  tests/fixtures/generate_ghoulbox_1_1.c -lm -o build/legacy-fixture/generate
build/legacy-fixture/generate build/legacy-fixture/project.fun8 build/legacy-fixture/preset.up4
cmp tests/fixtures/ghoulbox-1.1.fun8 build/legacy-fixture/project.fun8
cmp tests/fixtures/ghoulbox-1.1.up4 build/legacy-fixture/preset.up4
```

Keep the fixtures frozen; do not regenerate them with the new serializer to make a failing migration test pass.


`ghoulbox-prototype.gbm1` and `ghoulbox-prototype.up6` were written by the
microtuning prototype at `5a8438a`, before the Felucca integration. They carry
nonzero trims, root/scale choices and engine automation, and are read by
`tests/integration_format_test.c`. `generate_prototype.c` records their generator.
Compile it against that commit’s `tests/tuning_test.c` and generated headers,
with its tests directory on the include path; run from a disposable checkout
containing the original GHOULBOX 1.1 fixture. It writes into `tests/fixtures/`.
Never generate these fixtures using the current serializer.

## SARDAB / Ghoulbox 2.1 integration fixtures

`generate_ghoulbox_2.c` wrote `sardab-b9d47b7.gbm2` / `.up8` using our
`b9d47b7` serializer and `ghoulbox-2.1.funa` / `.up4` using upstream
`eaf04bd` (`ghoulbox-v2.1`). They cover four scale/root choices, nonzero cent
trims or temperaments, engine parameters, steps, FM6 patches, automation,
TEXTURE or upstream ambience, and optional keyboard layer/split.
`tests/ghoulbox2_test.c` checks the resulting migration, including silent
parameter-ID and scale-ID collisions.

With the normal generated headers in `build/gen`, reproduce into disposable
directories and compare without overwriting the fixtures:

```sh
mkdir -p build/fixture-b9 build/fixture-up
git archive b9d47b7 firmware tests/hostsim.c | tar -x -C build/fixture-b9
git archive eaf04bd firmware tests/hostsim.c | tar -x -C build/fixture-up
cc -O2 -w -Ibuild/gen -Ibuild/fixture-b9/tests -Ibuild/fixture-b9/firmware/src \
  tests/fixtures/generate_ghoulbox_2.c -lm -o build/fixture-b9/generate
cc -O2 -w -DUPSTREAM_GHOUL2 -Ibuild/gen -Ibuild/fixture-up/tests -Ibuild/fixture-up/firmware/src \
  tests/fixtures/generate_ghoulbox_2.c -lm -o build/fixture-up/generate
build/fixture-b9/generate build/fixture-b9/project build/fixture-b9/preset
build/fixture-up/generate build/fixture-up/project build/fixture-up/preset
cmp tests/fixtures/sardab-b9d47b7.gbm2 build/fixture-b9/project
cmp tests/fixtures/sardab-b9d47b7.up8 build/fixture-b9/preset
cmp tests/fixtures/ghoulbox-2.1.funa build/fixture-up/project
cmp tests/fixtures/ghoulbox-2.1.up4 build/fixture-up/preset
```
