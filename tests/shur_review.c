/* SPDX-License-Identifier: GPL-3.0-only */
/* SARDAB Shur review package: audio clips through the firmware's own audio code (hostsim.c) and Scala files, for a
 * musician to judge the experimental SHUR1 / SHUR2 tunings without a device (docs/SHUR_REVIEW.md).
 *   build/host/shur_review OUTDIR
 * Every clip is on ROOT D over a D3 drone. MIN is the 12-TET reference (the same keys, no offsets). */
#define main hostsim_main
#include "hostsim.c"
#undef main

enum { MELODY, DRONE };                            /* the tracks */
enum { NO_DRONE, WITH_DRONE };                     /* clip_scale: the PHYS oud sits ~20 ct sharp; no true-pitch drone */
#define DRONE_NOTE 50u                             /* D3 */
#define NOTE_S 0.6                                 /* one scale note */
#define TAIL_S 1.5                                 /* the release and reverb after the last note */

static FILE *wav;
static uint32_t frames;

static uint32_t preset_of(uint32_t e, const char *name)
{
    uint32_t i;
    for (i = 0; i < ENGINES[e]->npresets; i++)
        if (!strcmp(ENGINES[e]->presets[i].name, name))
            return i;
    fprintf(stderr, "no preset %s\n", name);
    exit(1);
}

static uint32_t scale_of(const char *name)
{
    uint32_t i;
    for (i = 0; i < NELEM(N_SCALE); i++)
        if (!strcmp(N_SCALE[i], name))
            return i;
    fprintf(stderr, "no scale %s\n", name);
    exit(1);
}

/* both tracks on scale s, ROOT D: the melody's sound, the drone a quiet sine */
static void tune(uint32_t s)
{
    uint32_t k;
    for (k = MELODY; k <= DRONE; k++) {
        trk[k].p[P_SCALE] = (int16_t)s;
        trk[k].p[P_ROOT] = 2;
    }
}

static void open_clip(const char *dir, const char *name, uint32_t e, const char *preset, uint32_t s)
{
    char path[512];
    uint32_t i;
    snprintf(path, sizeof path, "%s/%s.wav", dir, name);
    if (!(wav = fopen(path, "wb"))) {
        perror(path);
        exit(1);
    }
    wav_hdr(wav, 0);                               /* the length is patched in close_clip */
    frames = 0;

    /* boot state: no notes, default globals, empty reverb */
    memset(trk, 0, sizeof trk);
    host_tracks_init();
    for (i = 0; i < G_COUNT; i++)
        song.g[i] = GP[i].def;
    rev_clear();

    host_preset(&trk[MELODY], e, preset_of(e, preset));
    host_preset(&trk[DRONE], 0, preset_of(0, "SINE KEY"));
    trk[MELODY].p[P_VOICE] = V_POLY;
    trk[MELODY].p[P_AMODE] = 0;
    trk[DRONE].p[P_AMODE] = 0;
    trk[DRONE].p[P_SUS] = 127;
    trk[DRONE].p[P_LEVEL] = 60;

    /* steady pitch to judge: no chorus, delay, detune, glide or pitch modulation; the reverb stays */
    for (i = MELODY; i <= DRONE; i++) {
        trk[i].p[P_DIST] = trk[i].p[P_DIST + 1] = trk[i].p[P_DIST + 2] = 0;
        trk[i].p[P_DETUNE] = trk[i].p[P_GLIDE] = 0;
        trk[i].p[P_LD_PIT] = trk[i].p[P_ED_PIT] = 0;
    }
    tune(s);
    printf("%s\n", path);
}

static void render(double seconds)
{
    int32_t o[2 * CTL];
    uint32_t i, end = frames + (uint32_t)(seconds * FS);
    for (; frames < end; frames += CTL) {
        mix_block(o, CTL);
        for (i = 0; i < CTL; i++)
            wav_put(wav, o[2 * i], o[2 * i + 1]);
    }
}

static void close_clip(void)
{
    render(TAIL_S);
    fseek(wav, 0, SEEK_SET);
    wav_hdr(wav, frames);
    fclose(wav);
}

static void play(uint32_t note, double seconds)
{
    trk_note_on(&trk[MELODY], note, 100);
    render(seconds * 0.85);
    trk_note_off(&trk[MELODY], note);
    render(seconds * 0.15);
}

