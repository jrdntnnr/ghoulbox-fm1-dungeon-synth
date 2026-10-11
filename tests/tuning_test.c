/* SPDX-License-Identifier: GPL-3.0-only */
/* Real synth output, polyphonic note identity, and frozen pre-tuning records. */
#include <assert.h>
#define main hostsim_main
#include "hostsim.c"
#undef main
#define PROJ_HOST 1
#include "../firmware/src/project.c"
#define UP_HOST 1
#include "../firmware/src/upreset.c"

static void reset(void)
{
    memset(trk, 0, sizeof trk); memset(&song, 0, sizeof song);
    memset(midi_bend_q8, 0, sizeof midi_bend_q8);
    memset(midi_bend_target, 0, sizeof midi_bend_target);
    host_tracks_init();
    for (unsigned k=0;k<NTRK;k++) {
        track_t *t=&trk[k]; host_preset(t, 0, 0);
        t->p[P_E0]=3; t->p[P_E1]=t->p[P_E2]=t->p[P_E3]=0;
        t->p[P_E4]=127; t->p[P_E5]=t->p[P_E6]=0;
        t->p[P_ATK]=t->p[P_DEC]=t->p[P_REL]=0; t->p[P_SUS]=127;
        t->p[P_VOICE]=V_POLY; t->p[P_LD_PIT]=t->p[P_ED_PIT]=0;
    }
}
static voice_t *voice(track_t *t, unsigned note)
{
    for(unsigned i=0;i<NVOICE;i++) if(t->v[i].active && t->v[i].note==note) return &t->v[i];
    assert(!"missing voice"); return 0;
}
static double hz(unsigned note, int cents) { return 440*pow(2, ((int)note-69+cents/100.0)/12); }
static double error(double got, double want) { return 1200*log2(got/want); }

