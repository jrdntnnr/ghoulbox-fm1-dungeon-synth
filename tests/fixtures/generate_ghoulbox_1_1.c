/* SPDX-License-Identifier: GPL-3.0-only */
/* Compile against c7891e0, not the current firmware. See README.md. */
#define main hostsim_main
#include "hostsim.c"
#undef main
#define PROJ_HOST
#include "project.c"
#define UP_HOST
#include "upreset.c"
int main(int argc, char **argv) {
    project_t q = {0}; project_store_t st; up_rec_t u = {0};
    q.magic = PROJ_MAGIC; q.size = sizeof q; q.parts = NPART; q.phys = PROJ_PHYS;
    chain_defaults(&q.chain); memcpy(q.name,"LEGACY RAST",11);
    for (int t=0;t<NTRK;t++) {
        q.t[t].engine=0; q.t[t].p[P_SCALE]=1+t; q.t[t].p[P_ROOT]=t;
        q.t[t].p[P_E0]=3; q.t[t].p[P_E4]=100+t;
        q.t[t].step[0]=(step_t){.n=2,.note={60,64},.vel=90,.time=ST_NOTE};
        memcpy(q.fm6[t],FM6_FACTORY[t],FM6_PACKED);
    }
    q.motion.count=1; q.motion.event[0].param=P_E4; q.motion.event[0].value=77;
    q.sum=proj_sum(&q); if(!proj_pack(&st,&q)) return 2;
    FILE *f=fopen(argv[1],"wb"); fwrite(&st,1,sizeof st,f); fclose(f);
    u.used=UP_USED; u.ver=UP_VER; u.engine=0; u.np=P_COUNT; memcpy(u.name,"LEGACY",6);
    for(int i=0;i<P_COUNT;i++) up_set_value(&u,i,q.t[0].p[i]);
    f=fopen(argv[2],"wb"); fwrite(&u,1,sizeof u,f); fclose(f); return 0;
}
