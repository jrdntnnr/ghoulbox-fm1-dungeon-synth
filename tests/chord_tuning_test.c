/* SPDX-License-Identifier: GPL-3.0-only */
/* Exercise chord generation through real note input and oscillator rendering.
 * Expected maqam degrees are independent of the generated scale tables. */
#define TUNING_TEST_NO_MAIN 1
#include "tuning_test.c"

static const struct { const char *name; uint8_t key[7]; int8_t cents[7]; } MAQAM[] = {
    {"RAST",     {0,2,4,5,7,9,11}, {0,0,-50,0,0,0,-50}}, /* Rast */
    {"RAST NH",  {0,2,4,5,7,9,10}, {0,0,-50,0,0,0,0}},   /* Rast Nahawand */
    {"BAYATI",   {0,2,3,5,7,8,10}, {0,-50,0,0,0,0,0}},   /* Bayati */
    {"BAY RAST", {0,2,3,5,7,9,10}, {0,-50,0,0,0,-50,0}}, /* Bayati Rast */
    {"SUZNAK",   {0,2,4,5,7,8,11}, {0,0,-50,0,0,0,0}},   /* Suznak */
    {"BAY SHUR", {0,2,3,5,6,9,10}, {0,-50,0,0,0,0,0}},   /* Bayati Shuri */
    {"HIJAZ",    {0,1,4,5,7,8,10}, {0}},                  /* Hijaz */
    {"HIJAZKAR", {0,1,4,5,7,8,11}, {0}},                  /* Hijazkar */
    {"NIKRIZ",   {0,2,3,6,7,9,10}, {0}},                  /* Nikriz */
    {"SHUR1",   {0,2,3,5,7,8,10}, {0,-50,0,0,0,0,0}},   /* Shur, experimental */
    {"SHUR2",   {0,2,3,5,7,8,10}, {0,-50,0,0,-50,0,0}}, /* Shur, koron fifth, experimental */
};

static void chord_reset(void)
{
    reset();
    memset(kb_chn,0,sizeof kb_chn); memset(mchord,0,sizeof mchord);
    memset(chord_last,0,sizeof chord_last); memset(midi_ch,0,sizeof midi_ch);
    memset(midi_notes,0,sizeof midi_notes); memset(midi_owners,0,sizeof midi_owners);
    fm1_in.notes=kb_prev=0; mi_r=mi_w=mo_r=mo_w=0; usb.config=1;
    trk[0].p[P_CHRD]=CH_DIA7;
}

/* All voices advance in the same render block: catches global pitch-bend
 * shortcuts that tune the whole chord instead of its individual notes. */
static void sounding(const uint8_t *notes, const int *cents, unsigned n)
{
    track_t *t=&trk[0]; int32_t out[CTL]; uint32_t phase[4]; unsigned gates=0;
    for(unsigned i=0;i<NVOICE;i++) gates+=t->v[i].active && t->v[i].gate;
    assert(gates==n);
    for(unsigned i=0;i<n;i++) { assert(voice(t,notes[i])->gate); phase[i]=voice(t,notes[i])->ph[0]; }
    track_render(t,out,CTL);
    for(unsigned i=0;i<n;i++) {
        double want=hz(notes[i],cents[i]); assert(want<FS/(double)CTL);
        double got=(uint32_t)(voice(t,notes[i])->ph[0]-phase[i])*(double)FS/CTL/4294967296.0;
        if(fabs(error(got,want))>=0.5)
            fprintf(stderr,"chord note %u %+d ct: %.4f Hz, expected %.4f Hz\n",notes[i],cents[i],got,want);
        assert(fabs(error(got,want))<0.5);
    }
}

static void released(void)
{
    for(unsigned i=0;i<NVOICE;i++) assert(!trk[0].v[i].gate);
}

