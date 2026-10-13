/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments */
/* 1.6, on the real UI sources (with the stubs of tests/ui_test.c):
 * Discussion #238 (#242): SEQ's piano roll follows the play position:
 *   follows    a melodic pattern longer than 16 steps, the transport playing: the roll's bank is the playhead's;
 *              STEP and DETAIL alike; the cursor, the steps and the pattern length are not touched
 *   leaves     a 16-step pattern, the transport stopped, DRUM's grid, SEQ > AUTOMATION, HOME: the bank stays
 *   holds      KNOB 1 (STEP) or PRESETS (DETAIL) walking the cursor, an edit of the cursor's step (KNOB 2..4, DETAIL's
 *              four), step entry: the roll stays on the cursor's bank
 *   resumes    after 3 s without that, or once the playhead has gone one pass round the pattern, whichever is first
 *   resets     the transport stopped, another track
 * Built and run by tests/run_tests.sh. */
#define UI_TEST_NO_MAIN 1
#include "ui_test.c"

/* track 1 alone, `len` steps of 1/16, a note on every fourth step; the transport "playing" with the playhead at idx
 * (the sequencer itself is not run: only the UI's view of it), SEQ > STEP, the cursor on step `cur` */
static track_t *pattern(uint32_t len, uint32_t cur, uint32_t idx)
{
    track_t *t;
    uint32_t i;
    ui_power_on();
    song.sel = 0;
    t = &trk[0];
    track_defaults_steps(t);
    t->p[P_SLEN] = (int16_t)len;
    t->p[P_SDIV] = 2;
    t->p[P_SGATE] = 64;
    t->p[P_VOICE] = V_POLY;
    for (i = 0; i < len; i += 4)
        t->step[i] = (step_t){{(uint8_t)(48 + i)}, 1, ST_NOTE, 0, 0};
    go_page(GR_ROLL);
    ui.cursor = (uint8_t)cur;
    ui.bank = (uint8_t)(cur / 16u);
    song.playing = 1;
    t->seq_idx = (uint16_t)idx;
    frame();
    return t;
}
static void go_detail(void)
{
    uint32_t i;
    for (i = 0; i < NPAGES && !str_eq(PAGES[i].title, "DETAIL"); i++)
        ;
    ui.home = 0;
    ui.page = (uint8_t)i;
    page_entered();
    frame();
}
static void kt(uint32_t role, int32_t s)             /* a knob turned by s detents, unaccelerated */
{
    memset(ui.enc_t, 0, sizeof ui.enc_t);
    turn(role, s);
}
/* the playhead at idx, one UI frame later */
static void at(track_t *t, uint32_t idx)
{
    t->seq_idx = (uint16_t)idx;
    frame();
}

static int follows(void)
{
    int bad = 0, ok;
    track_t *t = pattern(40, 0, 0);
    step_t ref[NSTEP];
    memcpy(ref, t->step, sizeof ref);
    ok = ui.bank == 0u;
    at(t, 20);
    ok &= ui.bank == 1u;
    at(t, 36);
    ok &= ui.bank == 2u;
    at(t, 5);
    ok &= ui.bank == 0u;
    bad += check("SEQ > STEP, 40 steps, playing: the roll's bank is the playhead's (0, 1, 2, 0)", ok);
    ok = ui.cursor == 0u && t->p[P_SLEN] == 40 && !memcmp(ref, t->step, sizeof ref);
    at(t, 36);
    ok &= ui.cursor == 0u && !memcmp(ref, t->step, sizeof ref);
    bad += check("  the cursor, the steps and the length are not touched", ok);
    go_detail();
    at(t, 20);
    ok = ui.bank == 1u && detail_on();
    at(t, 3);
    ok &= ui.bank == 0u;
    bad += check("  SEQ > DETAIL follows the same way", ok);
    at(t, 39);
    ok = ui.bank == 2u;
    bad += check("  a last bank of 8 steps (steps 33..40) is followed too", ok);
    return bad;
}

