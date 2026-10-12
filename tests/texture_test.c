/* SPDX-License-Identifier: GPL-3.0-only */
/* TEXTURE (fx.c texture_*; after SLOOP 2.6 by isod89): a bed under the mix, FX > TEXTURE.
 *   off: SRC OFF, LEVEL 0, or WHEN PLAY while stopped: the mix bit for bit as without (a held note, every value
 *   of the other knobs).
 *   each generated source, ALWAYS, LEVEL 64, MOVE 0: in level (-45 .. -15 dBFS rms), bounded, no DC; USR1 with no
 *   sample: silence. Off again: faded out within 0.2 s, then the mix as without.
 *   TONE: left darker (less above 2 kHz), right thinner (less below 300 Hz).
 *   a project: TEXTURE kept in its inert slots (proj_tx_put, packed and read back, proj_tx_get); a project before it
 *   (those slots 0): the defaults, OFF. */
#include <math.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#define main hostsim_main
#include "hostsim.c"
#undef main
#define PROJ_HOST 1                                     /* (as tests/project_test.c) */
#include "../firmware/src/project.c"

#define SEC (FS / CTL)                                  /* blocks */

static int32_t out_buf[2u * CTL];
static int32_t *yl;                                     /* shared with the forked renders */
static int bad;

static void check(int ok, const char *what)
{
    printf("  %-72s %s\n", what, ok ? "ok" : "FAIL");
    bad += !ok;
}

/* nb blocks of the mix, the left channel into yl */
static void run(uint32_t nb)
{
    uint32_t b, i;
    for (b = 0; b < nb; b++) {
        mix_block(out_buf, CTL);
        for (i = 0; i < CTL; i++) {
            yl[b * CTL + i] = out_buf[2u * i];
        }
    }
}

static void tx_set(int32_t src, int32_t lvl, int32_t tone, int32_t move, int32_t when)
{
    song.g[G_TXSRC] = (int16_t)src;
    song.g[G_TXLVL] = (int16_t)lvl;
    song.g[G_TXTONE] = (int16_t)tone;
    song.g[G_TXMOVE] = (int16_t)move;
    song.g[G_TXWHEN] = (int16_t)when;
}

enum note_e { NOTE_OFF, NOTE_ON };
static void start(enum note_e note)
{
    uint32_t k;
    host_tracks_init();
    for (k = 0; k < NPART; k++) {
        host_preset(&trk[k], 0, 0);
    }
    song.playing = 0;
    if (note == NOTE_ON) {
        trk_note_on(&trk[0], 48, 100);
    }
}

/* nb blocks from power-on (a forked child: no state from the run before) with TEXTURE set so, into yl */
static void render(enum note_e note, int32_t src, int32_t lvl, int32_t tone, int32_t move, int32_t when, uint32_t nb)
{
    pid_t c = fork();
    if (!c) {
        start(note);
        tx_set(src, lvl, tone, move, when);
        run(nb);
        _exit(0);
    }
    waitpid(c, 0, 0);
}

static double rms_db(const int32_t *x, uint32_t n, double *mean)
{
    double s = 0, m = 0;
    uint32_t i;
    for (i = 0; i < n; i++) {
        s += (double)x[i] * x[i];
        m += x[i];
    }
    *mean = m / n;
    return 10.0 * log10(s / n / (32768.0 * 32768.0) + 1e-30);
}

/* power above (HP) or below (LP) a one-pole split at f, Hz: the energy of a first difference or a running mean */
enum band_e { BAND_LOW, BAND_HIGH };
static double band(const int32_t *x, uint32_t n, enum band_e b)
{
    double s = 0, z = 0;
    uint32_t i;
    for (i = 1; i < n; i++) {
        double v = b == BAND_HIGH ? (double)x[i] - x[i - 1] : (z += ((double)x[i] - z) * 0.03);
        s += v * v;
    }
    return s;
}

