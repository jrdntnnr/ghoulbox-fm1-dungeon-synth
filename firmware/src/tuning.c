/* SPDX-License-Identifier: GPL-3.0-only */
/* GHOULBOX tuning. Keys/steps retain their MIDI note identity; pitch is resolved
 * independently per voice. Offsets are relative to ROOT and repeat per octave.
 * Factory maqam tunings are editable 24-EDO starting points, not fixed claims
 * about intonation in performance. Original scale IDs and zero trims are exact
 * legacy behavior. Included after dsp.c, before the engines. */
#include "scales.h"

/* Upstream Ghoulbox temperament offsets, tenths of a cent from ROOT.
 * Composition: maqam preset + user cent trim + temperament. EQUAL adds zero. */
static const int16_t TEMPER[4][12] = {
    {0},
    {0, -98, 39, -59, 78, -20, 117, 20, -78, 59, -39, 98},
    {0, 171, -68, 103, -137, 34, -205, -34, 137, -103, 68, -171},
    {0, 117, 39, 156, -137, -20, -98, 20, 137, -156, -39, -117},
};
static int32_t tuning_decicents(const track_t *t, uint32_t note)
{
    uint32_t s, pc;
    /* Trigger maps are not pitched instruments. */
    if (t->eng_req == ENGI_DRUM || t->eng_req == ENGI_SLICE)
        return 0;
    s = (uint32_t)clamp(t->p[P_SCALE], 0, NELEM(SCALE_CENTS) - 1);
    pc = (note + 12u - (uint32_t)clamp(t->p[P_ROOT], 0, 11)) % 12u;
    return 10 * (SCALE_CENTS[s][pc] + clamp(t->p[P_TC0 + pc], -63, 63)) +
        TEMPER[clamp(t->p[P_TEMP], 0, 3)][pc];
}

/* Display / legacy whole-cent callers. Audio uses decicents below. */
static int32_t tuning_cents(const track_t *t, uint32_t note)
{
    int32_t c = tuning_decicents(t, note);
    return (c + (c < 0 ? -5 : 5)) / 10;
}
static int32_t tuning_pitch16(const track_t *t, uint32_t note)
{
    return clamp((int32_t)note * 16 + tuning_decicents(t, note) * 16 / 1000, 0, 2047);
}

static int32_t tuning_residual(const track_t *t, uint32_t note)
{
    int32_t c = tuning_decicents(t, note), q = c * 16 / 1000;
    int32_t target = (int32_t)note * 16000 + c * 16;
    if (target < 0 || target > 2047000) return 0;
    return (c * 16 - q * 1000) * 2367 / 160000;
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
    if (t->p[P_SCALE] >= 16 || t->p[P_TEMP]) return 1;
    for (i = P_TC0; i <= P_TC11; i++)
        if (t->p[i]) return 1;
    return 0;
}
