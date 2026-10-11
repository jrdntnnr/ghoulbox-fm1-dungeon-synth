/* SPDX-License-Identifier: GPL-3.0-only */
/* GHOULBOX tuning. Keys/steps retain their MIDI note identity; pitch is resolved
 * independently per voice. Offsets are relative to ROOT and repeat per octave.
 * Factory maqam tunings are editable 24-EDO starting points, not fixed claims
 * about intonation in performance. Original scale IDs and zero trims are exact
 * legacy behavior. Included after dsp.c, before the engines. */
#include "scales.h"

static int32_t tuning_cents(const track_t *t, uint32_t note)
{
    uint32_t s, pc;
    /* Trigger maps are not pitched instruments. */
    if (t->eng_req == ENGI_DRUM || t->eng_req == ENGI_SLICE)
        return 0;
    s = (uint32_t)clamp(t->p[P_SCALE], 0, NELEM(SCALE_CENTS) - 1);
    pc = (note + 12u - (uint32_t)clamp(t->p[P_ROOT], 0, 11)) % 12u;
    return SCALE_CENTS[s][pc] + clamp(t->p[P_TC0 + pc], -63, 63);
}

static int32_t tuning_pitch16(const track_t *t, uint32_t note)
{
    return clamp((int32_t)note * 16 + tuning_cents(t, note) * 16 / 100, 0, 2047);
}

static int32_t tuning_residual(const track_t *t, uint32_t note)
{
    int32_t c = tuning_cents(t, note), q = c * 16 / 100;
    int32_t target = (int32_t)note * 1600 + c * 16;
    if (target < 0 || target > 204700) return 0;
    return (c * 16 - q * 100) * 2367 / 16000;
}

/* Only used for engines which compute their own rate from pitch16. The zero
 * path preserves their existing output exactly; no division in sample loops. */
static uint32_t tuning_rate(uint32_t rate, int32_t fine)
{
    int64_t corrected;
    if (!fine) return rate;
    corrected = (int64_t)rate + (int64_t)rate * fine / 4096;
    return corrected <= 0 ? 0 : corrected >= 0xFFFFFFFFLL ? 0xFFFFFFFFu : (uint32_t)corrected;
}

/* Resolve every member of a tuned chord at the same precision, including its
 * unaltered keys. Keep the original scales with zero trims bit-identical. */
static int tuning_precise(const track_t *t)
{
    uint32_t i;
    if (t->p[P_SCALE] >= 16) return 1;
    for (i = P_TC0; i <= P_TC11; i++)
        if (t->p[i]) return 1;
    return 0;
}
