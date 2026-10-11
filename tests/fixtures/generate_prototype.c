/* SPDX-License-Identifier: GPL-3.0-only
 * Run only against the frozen prototype at 5a8438a; see README.md. */
#define main old_tuning_main
#include "tuning_test.c"
#undef main
int main(void) {
    project_t q; project_store_t s; up_rec_t u;
    FILE *f = fopen("tests/fixtures/ghoulbox-1.1.fun8", "rb");
    uint8_t b[3584]; assert(f && fread(b,1,sizeof b,f)==sizeof b); fclose(f);
    assert(proj_import(&q,b,sizeof b));
    for (unsigned t=0;t<NTRK;t++) { q.t[t].p[P_SCALE]=16+t; q.t[t].p[P_TC0+t]=-23-(int)t; q.t[t].p[P_TC11]=37; }
    q.motion.count=2; q.motion.on=1;
    q.motion.event[0]=(motion_event_t){0,P_PAN,-17};
    q.motion.event[1]=(motion_event_t){1,P_E4,103};
    q.sum=proj_sum(&q); assert(proj_pack(&s,&q));
    f=fopen("tests/fixtures/ghoulbox-prototype.gbm1","wb"); assert(f); fwrite(&s,1,sizeof s,f); fclose(f);
    memset(&u,0,sizeof u); u.used=UP_USED;u.ver=UP_VER;u.engine=q.t[0].engine;u.np=P_COUNT;
    memcpy(u.name,"PROTO TUNING",12);for(unsigned i=0;i<P_COUNT;i++)up_set_value(&u,i,q.t[0].p[i]);
    f=fopen("tests/fixtures/ghoulbox-prototype.up6","wb");assert(f);fwrite(&u,1,sizeof u,f);fclose(f);
}
