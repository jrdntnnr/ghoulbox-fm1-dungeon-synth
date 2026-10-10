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
