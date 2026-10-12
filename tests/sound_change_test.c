/* SPDX-License-Identifier: GPL-3.0-only */
/* A PRESET LOADED OVER A HELD NOTE (voice.c sound_change; after SLOOP 2.6 by isod89):
 *   the load releases what sounds (ui.c apply_preset_to: panic_req). The notes sounding then fade out with the
 *   sound they had (its waveform, filter, drive and release), as for an engine switch, instead of going on in the
 *   new one from the middle of a cycle: a jump, a click.
 *   ANALOG SINE KEY holding a low note, then SQR BASS loaded: the largest step from one sample to the next in the
 *   blocks after the load stays near the sine's own (before: the square's edge, at once). Then silence, and the
 *   next note plays the new sound. */
#include <math.h>
#define main hostsim_main
#include "hostsim.c"
#undef main

#define ENG_ANALOG 0u                                   /* ENGINES[0] */

static int32_t out_buf[2u * CTL];
static int bad;

static void check(int ok, const char *what)
{
    printf("  %-72s %s\n", what, ok ? "ok" : "FAIL");
    bad += !ok;
}

static uint32_t preset_by_name(uint32_t e, const char *n)
{
    uint32_t k;
    for (k = 0; k < ENGINES[e]->npresets; k++) {
        if (str_eq(ENGINES[e]->presets[k].name, n)) {
            return k;
        }
    }
    return 0;
}

static void no_sends(track_t *t)
{
    t->p[P_DIST] = t->p[P_CHOR] = t->p[P_DLY] = t->p[P_REV] = 0;
}

/* n blocks of the mix; the largest sample-to-sample step of the left channel, and its last sample */
static int32_t prev;
static int32_t blocks(uint32_t n)
{
    uint32_t i;
    int32_t d = 0;
    while (n--) {
        mix_block(out_buf, CTL);
        for (i = 0; i < CTL; i++) {
            int32_t s = out_buf[2u * i], a = abs(s - prev);
            d = a > d ? a : d;
            prev = s;
        }
    }
    return d;
}

/* as ui.c apply_preset_to: the sound kept for the fade, the new values, what sounds released */
static void load_preset(track_t *t, uint32_t e, uint32_t pi)
{
    sound_change(t);
    host_preset_req(t, e, pi);
    no_sends(t);
    panic_req |= (uint8_t)(1u << trk_index(t));
}

int main(void)
{
    track_t *t = &trk[0];
    uint32_t k;
    int32_t steady, after, tail;
    char b[112];

    host_tracks_init();
    for (k = 0; k < NPART; k++) {
        host_preset(&trk[k], 0, 0);
        no_sends(&trk[k]);
    }
    lim_reset();
    host_preset(t, ENG_ANALOG, preset_by_name(ENG_ANALOG, "SINE KEY"));
    no_sends(t);
    t->p[P_SUS] = 127;                                  /* held: no decay under the load */
    trk_note_on(t, 36, 120);
    (void)blocks(FS / 4u / CTL);
    steady = blocks(FS / 8u / CTL);

    load_preset(t, ENG_ANALOG, preset_by_name(ENG_ANALOG, "SQR BASS"));
    after = blocks(16);
    snprintf(b, sizeof b, "load over a held note: largest step %d after, %d before (<= 2x)", after, steady);
    check(after <= 2 * steady, b);

    tail = blocks(FS / 2u / CTL);
    (void)tail;
    trk_note_off(t, 36);
    (void)blocks(FS / CTL);
    for (k = 0; k < NVOICE && !t->v[k].active; k++) {
    }
    check(k == NVOICE, "then silence: no voice left");

    trk_note_on(t, 36, 120);
    (void)blocks(FS / 8u / CTL);
    snprintf(b, sizeof b, "the next note plays the new sound (WAVE %d: SQR)", t->p[P_E0]);
    check(t->p[P_E0] == 1 && blocks(FS / 8u / CTL) > 2 * steady, b);

    printf(bad ? "SOUND CHANGE: %d FAILED\n" : "SOUND CHANGE: passed\n", bad);
    return bad != 0;
}
