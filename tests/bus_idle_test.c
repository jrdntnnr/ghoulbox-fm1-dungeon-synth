/* SPDX-License-Identifier: GPL-3.0-only */
/* IDLE FX BUSES (fx.c fx_buses, bus_idle): a bus whose input has been 0, and its state at most BUS_QUIET, long enough
 * is skipped.
 *   chorus only: the output bit for bit as with every bus always running (bus_skip 0), its tail and after.
 *   a phrase with every send, its tails: within a few LSB of always running; a note after: within 16 (-66 dBFS: the
 *   buses' leftover of +-BUS_QUIET, held while skipped, still going round when not).
 *   the tails over: all three buses skipped (ROOM too: its tail held at +-41 for ever before fx.c mulq15z), the idle
 *   mix cheaper (host instructions).
 *   a send coming back: the bus runs at once (its first block). */
#include <math.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <libproc.h>
#include <unistd.h>
#define main hostsim_main
#include "hostsim.c"
#undef main

#define SEC (FS / CTL)                                  /* blocks */
#define NB (8u * SEC)

static int32_t out_buf[2u * CTL];
static int32_t *yl;                                     /* shared with the forked renders */
static int bad;

static void check(int ok, const char *what)
{
    printf("  %-72s %s\n", what, ok ? "ok" : "FAIL");
    bad += !ok;
}

enum sends_e { SEND_CHO, SEND_ALL };

/* 8 s from power-on: a chord for 1 s with these sends, its tails, a note again at 6 s; into yl (left) */
static void render(enum sends_e sends, uint8_t skip)
{
    pid_t c = fork();
    uint32_t k, b, i;
    track_t *t = &trk[0];
    if (c) {
        waitpid(c, 0, 0);
        return;
    }
    bus_skip = skip;
    host_tracks_init();
    for (k = 0; k < NPART; k++) {
        host_preset(&trk[k], 0, 0);
        trk[k].p[P_DIST] = trk[k].p[P_CHOR] = trk[k].p[P_DLY] = trk[k].p[P_REV] = 0;
    }
    t->p[P_CHOR] = 100;
    if (sends == SEND_ALL) {
        t->p[P_DLY] = 90;
        t->p[P_REV] = 100;
    }
    for (b = 0; b < NB; b++) {
        if (b == 0) {
            trk_note_on(t, 48, 110);
            trk_note_on(t, 55, 110);
        }
        if (b == SEC) {
            trk_note_off(t, 48);
            trk_note_off(t, 55);
        }
        if (b == 6u * SEC) {
            trk_note_on(t, 52, 110);
        }
        mix_block(out_buf, CTL);
        for (i = 0; i < CTL; i++) {
            yl[b * CTL + i] = out_buf[2u * i];
        }
    }
    _exit(0);
}

static uint64_t instr_now(void)
{
    struct rusage_info_v4 ri;
    return proc_pid_rusage(getpid(), RUSAGE_INFO_V4, (rusage_info_t *)&ri) ? 0 : ri.ri_instructions;
}

int main(void)
{
    static int32_t ref[NB * CTL];
    uint32_t i, b;
    int32_t d = 0;
    char s[112];

    yl = mmap(0, sizeof ref, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);

    /* chorus only: exact */
    render(SEND_CHO, 0);
    memcpy(ref, yl, sizeof ref);
    render(SEND_CHO, 1);
    check(!memcmp(ref, yl, sizeof ref), "chorus only: bit for bit as with every bus running (tail, then a note)");

    /* every send: within a few LSB */
    render(SEND_ALL, 0);
    memcpy(ref, yl, sizeof ref);
    render(SEND_ALL, 1);
    {
        int32_t dn = 0;
        for (i = 0; i < NB * CTL; i++) {                /* tails: to 6 s; then the note */
            int32_t e = abs(yl[i] - ref[i]);
            if (i < 6u * SEC * CTL) {
                d = e > d ? e : d;
            } else {
                dn = e > dn ? e : dn;
            }
        }
        snprintf(s, sizeof s, "every send: the tails within %d LSB (<= %d)", d, 3 * BUS_QUIET);
        check(d <= 3 * BUS_QUIET, s);
        snprintf(s, sizeof s, "the note after: within %d LSB (<= 16: the old residue, frozen or circulating)", dn);
        check(dn <= 16, s);
    }

    /* the tails over: skipped, cheaper; a send back: running at once */
    {
        uint64_t i0, i1, i2;
        host_tracks_init();
        for (i = 0; i < NPART; i++) {
            host_preset(&trk[i], 0, 0);
        }
        trk[0].p[P_CHOR] = trk[0].p[P_DLY] = trk[0].p[P_REV] = 100;
        trk_note_on(&trk[0], 48, 110);
        for (b = 0; b < SEC; b++) {
            mix_block(out_buf, CTL);
        }
        trk_note_off(&trk[0], 48);
        for (b = 0; b < 6u * SEC; b++) {
            mix_block(out_buf, CTL);
        }
        check(bus_q[BUS_CHO] >= BUS_HOLD[BUS_CHO] && bus_q[BUS_DLY] >= BUS_HOLD[BUS_DLY] &&
              bus_q[BUS_REV] >= BUS_HOLD[BUS_REV], "the tails over (6 s): chorus, delay and reverb skipped");
        i0 = instr_now();
        for (b = 0; b < SEC; b++) {
            mix_block(out_buf, CTL);
        }
        i1 = instr_now();
        bus_skip = 0;
        for (b = 0; b < SEC; b++) {
            mix_block(out_buf, CTL);
        }
        i2 = instr_now();
        bus_skip = 1;
        snprintf(s, sizeof s, "idle: %.0f instructions / sample, %.0f with every bus running",
                 (double)(i1 - i0) / (SEC * CTL), (double)(i2 - i1) / (SEC * CTL));
        check(i0 && i1 - i0 < i2 - i1, s);
        trk_note_on(&trk[0], 48, 110);
        for (b = 0; b < 2u; b++) {
            mix_block(out_buf, CTL);
        }
        check(!bus_q[BUS_CHO] && !bus_q[BUS_DLY] && !bus_q[BUS_REV], "a note with sends: every bus running again at once");
    }

    printf(bad ? "BUS IDLE: %d FAILED\n" : "BUS IDLE: passed\n", bad);
    return bad != 0;
}
