/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#define main hostsim_main
#include "hostsim.c"
#undef main
#define PROJ_HOST 1
#include "../firmware/src/project.c"
#define UP_HOST 1
#include "../firmware/src/upreset.c"
static void fixture(void *p, size_t n, const char *name)
{
    FILE *f=fopen(name,"rb"); assert(f); assert(fread(p,1,n,f)==n); assert(fgetc(f)==EOF); fclose(f);
}
int main(void)
{
    static project_t a,b; static project_store_t packed;
    /* Every descriptor endpoint, every engine, full patterns/motion/sections,
     * and signed tuning must fit without taking a byte from the FM6 patches. */
    for (unsigned e=0;e<NENGINES;e++) for(unsigned hi=0;hi<2;hi++) {
        memset(&a,0,sizeof a); a.magic=PROJ_MAGIC;a.size=sizeof a;a.parts=NPART;a.phys=PROJ_PHYS;
        for(unsigned t=0;t<NTRK;t++) {
            a.t[t].engine=e;
            for(unsigned i=0;i<P_COUNT;i++) {
                const param_desc_t *d=param_desc_of(e,i);
                a.t[t].p[i]=hi?d->max:d->min;
            }
            /* Retired SAMPLE PERC and DIGITAL intentionally convert, tested elsewhere. */
            if(e==ENGI_SAMPLE) a.t[t].p[P_E0]=0;
            memcpy(a.fm6[t],FM6_INIT,FM6_PACKED);
            for(unsigned i=0;i<NSTEP;i++) {
                a.t[t].step[i]=(step_t){{60,64,67,71},4,ST_NOTE,SF_ACCENT,127,255,255};
                step_set_nudge(&a.t[t].step[i],i%2?7:-8);step_set_ratchet(&a.t[t].step[i],4);
            }
        }
        a.chain.count=16;
        for(unsigned i=0;i<16;i++) {a.chain.row[i].repeat=i+1;for(unsigned t=0;t<NTRK;t++)a.chain.row[i].slot[t]=(i+t)%5;}
        a.motion.count=MOTION_MAX;a.motion.on=15;
        for(unsigned i=0;i<MOTION_MAX;i++)a.motion.event[i]=(motion_event_t){i,P_PAN|MOTION_LOCK,(int8_t)(i%128-64)};
        assert(proj_pack(&packed,&a));assert(proj_unpack(&b,packed.raw,sizeof packed));
        assert(!memcmp(a.t,b.t,sizeof a.t)&&!memcmp(&a.motion,&b.motion,sizeof a.motion));
        assert(!memcmp(a.fm6,b.fm6,sizeof a.fm6)&&!memcmp(&a.chain,&b.chain,sizeof a.chain));
    }
    uint8_t prototype[3648]; fixture(prototype,sizeof prototype,"tests/fixtures/ghoulbox-prototype.gbm1");
    assert(proj_import(&a,prototype,sizeof prototype));
    for(unsigned t=0;t<NTRK;t++) {
        assert(a.t[t].p[P_TC0+t]==-23-(int)t && a.t[t].p[P_TC11]==37);
        assert(a.t[t].p[P_SCALE]==16+(int)t && a.t[t].p[P_E4]==100+(int)t);
        assert(a.t[t].p[P_LN0]==127 && a.t[t].p[P_ITYPE]==0 && a.t[t].p[P_TMASK0]==63);
    }
    assert(a.motion.event[0].param==P_PAN && a.motion.event[1].param==P_E4);
    assert(proj_pack(&packed,&a)&&proj_import(&b,&packed,sizeof packed)&&!memcmp(a.t,b.t,sizeof a.t));
    up_rec_t u;int16_t values[P_COUNT],def[P_COUNT];
    for(unsigned i=0;i<P_COUNT;i++)def[i]=TP[i].def;
    fixture(&u,sizeof u,"tests/fixtures/ghoulbox-prototype.up6");assert(up_valid(&u));up_params(&u,values,def);
    assert(values[P_TC0]==-23&&values[P_TC11]==37&&values[P_E4]==100&&values[P_LN0]==127);
    u.ver=7;assert(up_valid(&u)&&up_grid(&u));
    /* Felucca's developmental count 103 has no tuning: version, not count, dispatches. */
    u.ver=4;u.np=103;
    for(unsigned i=0;i<103;i++)up_set_value(&u,i,0);
    up_set_value(&u,83,111);up_set_value(&u,95,3);up_set_value(&u,99,109);
    assert(up_valid(&u));up_params(&u,values,def);
    assert(values[P_LN0]==111&&values[P_TC0]==0&&values[P_E0]==3&&values[P_E4]==109);
    track_t *t=&trk[0];host_tracks_init();host_preset(t,0,0);
    t->p[P_SCALE]=SCALE_CUSTOM_ID;t->p[P_TMASK0]=0x35;t->p[P_TMASK1]=0x2a;t->p[P_TC4]=-50;
    assert(scale_mask(t)==0xab5&&tuning_cents(t,64)==-50&&tuning_cents(t,67)==0);
    t->p[P_ROOT]=2;assert(tuning_cents(t,66)==-50);
    t->p[P_TMASK0]=t->p[P_TMASK1]=0;assert(scale_mask(t)==1); /* tonic always playable */
    puts("Integration formats: all engine endpoints, full projects, prototype migration, upstream count collision and custom tuning pass");
    return 0;
}