static void maqam_sweep(void)
{
    unsigned cases=0;
    for(unsigned s=0;s<NELEM(MAQAM);s++) for(unsigned root=0;root<12;root++)
    for(unsigned degree=0;degree<7;degree++) for(unsigned mode=CH_DIA3;mode<=CH_DIA7;mode++)
    for(unsigned voicing=VC_CLOSE;voicing<=VC_BASS;voicing++) {
        chord_reset(); track_t *t=&trk[0]; uint8_t notes[4]; int cents[4];
        t->p[P_SCALE]=scale_id(MAQAM[s].name); t->p[P_ROOT]=root; t->p[P_CHRD]=mode; t->p[P_VOIC]=voicing;
        /* Include non-quarter-tone owner adjustments on two independent keys. */
        t->p[P_TC0+MAQAM[s].key[1]]=13; t->p[P_TC0+MAQAM[s].key[2]]=-7;
        kb_note[7]=36+root+MAQAM[s].key[degree]; key_on(7,t);
        unsigned n=kb_chn[7], seen=0;
        assert(n==(mode==CH_DIA3 && voicing!=VC_BASS ? 3u : 4u));
        for(unsigned i=0;i<n;i++) {
            notes[i]=kb_chord[7][i]; unsigned pc=(notes[i]+12-root)%12, d;
            for(d=0;d<7 && MAQAM[s].key[d]!=pc;d++);
            assert(d<7);
            unsigned rel=(d+7-degree)%7;
            assert(rel==0 || rel==2 || rel==4 || (mode==CH_DIA7 && rel==6));
            seen|=1u<<rel;
            cents[i]=MAQAM[s].cents[d]+(d==1?13:d==2?-7:0);
        }
        unsigned expected=mode==CH_DIA3?0x15u:voicing==VC_BASS?0x45u:0x55u;
        assert(seen==expected); sounding(notes,cents,n);
        key_off(7,t); released(); cases++;
    }
    printf("chord tuning: %u maqam / tonic / degree / triad-seventh / voicing cases, actual voice pitch within 0.5 cents\n",cases);
}

static void input_paths(void)
{
    const uint8_t rast[]={60,64,67,71}; const int rc[]={0,-37,0,-50};
    chord_reset(); track_t *t=&trk[0]; t->p[P_SCALE]=16; t->p[P_TC4]=13; t->p[P_QUANT]=QN_WHITE;
    song.playing=song.rec=1;
    fm1_in.notes=1u<<7; keyboard_block();
    sounding(rast,rc,4);
    assert(t->step[0].n==4 && !memcmp(t->step[0].note,rast,4));
    fm1_in.notes=0; keyboard_block(); released(); song.rec=song.playing=0;
    t->p[P_SLEN]=1; t->seq_active=1; seq_start(); events_block(CTL);
    sounding(rast,rc,4); seq_stop(); released();

    chord_reset(); t=&trk[0]; t->p[P_SCALE]=18; t->p[P_ROOT]=2;
    /* The tonic Bayati triad omits its altered second. Build on E to exercise it. */
    const uint8_t bayati[]={64,67,70,74}; const int bc[]={-50,0,0,0};
    midi_enqueue(9u|0x90u<<8|64u<<16|100u<<24,1); events_block(CTL);
    sounding(bayati,bc,4);
    t->p[P_CHRD]=CH_OFF; /* Note-off must retain the original generated notes. */
    midi_enqueue(8u|0x80u<<8|64u<<16,1); events_block(CTL); released();

    chord_reset(); t=&trk[0]; t->p[P_SCALE]=16; t->p[P_TC4]=13; t->p[P_AMODE]=AM_CHORD;
    fm1_in.notes=1u<<7; keyboard_block(); arp_tick(t,1); sounding(rast,rc,4);
    fm1_in.notes=0; keyboard_block(); arp_off(t); released();

    chord_reset(); t=&trk[0]; t->p[P_SCALE]=16; t->p[P_TC4]=13; t->p[P_AMODE]=AM_UP;
    fm1_in.notes=1u<<7; keyboard_block();
    for(unsigned i=0;i<4;i++) { arp_fire(t,div_samples(2)); sounding(rast+i,rc+i,1); }
    fm1_in.notes=0; keyboard_block(); arp_off(t); released();
    puts("chord tuning: WHITE keys, chord recording/playback, transposed Bayati MIDI, chord arp and UP arp pass");
}

