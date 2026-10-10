/* SPDX-License-Identifier: GPL-3.0-only */
/* Fixture adapter: remove the twelve tuning parameters from a packed GBM1
 * record and emit a genuine 91-parameter FUN7 layout, including old motion IDs.
 * Independent frozen FUN8 bytes are also covered by tuning_test.c. */
static void legacy_fun7(uint8_t *out, const project_store_t *in)
{
    uint32_t src=68, dst=68, sum;
    memset(out,0,PROJ_STORE_V7); memcpy(out,in->raw,68); out[66]=91;
    for(unsigned t=0;t<NTRK;t++) {
        memcpy(out+dst,in->raw+src,83); dst+=83; src+=95;
        memcpy(out+dst,in->raw+src,8+2+NSTEP*9);
        dst+=8+2+NSTEP*9; src+=8+2+NSTEP*9;
    }
    memcpy(out+dst,in->raw+src,sizeof(chain_config_t)); dst+=sizeof(chain_config_t); src+=sizeof(chain_config_t);
    motion_store_t m; memcpy(&m,in->raw+src,sizeof m);
    for(unsigned i=0;i<m.count;i++) if(m.event[i].param>=95) m.event[i].param-=12;
    memcpy(out+dst,&m,sizeof m);
    memcpy(out+PROJ_STORE_V7-16,in->raw+PROJ_NAME_OFF,12);
    uint32_t magic=0x46554E37u,size=PROJ_STORE_V7;
    memcpy(out,&magic,4); memcpy(out+4,&size,4);
    sum=proj_hash(out,PROJ_STORE_V7-4); memcpy(out+PROJ_STORE_V7-4,&sum,4);
}
