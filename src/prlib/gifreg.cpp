#include "gifreg.h"

static bool gifRegisterModeInitialized = false;
static PrDmaStripForSetGifRegister setGifRegisterMode[6];

void PrDmaStripForSetGifRegister::Append(u_int addr, const u_long& data) {
    mStrip[mStripLen].addr = addr;
    mStrip[mStripLen].data = data;
    mStripLen += 1;
}

void PrDmaStripForSetGifRegister::Freeze(u_char id, const void *addr) {
    int len = mStripLen;

    mFrozen = true;

    mTag.qwc = mStripLen + 1;
    mTag.mark = 0;
    mTag.id = id;
    mTag.next = (sceDmaTag*)addr;
    mTag.p[0] = SCE_VIF1_SET_FLUSH(0);
    mTag.p[1] = SCE_VIF1_SET_DIRECT(mStripLen + 1, 0);

    mGifTag.NLOOP = len;
    mGifTag.EOP = true;
    mGifTag.pad16 = 0;
    mGifTag.id = 0;
    mGifTag.PRE = false;
    mGifTag.PRIM = 0;
    mGifTag.FLG = SCE_GIF_PACKED;
    mGifTag.NREG = 1;
    mGifTag.REGS0 = SCE_GIF_PACKED_AD;
}

void PrInitializeDmaStripGifRegister(sceGsZbuf zbuf) {
    if (gifRegisterModeInitialized) {
        return;
    }

    for (int i = 0; i < PR_ARRAYSIZEU(setGifRegisterMode); i++) {
        PrDmaStripForSetGifRegister& strip = setGifRegisterMode[i];
        strip.Initialize();

        switch (i) {
        case eGifRegisterMode_Unk4:
            zbuf.ZMSK = 1;

            strip.Append(SCE_GS_TEST_1, SCE_GS_SET_TEST_1(1, 6, 0, 0, 0, 0, 1, 1));
            strip.Append(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1(0, 1, 0, 1, 128));
            strip.Append(SCE_GS_TEX1_1, SCE_GS_SET_TEX1_1(1, 0, 1, 1, 0, 0, 0));
            strip.Append(SCE_GS_ZBUF_1, *(u_long*)&zbuf);
            strip.Append(SCE_GS_FBA_1, SCE_GS_SET_FBA_1(0));
            break;
        case eGifRegisterMode_Unk0:
            zbuf.ZMSK = 0;

            strip.Append(SCE_GS_TEST_1, SCE_GS_SET_TEST_1(1, 6, 0, 0, 0, 0, 1, 2));
            strip.Append(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1(0, 1, 0, 1, 128));
            strip.Append(SCE_GS_TEX1_1, SCE_GS_SET_TEX1_1(1, 0, 1, 1, 0, 0, 0));
            strip.Append(SCE_GS_ZBUF_1, *(u_long*)&zbuf);
            strip.Append(SCE_GS_FBA_1, SCE_GS_SET_FBA_1(0));
            break;
        case eGifRegisterMode_Unk1:
            zbuf.ZMSK = 1;

            strip.Append(SCE_GS_ZBUF_1, *(u_long*)&zbuf);
            strip.Append(SCE_GS_ZBUF_2, *(u_long*)&zbuf);
            strip.Append(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1(0, 1, 0, 1, 128));
            strip.Append(SCE_GS_ALPHA_2, SCE_GS_SET_ALPHA_1(0, 1, 0, 1, 128));
            break;
        case eGifRegisterMode_Unk2:
            strip.Append(SCE_GS_PRIM, SCE_GS_SET_PRIM(4, 1, 0, 0, 0, 0, 1, 0, 0));

            strip.Append(SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(255, 0, 0, 128, 0x00000001));
            strip.Append(SCE_GS_XYZ2, SCE_GS_SET_XYZ2(GS_X_COORD(270), GS_Y_COORD(62), -1));

            strip.Append(SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(255, 255, 0, 128, 0x00000001));
            strip.Append(SCE_GS_XYZ2, SCE_GS_SET_XYZ2(GS_X_COORD(370), GS_Y_COORD(62), -1));

            strip.Append(SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(255, 0, 255, 128, 0x00000001));
            strip.Append(SCE_GS_XYZ2, GS_X_COORD(270) | (GS_Y_COORD(162) << 16));

            strip.Append(SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(0, 255, 255, 128, 0x00000001));
            strip.Append(SCE_GS_XYZ2, GS_X_COORD(370) | (GS_Y_COORD(162) << 16));
            break;
        case eGifRegisterMode_Unk3:
            zbuf.ZMSK = 0;

            strip.Append(SCE_GS_ZBUF_1, *(u_long*)&zbuf);
            strip.Append(SCE_GS_FBA_1, SCE_GS_SET_FBA_1(0));
            strip.Append(SCE_GS_TEST_1, SCE_GS_SET_TEST_1(1, 0, 0, 2, 0, 0, 1, 1));
            strip.Append(SCE_GS_PRIM, SCE_GS_SET_PRIM(6, 0, 0, 0, 0, 0, 0, 0, 0));
            strip.Append(SCE_GS_XYZ2, SCE_GS_SET_XYZ2(GS_X_COORD(0), GS_Y_COORD(0), 0));
            strip.Append(SCE_GS_XYZ2, SCE_GS_SET_XYZ2(GS_X_COORD(640), GS_Y_COORD(224), 0));
            strip.Append(SCE_GS_TEST_1, SCE_GS_SET_TEST_1(1, 6, 0, 0, 0, 0, 1, 2));
            strip.Append(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1(0, 1, 0, 1, 128));
            strip.Append(SCE_GS_TEX1_1, SCE_GS_SET_TEX1_1(1, 0, 1, 1, 0, 0, 0));
            break;
        case eGifRegisterMode_Unk5:
            zbuf.ZMSK = 0;

            strip.Append(SCE_GS_TEST_1, SCE_GS_SET_TEST_1(1, 0, 0, 2, 0, 0, 1, 1));
            strip.Append(SCE_GS_ZBUF_1, *(u_long*)&zbuf);
            strip.Append(SCE_GS_FBA_1, SCE_GS_SET_FBA_1(0));
            break;
        }

        strip.Freeze(0x60 /* DMAret */, NULL);
    }

    gifRegisterModeInitialized = true;
}

PrDmaStripForSetGifRegister* PrGetDmaStripGifRegister(PrSetGifRegisterMode mode) {
    return &setGifRegisterMode[mode];
}

INCLUDE_RODATA("asm/nonmatchings/prlib/gifreg", D_00396748);
