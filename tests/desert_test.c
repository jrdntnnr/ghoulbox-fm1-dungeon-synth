/* SPDX-License-Identifier: GPL-3.0-only */
/* Actual optional-bank decoding and microtonal chords, plus a scene audition. */
#define UI_TEST_NO_MAIN 1
#include "ui_test.c"
#include <assert.h>

static void read_bank(unsigned slot, const char *name)
{
    char path[160];
    snprintf(path,sizeof path,"assets/desert/%s.hdr",name);
    FILE *f=fopen(path,"rb"); assert(f);
    uint8_t *dest=(uint8_t *)host_slots+slot*SMP_USER_SIZE;
    assert(fread(dest,1,sizeof(smp_user_hdr_t),f)==sizeof(smp_user_hdr_t)); fclose(f);
    const smp_user_hdr_t *h=(const smp_user_hdr_t *)dest;
    assert(h->data_len<=SMP_USER_SIZE-SMP_USER_DATA);
    snprintf(path,sizeof path,"assets/desert/%s.bin",name);f=fopen(path,"rb");assert(f);
    assert(fread(dest+SMP_USER_DATA,1,h->data_len,f)==h->data_len); fclose(f);
    smp_user_scan(slot); assert(usr_nz[slot]==h->nz);
}
static voice_t *find_voice(track_t *t,unsigned note)
{
    for(unsigned i=0;i<NVOICE;i++) if(t->v[i].active && t->v[i].note==note)return &t->v[i];
    assert(0);return 0;
}
int main(void)
{
    const char *names[]={"oud","qanun","ney-air","tar"};
    for(unsigned slot=0;slot<3;slot++)read_bank(slot,names[slot]);
    for(unsigned bank=0;bank<4;bank++) for(int trim=-13;trim<=13;trim+=13) {
        unsigned slot=bank%3;
        read_bank(slot,names[bank]);
        ui_power_on(); host_tracks_init();
        track_t *t=&trk[0]; host_preset(t,ENGI_SAMPLE,0);
        t->p[P_E0]=SMP_USR_V0+slot;t->p[P_SCALE]=18;t->p[P_ROOT]=2;
        char label[16];sound_name(t,label);
        assert(str_eq(label,bank==0?"OUD":bank==1?"QANUN":bank==2?"NEY AIR":"TAR"));
        t->p[P_TC2]=trim; t->p[P_CHRD]=CH_DIA7; t->p[P_VOIC]=VC_CLOSE;
        kb_note[7]=64; key_on(7,t);
        assert(kb_chn[7]==4);
        int32_t out[CTL]; track_render(t,out,CTL);
        double before[4];
        for(unsigned i=0;i<4;i++) {
            voice_t *v=find_voice(t,kb_chord[7][i]);before[i]=v->ph[0]+v->ph[1]/65536.0;
        }
        track_render(t,out,CTL);
        for(unsigned i=0;i<4;i++) {
            unsigned note=kb_chord[7][i];voice_t *v=find_voice(t,note);
            const smp_zone_t *z=smp_zone(v->s[4]);
            int cents=note%12==4 ? -50+trim : 0;
            double got=(v->ph[0]+v->ph[1]/65536.0-before[i])/CTL;
            double want=pow(2,(note-z->root16/16.0+cents/100.0)/12)*z->rate/65536.0;
            if(fabs(1200*log2(got/want))>=1.0) fprintf(stderr,"slot %u trim %d note %u root %d got %.9f want %.9f error %.3f ph %u %u coarse %d\n",slot,trim,note,z->root16,got,want,1200*log2(got/want),v->ph[0],v->ph[1],t->p[P_E1]);
            assert(fabs(1200*log2(got/want))<1.0);
        }
        key_off(7,t);
        for(unsigned i=0;i<NVOICE;i++)assert(!t->v[i].gate);
    }
    puts("desert: all four banks decode and play independently tuned Bayati sevenths, including trims and note-off");
    ui_power_on(); host_tracks_init(); gb_scene_load(5);song.master_q12=4096;
    for(unsigned i=0;i<NTRK;i++)trk[i].seq_active=1;
    seq_start();
    FILE *f=fopen("build/host/citadel.wav","wb");assert(f);wav_hdr(f,FS*16);
    int peak=0;double energy=0;
    for(unsigned n=0;n<FS*16/CTL;n++) {
        int32_t out[CTL*2];events_block(CTL);mix_block(out,CTL);
        for(unsigned i=0;i<CTL;i++) {
            int a=abs(out[i*2]);if(a>peak)peak=a;energy+=(double)out[i*2]*out[i*2];
            wav_put(f,out[i*2],out[i*2+1]);
        }
    }
    fclose(f);assert(peak>500 && energy>0);
    printf("desert: 16-second CITADEL audition, peak %d, RMS %.1f\n",peak,sqrt(energy/(FS*16)));
    return 0;
}
