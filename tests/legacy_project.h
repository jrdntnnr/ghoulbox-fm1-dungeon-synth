/* SPDX-License-Identifier: GPL-3.0-only */
/* Decode the current store, then emit the frozen FUN7 schema used by tests.
 * Independent binary fixtures additionally cover original and prototype GHOULBOX. */
#include "old_pack.h"
static void legacy_fun7(uint8_t *out, const project_store_t *in)
{
    project_t q;
    if (!proj_unpack(&q, in->raw, sizeof *in) || !old_pack(out, &q, OLD_FUN7, 91u))
        memset(out, 0, PROJ_STORE_V7);
}
