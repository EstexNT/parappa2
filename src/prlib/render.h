#ifndef PRLIB_RENDER_H
#define PRLIB_RENDER_H

#include "common.h"

#include <eetypes.h>
#include <eestruct.h>
#include <libdma.h>

struct PrVu1InitPacket {
    sceDmaTag     tag;
    sceGifTag     giftag;

    sceGsFrame    frame;
    u_long        frame_addr;

    sceGsZbuf     zbuf;
    u_long        zbuf_addr;

    sceGsXyoffset xyoffset;
    u_long        xyoffset_addr;

    sceGsScissor  scissor;
    u_long        scissor_addr;
} PR_ALIGNED(128);

#endif /* PRLIB_RENDER_H */
