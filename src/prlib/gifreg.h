#ifndef PRLIB_GIFREG_H
#define PRLIB_GIFREG_H

#include "common.h"

#include <eetypes.h>
#include <eestruct.h>
#include <libdma.h>

struct PrGifPackedAd {
    u_long data;
    u_long addr;
};

class PrDmaStripForSetGifRegister {
public:
    PrDmaStripForSetGifRegister() {
        mStripLen = 0;
        mFrozen = false;
    }
    ~PrDmaStripForSetGifRegister() {
        /* Empty */
    }

    void Initialize() {
        mStripLen = 0;
        mFrozen = false;
    }

    void Append(u_int addr, const u_long& data);
    void Freeze(u_char id, const void *addr);

public:
    sceDmaTag mTag;
    sceGifTag mGifTag;
    PrGifPackedAd mStrip[16];
    int mStripLen;
    bool mFrozen;
    char unk128[0x8];
};

enum PrSetGifRegisterMode {
    eGifRegisterMode_Unk0 = 0,
    eGifRegisterMode_Unk1 = 1,
    eGifRegisterMode_Unk2 = 2,
    eGifRegisterMode_Unk3 = 3,
    eGifRegisterMode_Unk4 = 4,
    eGifRegisterMode_Unk5 = 5,
};

void PrInitializeDmaStripGifRegister(sceGsZbuf zbuf);
PrDmaStripForSetGifRegister* PrGetDmaStripGifRegister(PrSetGifRegisterMode mode);

#endif /* PRLIB_GIFREG_H */
