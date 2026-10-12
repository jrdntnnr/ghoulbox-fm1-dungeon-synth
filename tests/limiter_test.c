/* SPDX-License-Identifier: GPL-3.0-only */
/* MASTER LIMITER (fx.c master_block; after SLOOP 2.6 by isod89, its look-ahead limiter):
 *   onset: a bass 4x over the threshold out of silence, starting at its peak (a kick's attack). The limiter sees it coming, so no sample reaches the
 *   soft clip's knee much above the threshold (the old one caught up in ~4 samples: the first went in at up to 4x,
 *   a crackle on every kick with MASTER up).
 *   floor: a held bass 2x over the threshold. The gain stays put and keeps every bit, so the bins between the
 *   bass's harmonics (1..4 kHz) stay near the 16-bit floor (the old >> 4 kept 12 bits of the limited signal). */
#include <math.h>
#define main hostsim_main
#include "hostsim.c"
#undef main

#define F0 55.0                 /* the bass, Hz: FS / F0 not whole, the floor bins between its harmonics */
#define SEC (FS / CTL * CTL)  /* whole blocks */

static int32_t yl[SEC], yr[SEC];
static int fails;

static void reset(void)
{
    fx_lowcut = 0;
    dc_l = dc_r = dce_l = dce_r = 0;
    lim_reset();
}

/* x (n samples, both channels alike) through the master, out in yl */
static void run(const int32_t *x, uint32_t n)
{
    uint32_t i, k;
    static int32_t l[CTL], r[CTL];
    for (i = 0; i < n; i += CTL) {
        for (k = 0; k < CTL; k++)
            l[k] = r[k] = x[i + k];
        master_block(l, r, CTL);
        for (k = 0; k < CTL; k++)
            yl[i + k] = l[k], yr[i + k] = r[k];
    }
}

static double bin_pow(const int32_t *x, uint32_t n, double f)
{
    double w = 2.0 * cos(2.0 * M_PI * f / FS), s1 = 0, s2 = 0;
    uint32_t i;
    for (i = 0; i < n; i++) {                          /* Blackman-Harris: sidelobes under -92 dB, so the
                                                         * harmonics do not leak into the bins between them */
        double t = 2.0 * M_PI * i / (n - 1u);
        double bh = 0.35875 - 0.48829 * cos(t) + 0.14128 * cos(2 * t) - 0.01168 * cos(3 * t);
        double s0 = x[i] * bh + w * s1 - s2;
        s2 = s1;
        s1 = s0;
    }
    return (s1 * s1 + s2 * s2 - w * s1 * s2) / ((double)n * n);
}

static void check(int ok, const char *what)
{
    printf("  %-60s %s\n", what, ok ? "ok" : "FAIL");
    fails += !ok;
}

int main(void)
{
    static int32_t x[SEC];
    uint32_t i, on = SEC / 10u, half = SEC / 2u;
    int32_t pk = 0, lim_out = softclip(LIM_T + LIM_T / 16);
    double fund, floor = 0;
    char b[96];

    /* onset */
    reset();
    for (i = 0; i < SEC; i++)
        x[i] = i < on ? 0 : (int32_t)(4.0 * LIM_T * cos(2.0 * M_PI * F0 * (i - on) / FS));
    run(x, SEC);
    for (i = 0; i < SEC; i++)
        pk = abs(yl[i]) > pk ? abs(yl[i]) : pk;
    snprintf(b, sizeof b, "onset 4x over, at its peak: peak %d <= %d (the threshold +0.5 dB, knee)", pk, lim_out);
    check(pk <= lim_out, b);

    /* floor: the last half second of a held bass */
    reset();
    for (i = 0; i < SEC; i++)
        x[i] = (int32_t)(2.0 * LIM_T * sin(2.0 * M_PI * F0 * i / FS));
    run(x, SEC);
    fund = bin_pow(yl + half, half, F0);
    for (i = 18; i < 72; i++)                           /* 1.0 .. 4 kHz, halfway between harmonics */
        floor += bin_pow(yl + half, half, F0 * (i + 0.5));
    floor /= 54.0;
    snprintf(b, sizeof b, "held bass 2x over: floor %.1f dB under the bass (<= -120)", 10.0 * log10(floor / fund));
    check(10.0 * log10(floor / fund) <= -120.0, b);

    printf(fails ? "LIMITER: %d FAILED\n" : "LIMITER: passed\n", fails);
    return fails != 0;
}