int main(void)
{
    static int32_t ref[2u * FS / CTL * CTL];
    static const char *const NAME[] = {"", "VINYL", "TAPE", "RAIN", "WIND", "RADIO", "ROOM"};
    uint32_t n = 2u * SEC * CTL, i, s;
    int same;
    char b[112];

    /* off: the reference with defaults (OFF), then OFF / LEVEL 0 / PLAY while stopped with the knobs elsewhere */
    yl = mmap(0, sizeof ref, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    render(NOTE_ON, 0, 64, 0, 40, 0, 2u * SEC);
    memcpy(ref, yl, sizeof ref);
    render(NOTE_ON, 0, 127, -30, 127, 1, 2u * SEC);
    same = !memcmp(ref, yl, sizeof ref);
    render(NOTE_ON, TX_VINYL, 0, 20, 90, 1, 2u * SEC);
    same &= !memcmp(ref, yl, sizeof ref);
    render(NOTE_ON, TX_RAIN, 127, 0, 40, 0, 2u * SEC);   /* (PLAY, stopped) */
    same &= !memcmp(ref, yl, sizeof ref);
    check(same, "off (SRC OFF, LEVEL 0, PLAY while stopped): the mix bit for bit as without");

    /* each source */
    for (s = TX_VINYL; s <= TX_ROOM; s++) {
        double db, mean;
        int32_t pk = 0;
        render(NOTE_OFF, (int32_t)s, 64, 0, 0, 1, 2u * SEC);
        db = rms_db(yl + SEC * CTL, SEC * CTL, &mean);
        for (i = 0; i < n; i++) {
            pk = abs(yl[i]) > pk ? abs(yl[i]) : pk;
        }
        snprintf(b, sizeof b, "%-5s LEVEL 64: %.1f dBFS rms (-45 .. -15), peak %d, mean %.0f", NAME[s], db, pk, mean);
        check(db > -45.0 && db < -15.0 && pk < 32767 && fabs(mean) < 300.0, b);
    }
    render(NOTE_OFF, TX_USR1, 127, 0, 0, 1, SEC);
    for (i = 0, same = 1; i < SEC * CTL; i++) {
        same &= yl[i] == 0;
    }
    check(same, "USR1 with no sample: silence");

    /* TONE */
    {
        double lo0, hi0, lo, hi;
        render(NOTE_OFF, TX_TAPE, 64, 0, 0, 1, 2u * SEC);
        lo0 = band(yl + SEC * CTL, SEC * CTL, BAND_LOW);
        hi0 = band(yl + SEC * CTL, SEC * CTL, BAND_HIGH);
        render(NOTE_OFF, TX_TAPE, 64, -48, 0, 1, 2u * SEC);
        hi = band(yl + SEC * CTL, SEC * CTL, BAND_HIGH);
        render(NOTE_OFF, TX_TAPE, 64, 48, 0, 1, 2u * SEC);
        lo = band(yl + SEC * CTL, SEC * CTL, BAND_LOW);
        snprintf(b, sizeof b, "TONE -48: highs %.1f dB, +48: lows %.1f dB (both < -6)", 10 * log10(hi / hi0),
                 10 * log10(lo / lo0));
        check(10 * log10(hi / hi0) < -6.0 && 10 * log10(lo / lo0) < -6.0, b);
    }

    /* off again: faded, then as without (in this process: after the forked renders) */
    start(NOTE_ON);
    tx_set(TX_TAPE, 100, 0, 40, 1);
    run(SEC);
    tx_set(0, 100, 0, 40, 1);
    run(SEC / 5u);
    check(tx.g == 0, "off again: faded out within 0.2 s, nothing runs");

    /* a project */
    {
        static project_t p, q;
        static project_store_t st;
        start(NOTE_OFF);
        tx_set(TX_RADIO, 99, -17, 77, 1);
        song.g[G_TXRATE] = 3;
        memset(&p, 0, sizeof p);                        /* (as project_capture) */
        proj_tx_put(&p);
        tx_set(0, 1, 2, 3, 0);
        song.g[G_TXRATE] = 0;
        same = proj_pack(&st, &p) && proj_unpack(&q, st.raw, PROJ_STORE_SIZE);
        proj_tx_get(&q);                                /* (as project_restore_runtime) */
        same &= song.g[G_TXSRC] == TX_RADIO && song.g[G_TXLVL] == 99 &&
               song.g[G_TXTONE] == -17 && song.g[G_TXMOVE] == 77 && song.g[G_TXWHEN] == 1 && song.g[G_TXRATE] == 3;
        check(same, "a project keeps TEXTURE (SRC LEVEL TONE MOVE RATE WHEN), packed and read back");
        q.g[G_INFO] = q.g[G_ENGGO] = q.g[G_SYNC] = 0;   /* a project before it */
        proj_tx_get(&q);
        same = 1;
        for (i = PROJ_NG; i < G_COUNT; i++) {
            same &= song.g[i] == GP[i].def;
        }
        check(same && song.g[G_TXSRC] == 0, "a project before it: the defaults, OFF");
    }

    printf(bad ? "TEXTURE: %d FAILED\n" : "TEXTURE: passed\n", bad);
    return bad != 0;
}