/* D4 up to D5 and back, over the drone or alone */
static void clip_scale(const char *dir, const char *name, uint32_t e, const char *preset, uint32_t s, uint32_t drone)
{
    static const uint8_t UP[8] = {62, 64, 65, 67, 69, 70, 72, 74};
    int32_t i;
    open_clip(dir, name, e, preset, s);
    if (drone == WITH_DRONE)
        trk_note_on(&trk[DRONE], DRONE_NOTE, 90);
    render(1.0);
    for (i = 0; i < 8; i++)
        play(UP[i], NOTE_S);
    for (i = 6; i >= 0; i--)
        play(UP[i], NOTE_S);
    trk_note_off(&trk[DRONE], DRONE_NOTE);
    close_clip();
}

/* held dyads over the drone: D4 + E4 (the koron second), then D4 + A4 (the fifth) */
static void clip_dyads(const char *dir, const char *name, uint32_t s)
{
    static const uint8_t TOP[2] = {64, 69};
    uint32_t i;
    open_clip(dir, name, 0, "SINE KEY", s);
    trk_note_on(&trk[DRONE], DRONE_NOTE, 90);
    for (i = 0; i < 2u; i++) {
        trk_note_on(&trk[MELODY], 62, 90);
        trk_note_on(&trk[MELODY], TOP[i], 90);
        render(3.0);
        trk_note_off(&trk[MELODY], 62);
        trk_note_off(&trk[MELODY], TOP[i]);
        render(0.5);
    }
    trk_note_off(&trk[DRONE], DRONE_NOTE);
    close_clip();
}

/* A4 held while the scale switches 1 -> 2 -> 1 every 2 s (the LIVE policy: the held note retunes) */
static void clip_switch(const char *dir, uint32_t a, uint32_t b)
{
    uint32_t i;
    open_clip(dir, "fifth_switch_1_2_1", 0, "SINE KEY", a);
    trk_note_on(&trk[DRONE], DRONE_NOTE, 90);
    trk_note_on(&trk[MELODY], 69, 100);
    for (i = 0; i < 3u; i++) {
        tune(i == 1u ? b : a);
        render(2.0);
    }
    trk_note_off(&trk[MELODY], 69);
    trk_note_off(&trk[DRONE], DRONE_NOTE);
    close_clip();
}

/* scale s as a Scala file: its degrees above ROOT in cents, the octave as 2/1 */
static void scala(const char *dir, const char *file, uint32_t s)
{
    char path[512];
    uint32_t k, n = 0;
    FILE *f;
    snprintf(path, sizeof path, "%s/%s.scl", dir, file);
    if (!(f = fopen(path, "w"))) {
        perror(path);
        exit(1);
    }
    for (k = 1; k < 12u; k++)
        n += (SCALE_MASK[s] >> k) & 1u;
    fprintf(f, "! %s.scl\n!\n%s: SARDAB experimental 24-EDO starting point, not a reviewed transcription\n %u\n!\n",
            file, N_SCALE[s], n + 1u);
    for (k = 1; k < 12u; k++)
        if ((SCALE_MASK[s] >> k) & 1u)
            fprintf(f, " %d.0\n", (int)k * 100 + SCALE_CENTS[s][k]);
    fprintf(f, " 2/1\n");
    fclose(f);
    printf("%s\n", path);
}

int main(int argc, char **argv)
{
    const char *dir = argc > 1 ? argv[1] : ".";
    const uint32_t a = scale_of("SHUR1"), b = scale_of("SHUR2"), ref = scale_of("MIN");

    /* the scales, two timbres: a sine over the drone for pitch, the plucked oud model alone for context */
    clip_scale(dir, "scale_SHUR1_sine", 0, "SINE KEY", a, WITH_DRONE);
    clip_scale(dir, "scale_SHUR2_sine", 0, "SINE KEY", b, WITH_DRONE);
    clip_scale(dir, "scale_12TET_ref_sine", 0, "SINE KEY", ref, WITH_DRONE);
    clip_scale(dir, "scale_SHUR1_oud", ENGI_PHYS, "DESERT OUD", a, NO_DRONE);
    clip_scale(dir, "scale_SHUR2_oud", ENGI_PHYS, "DESERT OUD", b, NO_DRONE);
    clip_scale(dir, "scale_12TET_ref_oud", ENGI_PHYS, "DESERT OUD", ref, NO_DRONE);

    clip_dyads(dir, "dyads_SHUR1", a);
    clip_dyads(dir, "dyads_SHUR2", b);
    clip_dyads(dir, "dyads_12TET_ref", ref);
    clip_switch(dir, a, b);

    scala(dir, "shur1", a);
    scala(dir, "shur2", b);
    return 0;
}
