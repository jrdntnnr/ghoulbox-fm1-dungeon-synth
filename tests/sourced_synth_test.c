/* SPDX-License-Identifier: GPL-3.0-only */
/* Real audio, chord voices, independent reed drone, and stored preset/project values.
 * Run with an output directory; audio is also consumed by check_sourced_audio.py. */
#define UI_TEST_NO_MAIN 1
#include "ui_test.c"
#include <assert.h>

static unsigned preset_named(unsigned e, const char *name)
{
    for (unsigned k=0;k<ENGINES[e]->npresets;k++)
        if (!strcmp(name,ENGINES[e]->presets[k].name)) return k;
    assert(0); return 0;
}
static track_t *start(unsigned e, const char *name)
{
    memset(trk,0,sizeof trk); memset(&song,0,sizeof song);
    memset(midi_bend_q8,0,sizeof midi_bend_q8); memset(midi_bend_target,0,sizeof midi_bend_target);
    memset(kb_chn,0,sizeof kb_chn); memset(chord_last,0,sizeof chord_last);
    host_tracks_init(); host_preset(&trk[0],e,preset_named(e,name));
    trk[0].p[P_SCALE]=16; /* Rast on C, independently expected E/B half flat */
    return &trk[0];
}
static voice_t *playing(track_t *t, unsigned note)
{
    for(unsigned k=0;k<NVOICE;k++) if(t->v[k].active && t->v[k].gate && t->v[k].note==note) return &t->v[k];
    assert(0);return 0;
}
static void render_file(const char *dir,const char *name,track_t *t,unsigned samples)
{
    char path[512];snprintf(path,sizeof path,"%s/%s.wav",dir,name);
    FILE *f=fopen(path,"wb");assert(f); samples=(samples/CTL)*CTL;wav_hdr(f,samples);
    int32_t out[CTL]; int peak=0;double sum=0;
    for(unsigned j=0;j<samples;j+=CTL) {
        track_render(t,out,CTL);
        for(unsigned i=0;i<CTL;i++) {int a=abs(out[i]);if(a>peak)peak=a;sum+=(double)out[i]*out[i];wav_put(f,out[i],out[i]);}
    }
    fclose(f);printf("%s peak %d RMS %.1f\n",name,peak,sqrt(sum/samples));
    assert(peak>200 && peak<32768);
}
static void persistence(track_t *t)
{
    project_t a,b;project_store_t packed;
    project_capture(&a);assert(proj_pack(&packed,&a));assert(proj_import(&b,&packed,sizeof packed));
    proj_bound(&b);assert(!memcmp(a.t[0].p,b.t[0].p,sizeof a.t[0].p));
    up_rec_t r={0};r.ver=UP_VER;r.np=P_COUNT;r.engine=t->engine;
    strcpy(r.name,"SOURCED");
    for(unsigned k=0;k<P_COUNT;k++) up_set_value(&r,k,t->p[k]);
    int16_t p[P_COUNT];up_values(&r,p);
    for(unsigned k=P_E0;k<=P_E7;k++) assert(p[k]==t->p[k]);
}
int main(int argc,char **argv)
{
    assert(argc==2);const char *dir=argv[1];
    const char *names[]={"SANTUR","DUSK SANTUR","MIJWIZ","ARGHUL"};
    unsigned trio=0;while(ENGINES[trio]!=&ENG_TRIO) trio++;
    for(unsigned k=0;k<4;k++) {
        unsigned e=k<2?ENGI_PHYS:trio;
        track_t *t=start(e,names[k]);persistence(t);
        /* Real chord generator: Rast C/E-half-flat/G, with an extra +13c trim on E. */
        t->p[P_TC4]=13;t->p[P_CHRD]=CH_DIA3;kb_note[7]=60;key_on(7,t);
        assert(kb_chn[7]==3);playing(t,60);playing(t,64);playing(t,67);
        char file[80];snprintf(file,sizeof file,"%u_chord",k);render_file(dir,file,t,FS);
        key_off(7,t);for(unsigned i=0;i<NVOICE;i++)assert(!t->v[i].gate);
        /* Render single notes at zero course spread: measure actual audio, not just pitch inputs. */
        const unsigned notes[]={48,52,60,64,71,76,84};
        for(unsigned j=0;j<NELEM(notes);j++) {
            t=start(e,names[k]);t->p[P_TC4]=13;
            if(k<2)t->p[P_E1]=0;else t->p[P_E3]=0;
            trk_note_on(t,notes[j],100);
            snprintf(file,sizeof file,"%u_note_%u",k,notes[j]);render_file(dir,file,t,FS);
        }
        /* Preset as shipped, with beating; and release actually reaches silence. */
        t=start(e,names[k]);trk_note_on(t,64,100);
        snprintf(file,sizeof file,"%u_preset",k);render_file(dir,file,t,FS*2);
        trk_note_off(t,64);int32_t out[CTL];for(unsigned j=0;j<FS*12/CTL;j++)track_render(t,out,CTL);
        for(unsigned i=0;i<NVOICE;i++)assert(!t->v[i].active);
    }
    /* Arghul drone holds D3 across melody, bend and melody-trim changes; its own
     * tonic trim and global reference tuning still apply. Check its phase, independently. */
    track_t *t=start(trio,"ARGHUL");t->p[P_ROOT]=2;t->p[P_TC0]=11;song.g[G_TUNE]=-9;
    trk_note_on(t,66,100);int32_t out[CTL];track_render(t,out,CTL);
    voice_t *v=playing(t,66);double expected=440*pow(2,(50-69+0.02)/12.0);
    for(unsigned j=0;j<3;j++) {
        midi_bend_target[0]=j?256:0;t->p[P_TC4]=j?35:0;
        uint32_t before=v->ph[1];track_render(t,out,CTL);
        double got=(uint32_t)(v->ph[1]-before)*(double)FS/CTL/4294967296.0;
        assert(fabs(1200*log2(got/expected))<0.5);
    }
    puts("sourced synths: chord generation, release, project/preset persistence and independent tuned drone pass");
    return 0;
}