static void pitch_math(void)
{
    reset(); track_t *t=&trk[0]; double worst=0;
    assert(NELEM(SCALE_CENTS)==41 && NELEM(SCALE_MASK)==41);
    for(int s=0;s<40;s++) for(int root=0;root<12;root++) {
        t->p[P_SCALE]=s; t->p[P_ROOT]=root;
        for(int n=12;n<116;n++) for(int trim=-63;trim<=63;trim++) {
            int pc=(n+12-root)%12, c=SCALE_CENTS[s][pc]+trim;
            t->p[P_TC0+pc]=trim;
            uint32_t inc=pitch_inc(tuning_pitch16(t,n));
            inc+=(uint32_t)((int32_t)(inc>>12)*tuning_residual(t,n));
            double e=fabs(error(inc*(double)FS/4294967296.0,hz(n,c)));
            if(e>worst) worst=e;
            assert(e<0.5);
            t->p[P_TC0+pc]=0;
        }
    }
    t->p[P_ROOT]=0; t->p[P_SCALE]=0; t->p[P_TC0]=-63;
    assert(tuning_pitch16(t,0)==0 && tuning_residual(t,0)==0);
    t->p[P_SCALE]=16; t->p[P_TC0]=0;
    assert(tuning_cents(t,64)==-50 && tuning_cents(t,71)==-50 && !tuning_cents(t,67));
    t->p[P_TC0+4]=13; assert(tuning_cents(t,64)==-37 && tuning_cents(t,76)==-37);
    t->p[P_ROOT]=2; assert(tuning_cents(t,66)==-37 && !tuning_cents(t,64));
    t->eng_req=ENGI_DRUM; assert(!tuning_cents(t,66));
    t->eng_req=ENGI_SLICE; assert(!tuning_cents(t,66));
    printf("tuning: 40 scales, 12 tonics, 104 notes and all trims: worst %.3f cents\n",worst);
}
static double audio_frequency(unsigned scale, unsigned note, int trim)
{
    reset(); track_t *t=&trk[0]; int32_t out[CTL], prev=0; double first=0,last=0; int crosses=0;
    t->p[P_SCALE]=scale; t->p[P_TC0+note%12]=trim;
    trk_note_on(t,note,100);
    for(unsigned j=0;j<FS*2/CTL;j++) {
        track_render(t,out,CTL);
        for(unsigned i=0;i<CTL;i++) {
            unsigned pos=j*CTL+i;
            if(pos>FS/4 && prev<=0 && out[i]>0) {
                double at=pos-1+(double)-prev/(out[i]-prev);
                if(!crosses) first=at;
                last=at; crosses++;
            }
            prev=out[i];
        }
    }
    assert(crosses>50); return (crosses-1)*FS/(last-first);
}
static void audio_test(void)
{
    const struct { unsigned s,n; int trim,c; } cases[] = {
        {0,64,0,0},{16,64,0,-50},{16,71,0,-50},{18,62,0,-50},
        {16,64,13,-37},{16,64,-13,-63},{0,69,1,1},{0,69,-1,-1},{16,67,0,0}
    };
    for(unsigned k=0;k<NELEM(cases);k++) {
        double got=audio_frequency(cases[k].s,cases[k].n,cases[k].trim);
        double e=error(got,hz(cases[k].n,cases[k].c));
        printf("audio: note %u, %+d cents: %.4f Hz (error %+.3f cents)\n",cases[k].n,cases[k].c,got,e);
        assert(fabs(e)<0.5);
    }
    reset(); track_t *t=&trk[0]; int32_t out[CTL];
    t->p[P_SCALE]=16; t->p[P_TC0+4]=13;
    trk_note_on(t,60,100); trk_note_on(t,64,100); trk_note_on(t,67,100);
    trk_note_on(&trk[1],64,100);
    track_render(t,out,CTL); track_render(&trk[1],out,CTL);
    for(unsigned n=60;n<=67;n+=(n==60?4:3)) {
        voice_t *v=voice(t,n); uint32_t before=v->ph[0];
        track_render(t,out,CTL);
        double got=(uint32_t)(v->ph[0]-before)*(double)FS/CTL/4294967296.0;
        assert(fabs(error(got,hz(n,n==64?-37:0)))<0.5);
    }
    assert(voice(&trk[1],64)->pitch16==64*16);
    t->p[P_SCALE]=0; t->p[P_TC0+4]=0;
    for(int i=0;i<100;i++) track_render(t,out,CTL);
    assert(voice(t,64)->pitch_cur==64*16);
    trk_note_off(t,64); assert(!voice(t,64)->gate && voice(t,60)->gate && voice(t,67)->gate);
    puts("tuning: simultaneous notes, track isolation, live retuning and original note-off identity pass");
}
static void independent_rates(void)
{
    const int offsets[]={-50,-37,-1,1,13,50};
    for(unsigned k=0;k<NELEM(offsets);k++) {
        reset(); track_t *t=&trk[0]; host_preset(t,ENGI_SAMPLE,0);
        t->p[P_E0]=0; t->p[P_E1]=0; t->p[P_TC4]=offsets[k];
        trk_note_on(t,64,100); voice_t *v=voice(t,64);
        const smp_zone_t *z=smp_zone(v->s[4]); assert(z->n && v->s[6]==0);
        vmod_t m={0}; m.pitch16=tuning_pitch16(t,64); m.tuning_fine=tuning_residual(t,64);
        m.amp0=m.amp1=32767; int32_t out[CTL]={0}; sample_render(t,v,out,1,&m);
        double want=pow(2,(64-z->root16/16.0+offsets[k]/100.0)/12)*z->rate/65536.0;
        double got=v->ph[0]-1+v->ph[1]/65536.0;
        assert(fabs(error(got,want))<0.6);
        reset(); t=&trk[0]; memset(eng_mem,0,sizeof eng_mem); host_preset(t,8,0) /* GRAIN */;
        t->p[P_E0]=t->p[P_E1]=t->p[P_E4]=t->p[P_E6]=0; t->p[P_TC4]=offsets[k];
        trk_note_on(t,64,100); v=voice(t,64);
        gr_part_t *g=gr_of(t); gr_grain_t *found=0;
        for(unsigned n=0;n<100 && !found;n++) {
            track_render(t,out,CTL);
            for(unsigned j=0;j<GR_NG;j++) if(g->g[j].owner) {found=&g->g[j];break;}
        }
        assert(found); z=gr_zone(0,v->s[0]);
        want=pow(2,(64-z->root16/16.0+offsets[k]/100.0)/12)*z->rate/65536.0;
        assert(fabs(error(found->step/65536.0,want))<0.6);
    }
    reset(); track_t *t=&trk[0]; host_preset(t,ENGI_GURDY,0);
    t->p[P_E0]=t->p[P_E7]=0; t->p[P_SCALE]=16; t->p[P_TC4]=13;
    t->p[P_TC0]=7; t->p[P_TC7]=-13;
    trk_note_on(t,64,100); voice_t *v=voice(t,64); int32_t out[CTL];
    uint32_t before[3]={v->ph[0],v->ph[1],v->ph[2]}; track_render(t,out,CTL);
    const unsigned notes[]={64,36,43}; const int cents[]={-37,7,-13};
    for(unsigned j=0;j<3;j++) {
        double got=(uint32_t)(v->ph[j]-before[j])*(double)FS/CTL/4294967296.0;
        assert(fabs(error(got,hz(notes[j],cents[j])))<0.5);
    }
    puts("tuning: SAMPLE and GRAIN playback rates retain fine cents; GURDY melody and both drones tune independently");
}
static void input_test(void)
{
    reset(); track_t *t=&trk[0]; kb_prev=0; usb.config=1; mo_w=mo_r=0;
    t->p[P_QUANT]=2; t->p[P_SCALE]=16; t->p[P_AMODE]=1;
    song.playing=song.rec=1;
    fm1_in.notes=1u<<11; keyboard_block(); arp_tick(t,1);
    assert(t->step[0].n==1 && t->step[0].note[0]==64);
    assert(voice(t,64)->pitch16==64*16-8);
    fm1_in.notes=0; keyboard_block(); arp_off(t); seq_stop(); song.rec=0;
    t->p[P_AMODE]=0; t->p[P_SLEN]=1; t->seq_active=1;
    seq_start(); events_block(CTL);
    assert(voice(t,64)->pitch16==64*16-8 && voice(t,64)->gate);
    seq_stop(); assert(!voice(t,64)->gate);
    reset(); t=&trk[0]; t->p[P_SCALE]=18;
    memset(midi_ch,0,sizeof midi_ch); memset(midi_notes,0,sizeof midi_notes);
    memset(midi_owners,0,sizeof midi_owners); mi_r=mi_w=0;
    midi_enqueue(9u | 0x90u<<8 | 62u<<16 | 100u<<24,1); events_block(CTL);
    assert(voice(t,62)->pitch16==62*16-8);
    midi_enqueue(8u | 0x80u<<8 | 62u<<16,1); events_block(CTL);
    assert(!voice(t,62)->gate);
    puts("tuning: WHITE keys, arp recording, sequencer playback and MIDI input keep microtonal pitches and release correctly");
}
static void persistence_test(void)
{
    static uint8_t bytes[3584]; project_t q,r; project_store_t packed;
    FILE *f=fopen("tests/fixtures/ghoulbox-1.1.fun8","rb"); assert(f);
    assert(fread(bytes,1,sizeof bytes,f)==sizeof bytes); fclose(f);
    assert(proj_import(&q,bytes,sizeof bytes) && proj_ok(&q));
    for(unsigned t=0;t<NTRK;t++) {
        assert(q.t[t].p[P_SCALE]==1+t && q.t[t].p[P_ROOT]==t);
        assert(q.t[t].p[P_E0]==3 && q.t[t].p[P_E4]==100+t);
        for(unsigned i=P_TC0;i<=P_TC11;i++) assert(q.t[t].p[i]==0);
        assert(q.t[t].step[0].note[1]==64 && q.t[t].step[0].n==2);
        assert(!memcmp(q.fm6[t],FM6_FACTORY[t],FM6_PACKED));
        q.t[t].p[P_SCALE]=16+t; q.t[t].p[P_TC0+t]=-63+t; q.t[t].p[P_TC11]=63;
    }
    assert(q.motion.event[0].param==P_E4 && q.motion.event[0].value==77);
    q.sum=proj_sum(&q); assert(proj_pack(&packed,&q));
    assert(proj_import(&r,&packed,sizeof packed) && !memcmp(q.t,r.t,sizeof q.t));
    packed.raw[83]^=1; assert(!proj_import(&r,&packed,sizeof packed));
    up_rec_t u; int16_t p[P_COUNT],def[P_COUNT]={0};
    f=fopen("tests/fixtures/ghoulbox-1.1.up4","rb"); assert(f);
    assert(fread(&u,1,sizeof u,f)==sizeof u); fclose(f);
    assert(up_valid(&u)); up_params(&u,p,def);
    assert(p[P_E0]==3 && p[P_E4]==100);
    for(unsigned i=P_TC0;i<=P_TC11;i++) assert(p[i]==0);
    u.ver=UP_VER; u.np=P_COUNT;
    for(unsigned i=0;i<P_COUNT;i++) up_set_value(&u,i,q.t[0].p[i]);
    assert(up_valid(&u)); up_params(&u,p,def);
    assert(p[P_TC0]==-63 && p[P_TC11]==63 && p[P_SCALE]==16);
    u.ver=4; assert(!up_valid(&u));
    puts("tuning: frozen GHOULBOX 1.1 project/preset migration, FM6, motion IDs, tuned round trip and corrupt data pass");
}
#ifndef TUNING_TEST_NO_MAIN
int main(void) { pitch_math(); audio_test(); independent_rates(); input_test(); persistence_test(); return 0; }
#endif