static int leaves(void)
{
    int bad = 0, ok;
    track_t *t = pattern(16, 0, 0);
    at(t, 9);
    ok = ui.bank == 0u;
    bad += check("a 16-step pattern: nothing to follow (bank 0)", ok);
    t = pattern(40, 0, 0);
    song.playing = 0;
    at(t, 36);
    ok = ui.bank == 0u;
    bad += check("the transport stopped: the playhead does not move the roll", ok);
    t = pattern(40, 0, 0);
    go_page(GR_EVENTS);
    at(t, 36);
    ok = ui.bank == 0u;
    bad += check("SEQ > AUTOMATION (not the roll): the bank stays", ok);
    t = pattern(40, 0, 0);
    ui.home = 1;
    at(t, 36);
    ok = ui.bank == 0u;
    bad += check("HOME: the bank stays", ok);
    ui_power_on();
    song.sel = 3;
    t = &trk[3];
    track_defaults_steps(t);
    t->p[P_SLEN] = 40;
    go_page(GR_ROLL);
    ui.cursor = 0;
    ui.bank = 0;
    song.playing = 1;
    ok = drum_track(t);
    at(t, 36);
    ok &= ui.bank == 0u;
    bad += check("a DRUM track (its grid): the bank stays, the keys keep their steps", ok);
    return bad;
}

static int holds(void)
{
    int bad = 0, ok;
    track_t *t = pattern(40, 0, 0);
    at(t, 20);
    ok = ui.bank == 1u;
    kt(EN_K1, 1);                                     /* the cursor to step 2: the bank is its own (0) */
    ok &= ui.cursor == 1u && ui.bank == 0u;
    at(t, 36);
    ok &= ui.bank == 0u;
    at(t, 37);
    ok &= ui.bank == 0u;
    bad += check("STEP: KNOB 1 walks the cursor: the roll stays on the cursor's bank while the playhead moves on", ok);
    fm1_ms += 3100u;
    frame();
    ok = ui.bank == 2u;
    bad += check("  3 s without a touch: following again (bank 2)", ok);
    t = pattern(40, 0, 0);
    ui.cursor = 33;
    ui.bank = 2;
    kt(EN_K1, 1);                                     /* step 34 (bank 2), playhead on 0: held */
    ok = ui.cursor == 34u && ui.bank == 2u;
    {
        uint32_t i;
        for (i = 1; i < 40u; i++) {                   /* a pass: 39 steps gone, still held (bank 2 though the playhead is in 0..2) */
            at(t, i);
            if (i < 39u)
                ok &= ui.bank == 2u;
        }
        ok &= ui.bank == 2u;
        at(t, 0);                                     /* the 40th step: one pass round */
        ok &= ui.bank == 0u;
    }
    bad += check("  one pass round the pattern (well under 3 s): following again", ok);
    t = pattern(40, 0, 0);
    at(t, 20);
    ok = ui.bank == 1u && ui.cursor == 0u;
    kt(EN_K2, 1);                                     /* NOTE of the cursor's step */
    ok &= ui.bank == 0u && ui.cursor == 0u;
    at(t, 21);
    ok &= ui.bank == 0u;
    bad += check("KNOB 2 (the cursor's step edited): the roll goes back to the cursor and stays", ok);
    t = pattern(40, 0, 0);
    at(t, 20);
    ui.entry_open = 1;
    frame();
    ok = ui.bank == 0u;
    at(t, 36);
    ok &= ui.bank == 0u;
    bad += check("step entry has the keys: the roll stays on the cursor", ok);
    ui.entry_open = 0;
    t = pattern(40, 0, 0);
    go_detail();
    at(t, 20);
    ok = ui.bank == 1u;
    kt(EN_K2, 1);                                     /* DETAIL: RATCH of the cursor's step */
    ok &= ui.bank == 0u;
    at(t, 36);
    ok &= ui.bank == 0u;
    bad += check("DETAIL: KNOB 2 (a field of the cursor's step) holds the roll too", ok);
    return bad;
}

static int resets(void)
{
    int bad = 0, ok;
    track_t *t = pattern(40, 0, 0);
    kt(EN_K1, 1);
    song.playing = 0;                                 /* held, then stopped ... */
    at(t, 3);
    song.playing = 1;                                 /* ... and played again: following at once */
    at(t, 36);
    ok = ui.bank == 2u;
    bad += check("stop, play: the hold is gone (the roll follows at once)", ok);
    t = pattern(40, 0, 0);
    kt(EN_K1, 1);
    song.playing = 1;
    track_select(1);
    trk[1] = *t;
    trk[1].seq_idx = 36;
    frame();
    ok = ui.bank == 2u;
    bad += check("another track: the hold is gone (the roll follows that track's playhead)", ok);
    return bad;
}

int main(void)
{
    int bad = follows() + leaves() + holds() + resets();
    printf("%s\n", bad ? "1.6 TEST FAILED" : "1.6 tests passed");
    return bad != 0;
}
