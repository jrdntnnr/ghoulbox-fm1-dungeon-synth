/* SPDX-License-Identifier: GPL-3.0-only */
/* Compile against b9d47b7, or eaf04bd with -DUPSTREAM_GHOUL2.
 * Frozen fixtures must be written by the original serializer. */
#define main hostsim_main
#include "hostsim.c"
#undef main
#define PROJ_HOST 1
#include "project.c"
#define UP_HOST 1
#include "upreset.c"
int main(int argc, char **argv)
{
    project_t q = {0}; project_store_t st; up_rec_t u = {0};
    q.magic=PROJ_MAGIC; q.size=sizeof q; q.parts=NPART; q.phys=PROJ_PHYS;
    chain_defaults(&q.chain); memcpy(q.name,"DESERT SAVE",11);
    for (int t=0;t<NTRK;t++) {
        q.t[t].engine=0;
        for(int i=0;i<P_COUNT;i++) q.t[t].p[i]=param_desc_of(0,i)->def;
        q.t[t].p[P_SCALE]=16+t%3; q.t[t].p[P_ROOT]=t;
        q.t[t].p[P_E0]=3; q.t[t].p[P_E4]=100+t;
#ifdef UPSTREAM_GHOUL2
        q.t[t].p[P_TEMP]=1+t%3;
#else
        q.t[t].p[P_TC0+t]=-23-t; q.t[t].p[P_TC11]=37;
#endif
        q.t[t].step[0]=(step_t){.n=2,.note={60,64},.vel=90,.time=ST_NOTE};
        memcpy(q.fm6[t],FM6_FACTORY[t],FM6_PACKED);
    }
#ifdef UPSTREAM_GHOUL2
    q.g[G_KMODE]=2; q.g[G_KTRK]=1; q.g[G_KOCT]=4; q.g[G_KSPLIT]=7;
    q.g[G_AMB]=32; q.g[G_CRKL]=100;
#else
    song.g[G_TXSRC]=4; song.g[G_TXLVL]=73; song.g[G_TXTONE]=-20;
    song.g[G_TXMOVE]=52; song.g[G_TXRATE]=3; song.g[G_TXWHEN]=1;
    proj_tx_put(&q);
#endif
    q.g[G_TAPE]=31; q.g[G_CRSH]=27;
    q.motion.count=2; q.motion.on=1;
    q.motion.event[0]=(motion_event_t){0,P_E4|MOTION_LOCK,77};
    q.motion.event[1]=(motion_event_t){1,P_PAN,-43};
    q.sum=proj_sum(&q); if(!proj_pack(&st,&q)) return 2;
    FILE *f=fopen(argv[1],"wb"); if(!f)return 3; fwrite(&st,1,sizeof st,f); fclose(f);
    u.used=UP_USED; u.ver=UP_VER; u.engine=0; u.np=P_COUNT; memcpy(u.name,"DESERT SAVE",11);
    for(int i=0;i<P_COUNT;i++) up_set_value(&u,i,q.t[0].p[i]);
    f=fopen(argv[2],"wb"); if(!f)return 3; fwrite(&u,1,sizeof u,f); fclose(f);
    return 0;
}