static void custom_and_labels(void)
{
    chord_reset(); track_t *t=&trk[0]; uint8_t nn[4]; int32_t r; uint16_t mask; char name[12];
    t->p[P_SCALE]=SCALE_CUSTOM_ID; t->p[P_TMASK0]=t->p[P_TMASK1]=63; t->p[P_TC4]=-37;
    kb_note[7]=60; key_on(7,t);
    const uint8_t all[]={60,62,64,66}; const int ac[]={0,0,-37,0};
    sounding(all,ac,4); key_off(7,t); released();
    /* A Scala Rast mask and its imported cent trims. */
    t->p[P_TMASK0]=0x35; t->p[P_TMASK1]=0x2a; t->p[P_TC4]=-50; t->p[P_TC11]=-50;
    key_on(7,t); const uint8_t rast[]={60,64,67,71}; const int rc[]={0,-50,0,-50};
    sounding(rast,rc,4); key_off(7,t); released();
    unsigned n=chord_make(t,60,nn,&r,&mask); chord_tuned_name(name,t,r,mask,nn,n);
    assert(!strcmp(name,"C TUNED"));
    t->p[P_SCALE]=1; t->p[P_TC4]=t->p[P_TC11]=0;
    n=chord_make(t,60,nn,&r,&mask); chord_tuned_name(name,t,r,mask,nn,n);
    assert(!strcmp(name,"Cmaj7"));
    t->p[P_SCALE]=16; t->p[P_CHRD]=CH_MAJ;
    key_on(7,t); const uint8_t triad[]={60,64,67}; const int tc[]={0,-50,0};
    sounding(triad,tc,3); key_off(7,t); released();
    puts("chord tuning: full-key CUSTOM, imported Rast mask, fixed-shape tuning and truthful chord labels pass");
}

static void all_keys_audio(void)
{
    chord_reset(); track_t *t=&trk[0];
    t->p[P_SCALE]=scale_id("RAST"); t->p[P_QUANT]=QN_ALL; t->p[P_CHRD]=CH_OFF;
    /* C#4 and D4 physical keys are distinct D and E half-flat scale degrees. */
    fm1_in.notes=(1u<<8)|(1u<<9); keyboard_block();
    const uint8_t melody[]={62,64}; const int mc[]={0,-50}; sounding(melody,mc,2);
    t->p[P_QUANT]=QN_OFF; fm1_in.notes=0; keyboard_block(); released();
    t->p[P_QUANT]=QN_ALL; t->p[P_CHRD]=CH_DIA7; song.rec=song.playing=1;
    fm1_in.notes=1u<<9; keyboard_block();
    const uint8_t chord[]={64,67,71,74}; const int cc[]={-50,0,-50,0}; sounding(chord,cc,4);
    assert(t->step[0].n==4 && !memcmp(t->step[0].note,chord,4));
    fm1_in.notes=0; keyboard_block(); released(); song.rec=song.playing=0;
    t->p[P_SLEN]=1; t->seq_active=1; seq_start(); events_block(CTL);
    sounding(chord,cc,4); seq_stop(); released();
    t->p[P_AMODE]=AM_CHORD; fm1_in.notes=1u<<9; keyboard_block(); arp_tick(t,1);
    sounding(chord,cc,4); fm1_in.notes=0; keyboard_block(); arp_off(t); released();
    /* The reported bug: retain actual input/root even in a one-voice preset. */
    chord_reset(); t=&trk[0]; t->p[P_SCALE]=scale_id("HIJAZ"); t->p[P_ROOT]=7;
    t->p[P_VOICE]=V_LEGATO; t->p[P_CHRD]=CH_DIA3;
    uint8_t out[4]; assert(chord_build(t,69,out)==1 && out[0]==68);
    assert(chord_last[0].input==69 && chord_last[0].root==68 && chord_last[0].n==1);
    chord_build(t,68,out); assert(chord_last[0].input==chord_last[0].root);
    t->p[P_CHRD]=CH_OFF; chord_build(t,69,out); assert(out[0]==69 && chord_last[0].n==0);
    puts("ALL: black-key microtonal audio, chords, recording/playback, arp, releases; mono snap feedback pass");
}

int main(void) { maqam_sweep(); input_paths(); custom_and_labels(); all_keys_audio(); return 0; }
