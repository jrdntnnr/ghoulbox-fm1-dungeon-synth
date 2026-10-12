#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
# Native checks for the microtuning milestone. No JieLi SDK or firmware package.
set -eu
cd "$(dirname "$0")/.."
CC=${CC:-cc}
OUT=build/host
mkdir -p "$OUT" build/assessment
python3 tools/gen_scales.py --check
if [ ! -f build/gen/ui_fonts.h ] || [ ! -f build/gen/felucca_samples.h ]; then
    python3 -c 'import sys; sys.path.insert(0,"tools"); import build; build.generate()'
fi
for name in tuning chord_tuning desert integration_format scale project chord motion ratchet midi_control ui persistence backup editor; do
    "$CC" -O2 -w -Ibuild/gen -Ifirmware/src -o "$OUT/${name}_test" "tests/${name}_test.c" -lm
    if "$OUT/${name}_test" > "build/assessment/tuning-${name}.log" 2>&1; then
        echo "$name: passed"
    else
        cat "build/assessment/tuning-${name}.log"
        exit 1
    fi
done
# New melodic models: actual chord voices, persistence, release and independent drone.
# FFT pitch measurements are separate (check_sourced_audio.py requires NumPy).
mkdir -p build/assessment/sourced-synths
"$CC" -O2 -w -Ibuild/gen -Ifirmware/src -o "$OUT/sourced_synth_test" tests/sourced_synth_test.c -lm
"$OUT/sourced_synth_test" build/assessment/sourced-synths > build/assessment/tuning-sourced-synth.log
echo "sourced synths: passed"
# the Shur review renderer (docs/SHUR_REVIEW.md): built so it cannot rot, not run
"$CC" -O2 -w -Ibuild/gen -Ifirmware/src -o "$OUT/shur_review" tests/shur_review.c -lm
"$CC" -O2 -w -Ibuild/gen -Ifirmware/src -o "$OUT/regress" tests/regress.c -lm
"$OUT/regress" tests/golden.txt tests/cpu_baseline.txt > build/assessment/tuning-regress.log
"$CC" -O2 -w -Ibuild/gen -Ifirmware/src -Itests -o "$OUT/ui_render" tests/ui_render.c -lm
mkdir -p build/ui_tuning/ppm build/ui_tuning_slot
"$OUT/ui_render" build/ui_tuning build/ui_tuning_slot > build/assessment/tuning-ui-render.log
"$CC" -O2 -w -Ibuild/gen -Ifirmware/src -o "$OUT/descdump" tests/descdump.c -lm
"$OUT/descdump" > "$OUT/desc.json"
node web/test_web.mjs > build/assessment/tuning-web.log
node web/test_backup.mjs > build/assessment/tuning-web-backup.log
node web/test_scala.mjs > build/assessment/tuning-scala.log
echo "Microtuning host checks passed. Hardware build and on-device checks are separate."
