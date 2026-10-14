/* SPDX-License-Identifier: GPL-3.0-only */
/* The second core (JIANT 0.6.71 port: hal/fm1_multicore.h, fx.c mix_block). On the host there is no second core:
 * mc_host 1 runs the jobs at once, as core 1 (MC_CPU() 1 while it does), through the same split, the same buffers of
 * its own and the same join. Checked here:
 *   - a phrase on four parts (SPREAD, the matrix, ENV SYNC, PHASE's crossfade, DIST, the sends) rendered on one core
 *     and on two is the same, bit for bit (each run in a fresh process); on two, both cores got parts
 *   - a job given up mid-part (mc_lost): the part's matrix values are put back, as mod_end would
 *   - MENU > ABOUT's line */
#define UI_TEST_NO_MAIN 1
#include "ui_test.c"
#include <sys/wait.h>
#include <unistd.h>

static int bad;
static void cck(const char *what, int ok)
{
    printf("cores: %-80s %s\n", what, ok ? "ok" : "FAIL");
    bad += !ok;
}

static uint32_t eng_of(const engine_t *e)
{
    uint32_t i;
    for (i = 0; i < NENGINES; i++)
        if (ENGINES[i] == e)
            return i;
    return 0;
}

typedef struct { uint64_t hash; uint32_t jobs, mask_or, both; } run_t;

/* the phrase: a chord on every part, off, another; 1 s. FNV of the output */
static run_t phrase(uint32_t cores)
{
    static int32_t o[2 * CTL];
    run_t r = {0x811C9DC5ull, 0, 0, 0};
    uint32_t b, i, k, nb = FS / CTL;
    host_tracks_init();
    ui_power_on();
    host_preset(&trk[0], ENGI_ANALOG, 0);
    trk[0].p[P_VOICE] = V_POLY; trk[0].p[P_SPRD] = 60; trk[0].p[P_DLY] = 40;
    host_preset(&trk[1], eng_of(&ENG_FM6), 0);
    trk[1].p[P_M1SRC] = MS_LFO; trk[1].p[P_M1DST] = MD_AMP; trk[1].p[P_M1AMT] = 40; trk[1].p[P_LRATE] = 80;
    trk[1].p[P_REV] = 50;
    host_preset(&trk[2], ENGI_PHYS, 0);
    trk[2].p[P_ESYNC] = 1; trk[2].p[P_DIST] = 30; trk[2].p[P_CHOR] = 30;
    host_preset(&trk[3], eng_of(&ENG_PHASE), 0);
    trk[3].p[P_VOICE] = V_POLY;
    mc_host = (uint8_t)cores;
    for (b = 0; b < nb; b++) {
        if (b == 0 || b == nb / 2u)
            for (k = 0; k < NTRK; k++) {
                trk_note_on(&trk[k], 48 + 5 * k + (b ? 2 : 0), 100);
                trk_note_on(&trk[k], 55 + 5 * k + (b ? 2 : 0), 90);
            }
        if (b == nb / 3u || b == nb - nb / 6u)
            for (k = 0; k < NTRK; k++) {
                trk_note_off(&trk[k], 48 + 5 * k + (b > nb / 2u ? 2 : 0));
                trk_note_off(&trk[k], 55 + 5 * k + (b > nb / 2u ? 2 : 0));
            }
        if (b == nb / 4u)
            trk[3].p[P_E2] = 100;                       /* PHASE: a fold change mid-note (phase_xfade) */
        mix_block(o, CTL);
        for (i = 0; i < 2u * CTL; i++) {
            r.hash ^= (uint32_t)o[i];
            r.hash *= 0x100000001B3ull;
        }
        r.mask_or |= mc_mask_last;
        r.both += mc_mask_last && mc_mask_last != 0xFu;
    }
    r.jobs = mc_jobs;
    return r;
}

static int phrase_in_child(uint32_t cores, run_t *out)   /* a fresh process a run: every seed and state at boot */
{
    int fd[2], st = 0;
    pid_t p;
    if (pipe(fd))
        return 0;
    p = fork();
    if (p == 0) {
        run_t r = phrase(cores);
        (void)!write(fd[1], &r, sizeof r);
        _exit(0);
    }
    close(fd[1]);
    if (read(fd[0], out, sizeof *out) != (ssize_t)sizeof *out)
        return 0;
    close(fd[0]);
    waitpid(p, &st, 0);
    return WIFEXITED(st) && !WEXITSTATUS(st);
}

int main(void)
{
    run_t one, two;
    int ok = phrase_in_child(0, &one) && phrase_in_child(1, &two);
    printf("    (one core %016llx jobs %u; two %016llx jobs %u, parts on core 1 %x, blocks shared %u)\n",
           (unsigned long long)one.hash, one.jobs, (unsigned long long)two.hash, two.jobs, two.mask_or, two.both);
    cck("four parts (SPREAD, matrix, ENV SYNC, PHASE, DIST, sends) on two cores: the same as on one, bit for bit",
        ok && one.hash == two.hash && one.jobs == 0 && two.jobs > 0);
    cck("  both cores rendered parts (not all on one), two or more on core 1 (the host: by the voices alone)",
        ok && two.both > 0 && __builtin_popcount(two.mask_or) >= 2);

    {   /* a job given up mid-part: the matrix values core 1 put in the part are put back */
        track_t *t = &trk[1];
        int16_t before[P_COUNT];
        host_tracks_init();
        ui_power_on();
        host_preset(t, eng_of(&ENG_FM6), 0);
        t->p[P_M1SRC] = MS_ENV; t->p[P_M1DST] = MD_PAN; t->p[P_M1AMT] = 60;   /* (PAN: a per-block one, in t->p) */
        t->m_env = 20000;
        memcpy(before, t->p, sizeof before);
        mc_host_cpu = 1;
        mod_begin(t);                                   /* (as core 1 does, first thing in mix_part_run) */
        mc_host_cpu = 0;
        ok = mod_c[1].on && memcmp(before, t->p, sizeof before) != 0;
        mc_cur = 1;
        mc_lost();
        ok = ok && !memcmp(before, t->p, sizeof before) && mc_cur == -1;
        cck("mc_lost: a part's matrix values (core 1's mod_begin) put back, bit for bit", ok);
        mc_host = 0;
    }
    cck("ABOUT's line: CORES 1 on the host, CORES 2 with the jobs handed over",
        !strcmp(mc_status(), "CORES 1") && (mc_host = 1, !strcmp(mc_status(), "CORES 2")));
    mc_host = 0;
    puts(bad ? "CORES TEST FAILED" : "cores test passed");
    return bad != 0;
}
