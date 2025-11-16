#include "main/mbar.h"

#include "main/cmnfile.h"
#include "os/syssub.h"
#include "os/tim2.h"
#include "os/cmngifpk.h"
#include "os/system.h"

#include <stdio.h>
#include <string.h>

/* data 186288 */ extern TIM2_DAT tim2spr_tbl_tmp1[]; /* static, tim2spr_tbl */
/* data 186a68 */ extern u_int tmpColor[]; /* static */
/* data 186aa8 */ extern NIKO_CHAN_STR niko_chan_str_hook[]; /* static */
/* data 186af8 */ extern NIKO_CHAN_STR niko_chan_str_vs[]; /* static */
/* sdata 399558 */ extern NIKO_CHAN_STR *niko_chan_str_pp; /* static */
/* sdata 39955c */ extern int niko_chan_str_cnt; /* static */
/* sdata 399560 */ extern int hook_use_flag; /* static */
/* data 186b28 */ extern MBHOOK_STR mbhook_str[2]; /* static */
/* data 186b38 */ extern u_int hook_fr_dat[]; /* static */
/* sdata 399564 */ extern int exam_disp_cursor_timer; /* static */
/* sdata 399568 */ extern int scoreTentouFlag; /* static */
/* data 186b78 */ extern u_char scr_tenmetu_col[4][3]; /* static */
/* sdata 39956c */ extern int otehonAniCnt; /* static */
/* sdata 399570 */ extern int othon_frame; /* static */
/* sdata 399574 */ extern int vs_mouse_disp_flag; /* static */
/* data 186b88 */ extern METCOL_STR metcol_str[3]; /* static */
/* data 186bb0 */ extern MBA_CHAR_DATA mba_char_data[]; /* static */
/* sdata 399578 */ extern int mbar_pos_y_ofs; /* static */
/* data 186d78 */ extern u_char colp[][3]; /* static */
/* data 17c2b8 */ extern GAME_STATUS game_status; /* static */
/* data 186d90 */ extern void (*marSetPrgTbl[])(MBAR_REQ_STR*); /* static */
/* data 186da0 */ extern GUIMAP guimap[]; /* static */
// /* data 186ee0 */ static int guimap_single[];
// /* data 186ef0 */ static int guimap_vs[];
// /* data 186f00 */ static int guimap_sr[];
// /* data 186f08 */ static int guimap_hk[];
/* bss 1c70030 */ extern GLOBAL_PLY *exam_global_ply[4]; /* static */
/* sbss 399a80 */ extern GLOBAL_PLY *exam_global_ply_current; /* static */
/* bss 1c70040 */ extern int exam_global_ply_current_ply[4]; /* static */
/* bss 1c70050 */ extern int metFrameCnt[3]; /* static */
/* bss 1c70060 */ extern int metFrameCntLight[3]; /* static */
#if 0
/* sdata 399584 */ enum SCR_TENMETU_ENUM {
    SCR_TENMETU_NORMAL = 0,
    SCR_TENMETU_PL = 1,
    SCR_TENMETU_MI = 2,
    SCR_TENMETU_BLACK = 3,
    SCR_TENMETU_MAX = 4
};
#endif
/* bss 1c70070 */ extern u_char scr_tenmetu_col_dat[4][3]; /* static */
/* bss 1c70080 */ extern int conditionFramCnt[4]; /* static */
/* bss 1c70090 */ extern int vsScoreMove[4]; /* static */
/* bss 1c700a0 */ extern int vsScoreAni[4]; /* static */
/* bss 1c700b0 */ extern VS_SCR_CTRL vs_scr_ctrl[4]; /* static */
/* bss 1c70110 */ extern MBAR_REQ_STR mbar_req_str[5]; /* static */
/* sbss 399a84 */ extern int mbar_ctrl_time; /* static */
/* sbss 399a88 */ extern int mbar_ctrl_stage; /* static */
/* sbss 399a8c */ extern int mbar_ctrl_stage_selT; /* static */
/* bss 1c701c8 */ extern sceGifPacket mbar_gif; /* static */
/* sbss 399a90 */ extern PR_SCENEHANDLE guime_hdl; /* static */
/* sbss 399a94 */ extern PR_CAMERAHANDLE guime_camera_hdl; /* static */

static void   clrColorBuffer(int id);
static void   NikoReset(void);
static void   MbarNikoDisp(sceGifPacket *gifpk_pp);
static void   MbarHookPoll(void);
static int    vsScr2Move(long scr);
static void   vsAnimationPoll(void);
static void   metColorInit(void);
static void   metColorSet(EXAM_TYPE exam_type, float per);
static void   ExamDispOn(void);
static u_long hex2dec_mbar_tmp(u_long data);
static u_long hex2decPlMi(long data);
static void   examScoreSet(sceGifPacket *ex_gif_pp);
static void   examLevelDisp(sceGifPacket *ex_gif_pp);
static void   MbarCl1CharSet(int col_num, int moto_num);
static void   MbarCharSetSub(void);
static int    MbarGetDispPosX(int tick);
static int    MbarGetDispPosY(int tick);
static int    MbarGetTimeArea(MBAR_REQ_STR *mr_pp);
static int    MbarGetTimeArea2(MBAR_REQ_STR *mr_pp);
static int    MbarGetStartTime(MBAR_REQ_STR *mr_pp);
static int    MbarGetEndTime(MBAR_REQ_STR *mr_pp);
static int    MbarGetStartTap(MBAR_REQ_STR *mr_pp);
static void   MbarOthSet(MBAR_REQ_STR *mr_pp);
static void   MbarCurSet(MBAR_REQ_STR *mr_pp);
static int    MbarTapSubt(MBAR_REQ_STR *mr_pp);
/*static*/ void   MbarPosOffsetSet(MBAR_REQ_STR *mr_pp);
static void   mbar_othon_frame_set(MBAR_REQ_STR *mr_pp);
static void   guidisp_init_pr(void);
static void   guidisp_draw_quit(int drapP);

void examCharSet(EX_CHAR_DISP *ecd_pp, sceGifPacket *gifpk_pp) {
    int wl, hl;
    int xp, yp;

    sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEXFLUSH, 0);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_PRMODECONT, 1);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_CLAMP_1, SCE_GS_SET_CLAMP_1(1, 1, 0, 0, 0, 0));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1(0, 1, 0, 1, 0));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEX0_1, ecd_pp->GsTex0);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEX1_1, ecd_pp->GsTex1);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_PABE, SCE_GS_SET_PABE(PR_REGS(ecd_pp).pabe));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_FBA_1, SCE_GS_SET_FBA(PR_REGS(ecd_pp).fba));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_COLCLAMP, 1);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(ecd_pp->kido[0], ecd_pp->kido[1], ecd_pp->kido[2], 128, 0x3f800000));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEST_1, SCE_GS_SET_TEST_1(1, 6, 0, 0, 0, 0, 1, 1));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEXA, SCE_GS_SET_TEXA(0, 1, 128));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_PRIM, SCE_GS_SET_PRIM(6, 0, 1, 0, ecd_pp->alpha, 0, 1, 0, 0));

    wl = ecd_pp->w * ecd_pp->scalex * 16.0f;
    hl = ecd_pp->h * ecd_pp->scaley * 16.0f;
    xp = ecd_pp->x - (int)(ecd_pp->cx * ecd_pp->scalex);
    yp = ecd_pp->y - (int)(ecd_pp->cy * ecd_pp->scaley);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_UV, SCE_GS_SET_UV(ecd_pp->u << 4, ecd_pp->v << 4));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_XYZ2, SCE_GS_SET_XYZ2(xp & 0xffff, yp & 0xffff, 1));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_UV, SCE_GS_SET_UV(((ecd_pp->u + ecd_pp->w) << 4) + 8, ((ecd_pp->v + ecd_pp->h) << 4) + 8));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_XYZ2, SCE_GS_SET_XYZ2((xp + wl) & 0xffff, (yp + hl) & 0xffff, 1));
}

static void clrColorBuffer(int id) {
    extern sceGsLoadImage tp_tmp_72; /* static */
    TIM2_DAT *tim2_dat_pp;
    u_char *tr_adr;
    u_int cpsm, cbp;

    tim2_dat_pp = &tim2spr_tbl_tmp1[id];
    tr_adr = (u_char*)&tmpColor;
    cpsm = PR_TEX0(tim2_dat_pp).CPSM;
    cbp = PR_TEX0(tim2_dat_pp).CBP;
    sceGsSetDefLoadImage(&tp_tmp_72, cbp, 1, cpsm, 0, 0, 8, 2);
    FlushCache(0);
    sceGsExecLoadImage(&tp_tmp_72, (u_long128*)tr_adr);
    sceGsSyncPath(0, 0);
}

void MbarMemberClear(int stg) {
    if (stg < 6) {
        clrColorBuffer(59);
    }
    if (stg < 4) {
        clrColorBuffer(58);
    }
    if (stg < 3) {
        clrColorBuffer(57);
    }
    if (stg < 2) {
        clrColorBuffer(56);
    }
    if (stg < 1) {
        clrColorBuffer(55);
    }
}

void examCharBasic(EX_CHAR_DISP *ecd_pp, TIM2_DAT *tim2_dat_pp) {
    ecd_pp->GsTex0 = tim2_dat_pp->GsTex0;
    ecd_pp->GsTex1 = tim2_dat_pp->GsTex1;
    ecd_pp->GsRegs = tim2_dat_pp->GsRegs;

    ecd_pp->w = tim2_dat_pp->w;
    ecd_pp->h = tim2_dat_pp->h;
    
    ecd_pp->u = 0;
    ecd_pp->v = 0;
    ecd_pp->cx = 0;
    ecd_pp->cy = 0;
    
    ecd_pp->scalex = 1.0f;
    ecd_pp->scaley = 1.0f;
    ecd_pp->alpha = 0.0f;

    ecd_pp->kido[0] = 128;
    ecd_pp->kido[1] = 128;
    ecd_pp->kido[2] = 128;
}

void examCharScaleSet(EX_CHAR_DISP *ecd_pp, float scx, float scy) {
    ecd_pp->scalex = ecd_pp->scalex * scx;
    ecd_pp->scaley = ecd_pp->scaley * scy;
}

void examCharCltSet(EX_CHAR_DISP *ecd_pp, TIM2_DAT *tim2_dat_pp) {
    ((sceGsTex0*)&ecd_pp->GsTex0)->CBP = ((sceGsTex0*)&tim2_dat_pp->GsTex0)->CBP;
}

void examCharPosSet(EX_CHAR_DISP *ecd_pp, int xp, int yp) {
    ecd_pp->x = (xp << 4) - GS_X_COORD(640);
    ecd_pp->y = (yp << 4) - GS_Y_COORD(224);
}

void examCharUVWHSet(EX_CHAR_DISP *ecd_pp, u_short u, u_short v, u_short w, u_short h) {
    ecd_pp->u = u;
    ecd_pp->v = v;
    ecd_pp->w = w;
    ecd_pp->h = h;
}

void examCharAlphaSet(EX_CHAR_DISP *ecd_pp, u_short on_off) {
    ecd_pp->alpha = on_off;
}

void examCharKidoSet(EX_CHAR_DISP *ecd_pp, u_char rc, u_char gc, u_char bc) {
    ecd_pp->kido[0] = rc;
    ecd_pp->kido[1] = gc;
    ecd_pp->kido[2] = bc;
}

static void NikoReset(void) {
    int i;

    if (niko_chan_str_pp != NULL) {
        for (i = 0; i < niko_chan_str_cnt; i++) {
            niko_chan_str_pp[i].niko_enum = NIKO_KAGE;
        }
    }
}

void MbarNikoHookUse(void) {
    niko_chan_str_cnt = 10;
    niko_chan_str_pp = niko_chan_str_hook;
    NikoReset();
}

void MbarNikoVsUse(void) {
    niko_chan_str_cnt = 6;
    niko_chan_str_pp = niko_chan_str_vs;
    NikoReset();
}

void MbarNikoUnUse(void) {
    niko_chan_str_pp = NULL;
    niko_chan_str_cnt = 0;
}

void MbarNikoSet(int num, int ofs) {
    int i;

    if (niko_chan_str_pp == NULL) {
        return;
    }

    for (i = 0; i < num / 2; i++) {
        if ((i + ofs) >= niko_chan_str_cnt) {
            printf("NIKO OVER!!\n");
            return;
        }
        niko_chan_str_pp[i + ofs].niko_enum = NIKO_MARU;
    }

    if (num & 0x1) {
        if (((num / 2) + ofs) >= niko_chan_str_cnt) {
            printf("NIKO OVER!!\n");
            return;
        }  
        niko_chan_str_pp[(num / 2) + ofs].niko_enum = NIKO_HALF;
    }
}

static void MbarNikoDisp(sceGifPacket *gifpk_pp) {
    int i;
    NIKO_CHAN_STR *niko_pp;
    TIM2_DAT *tim2_dat_pp;
    TIM2_DAT *tim2_dat2_pp;

    if (niko_chan_str_pp == NULL || niko_chan_str_cnt == 0) {
        return;
    }

    ChangeDrawAreaSetGifTag(DrawGetDrawEnvP(DNUM_DRAW), gifpk_pp);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEXFLUSH, 0);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEST_1, SCE_GS_SET_TEST_1(1, 6, 0, 0, 0, 0, 1, 1));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEXA, SCE_GS_SET_TEXA(0, 1, 128));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_CLAMP_1, SCE_GS_SET_CLAMP_1(1, 1, 0, 0, 0, 0));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_PABE, 0);
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEXA, SCE_GS_SET_TEXA(0, 1, 128));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1(0, 1, 0, 1, 0));

    niko_pp = niko_chan_str_pp;
    for (i = 0; i < niko_chan_str_cnt; i++, niko_pp++) {
        tim2_dat_pp = NULL;
        tim2_dat2_pp = NULL;

        switch (niko_chan_str_pp[i].niko_enum) {
        case NIKO_KAGE:
            tim2_dat_pp = &tim2spr_tbl_tmp1[60];
            break;
        case NIKO_HALF:
            tim2_dat_pp = &tim2spr_tbl_tmp1[60];
            tim2_dat2_pp = tim2_dat_pp + 1;
            break;
        case NIKO_MARU:
            tim2_dat_pp = &tim2spr_tbl_tmp1[61];
            break;
        case NIKO_SKIP:
            tim2_dat_pp = &tim2spr_tbl_tmp1[62];
            break;
        }

        if (tim2_dat_pp != NULL) {
            sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEX0_1, tim2_dat_pp->GsTex0);
            sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEX1_1, tim2_dat_pp->GsTex1);
            sceGifPkAddGsAD(gifpk_pp, SCE_GS_PRIM, SCE_GS_SET_PRIM(6, 0, 1, 0, 1, 0, 1, 0, 0));
            sceGifPkAddGsAD(gifpk_pp, SCE_GS_UV, SCE_GS_SET_UV(0, 0));
            sceGifPkAddGsAD(gifpk_pp, SCE_GS_XYZ2, SCE_GS_SET_XYZ2((niko_pp->xp + 0x6c0) << 4, (niko_pp->yp + 0x790) << 4, 1));
            sceGifPkAddGsAD(gifpk_pp, SCE_GS_UV, SCE_GS_SET_UV(tim2_dat_pp->w << 4, tim2_dat_pp->h << 4));
            sceGifPkAddGsAD(gifpk_pp, SCE_GS_XYZ2, SCE_GS_SET_XYZ2((niko_pp->xp + tim2_dat_pp->w + 0x6c0) << 4, (niko_pp->yp + (tim2_dat_pp->h / 2) + 0x790) << 4, 1));

            if (tim2_dat2_pp != NULL) {
                sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEX0_1, tim2_dat2_pp->GsTex0);
                sceGifPkAddGsAD(gifpk_pp, SCE_GS_TEX1_1, tim2_dat2_pp->GsTex1);
                sceGifPkAddGsAD(gifpk_pp, SCE_GS_PRIM, SCE_GS_SET_PRIM(6, 0, 1, 0, 1, 0, 1, 0, 0));
                sceGifPkAddGsAD(gifpk_pp, SCE_GS_UV, SCE_GS_SET_UV(0, 0));
                sceGifPkAddGsAD(gifpk_pp, SCE_GS_XYZ2, SCE_GS_SET_XYZ2((niko_pp->xp + 0x6c0) << 4, (niko_pp->yp + 0x790) << 4, 1));
                sceGifPkAddGsAD(gifpk_pp, SCE_GS_UV, SCE_GS_SET_UV(tim2_dat2_pp->w << 4, (tim2_dat2_pp->h / 2) << 4));
                sceGifPkAddGsAD(gifpk_pp, SCE_GS_XYZ2, SCE_GS_SET_XYZ2((niko_pp->xp + tim2_dat2_pp->w + 0x6c0) << 4, (niko_pp->yp + (tim2_dat2_pp->h / 4) + 0x790) << 4, 1));
            }
        }
    }
}

void MbarHookUseInit(void) {
    int i;

    hook_use_flag = TRUE;

    for (i = 0; i < PR_ARRAYSIZE(mbhook_str); i++) {
        mbhook_str[i].timer = 0;
        Tim2Trans(cmnfGetFileAdrs(mbhook_str[i].moto));
    }
}

void MbarHookUnUse(void) {
    hook_use_flag = FALSE;
}

void MbarHookUseOK(void) {
    mbhook_str[1].timer = 1;
}

void MbarHookUseNG(void) {
    mbhook_str[0].timer = 1;
}

static void MbarHookPoll(void) {
    int i;
    u_char *saki_pp;
    u_char *moto_pp;

    if (!hook_use_flag) {
        return;
    }

    for (i = 0; i < 2; i++) {
        if (mbhook_str[i].timer != 0) {
            mbhook_str[i].timer++;
            moto_pp = cmnfGetFileAdrs(mbhook_str[i].moto);
            saki_pp = cmnfGetFileAdrs(mbhook_str[i].saki);
            if (mbhook_str[i].timer < 0 || mbhook_str[i].timer > 15) {
                Tim2Trans(moto_pp);
                mbhook_str[i].timer = 0;
            } else {
                Cl2MixTrans(hook_fr_dat[mbhook_str[i].timer], 128, saki_pp, moto_pp);
            }
        }
    }
}

void vsAnimationInit(void) {
    int i;

    for (i = 0; i < 4; i++) {
        vsScoreMove[i] = 0;
        vsScoreAni[i] = 0;
    }

    WorkClear(vs_scr_ctrl, sizeof(vs_scr_ctrl));
}

void vsAnimationReq(int ply, long scrMoto, long scrSaki, VS_MV_TYPE vt) {
    VS_SCR_CTRL *vsc_pp;

    vsc_pp = &vs_scr_ctrl[ply];
    vsc_pp->motoScr = scrMoto;
    vsc_pp->sakiScr = scrSaki;
    vsc_pp->animation_time = 60;
    vsc_pp->vt = vt;
}

void vsAnimationReset(int ply, long scr) {
    VS_SCR_CTRL *vsc_pp;

    vsc_pp = &vs_scr_ctrl[ply];
    vsc_pp->motoScr = scr;
    vsc_pp->sakiScr = scr;
    vsc_pp->animation_time = 0;
    vsc_pp->vt = VSMT_NONE;
}

static int vsScr2Move(long scr) {
    if (scr > 500) {
        scr = 500;
    }

    return scr * 2;
}

static void vsAnimationPoll(void) {
    int i, score;

    for (i = 0; i < 4; i++) {
        if (vs_scr_ctrl[i].animation_time != 0) {
            vsScoreAni[i] = 60 - vs_scr_ctrl[i].animation_time;
            score = ((vs_scr_ctrl[i].sakiScr - vs_scr_ctrl[i].motoScr) * vsScoreAni[i]) / 60;
            score += vs_scr_ctrl[i].motoScr;
            vsScoreMove[i] = score;
            vsScoreMove[i] = vsScr2Move(vsScoreMove[i]);
            if (vs_scr_ctrl[i].vt == VSMT_UP) {
                vsScoreAni[i] += 62;
            }
            if (vs_scr_ctrl[i].vt == VSMT_DW) {
                vsScoreAni[i] += 2;
            }
            vs_scr_ctrl[i].animation_time--;
            if (vs_scr_ctrl[i].animation_time == 0) {
                vs_scr_ctrl[i].motoScr = vs_scr_ctrl[i].sakiScr;
            }
        } else {
            vsScoreAni[i] = 0;
            vsScoreMove[i] = vsScr2Move(vs_scr_ctrl[i].sakiScr);
        }
    }
}

static void metColorInit(void) {
    int i;

    for (i = 0; i < PR_ARRAYSIZE(metcol_str); i++) {
        Tim2Trans(cmnfGetFileAdrs(metcol_str[i].df_num));
    }
}

static void metColorSet(EXAM_TYPE exam_type, float per) {
    u_char *moto_pp;
    u_char *saki_pp;
    int sakiper;

    if (per == 0.0f) {
        return Tim2Trans(cmnfGetFileAdrs(metcol_str[exam_type].df_num));
    }
    if (per == -1.0f) {
        return Tim2Trans(cmnfGetFileAdrs(metcol_str[exam_type].ng_num));
    }
    if (per == 1.0f) {
        return Tim2Trans(cmnfGetFileAdrs(metcol_str[exam_type].ok_num));
    }
    sakiper = metcol_str[exam_type].df_num;
    moto_pp = cmnfGetFileAdrs(sakiper);
    if (per < 0.0f) {
        per = -per;
        if (per > 1.0f) {
            per = 1.0f;
        }
        saki_pp = cmnfGetFileAdrs(metcol_str[exam_type].ng_num);
        sakiper = per * 128;
    } else {
        if (per > 1.0f) {
            per = 1.0f;
        }
        saki_pp = cmnfGetFileAdrs(metcol_str[exam_type].ok_num);
        sakiper = per * 128;
    }
    Cl2MixTrans(sakiper, 128, saki_pp, moto_pp);
}

void metFrameInit(void) {
    int i;

    for (i = 0; i < 3; i++) {
        metFrameCnt[i] = 0x8c;
        metFrameCntLight[i] = 0;
    }

    scoreTentouFlag = 0;

    for (i = 0; i < 4; i++) {
        scr_tenmetu_col_dat[i][0] = scr_tenmetu_col[0][0];
        scr_tenmetu_col_dat[i][1] = scr_tenmetu_col[0][1];
        scr_tenmetu_col_dat[i][2] = scr_tenmetu_col[0][2];
        exam_global_ply_current_ply[i] = 0;
    }
}

void conditionFrameInit(void) {
    int i;

    for (i = 0; i < PR_ARRAYSIZE(conditionFramCnt); i++) {
        conditionFramCnt[i] = 60;
    }
}

void ExamDispInit(void) {
    int i;

    for (i = 0; i < PR_ARRAYSIZE(exam_global_ply); i++) {
        exam_global_ply[i] = NULL;
    }

    exam_disp_cursor_timer = -1;

    metFrameInit();

    metColorInit();

    conditionFrameInit();

    otehonAniCnt = 0;
    othon_frame = 0;

    ExamDispReset();

    vsAnimationInit();
}

void ExamDispPlySet(GLOBAL_PLY *ply, int pos) {
    exam_global_ply[pos] = ply;
}

void ExamDispReq(int ply, int plmi) {
    exam_disp_cursor_timer = 0;
    exam_global_ply_current = exam_global_ply[ply];
    if (plmi) {
        exam_global_ply_current_ply[ply] = 1;
    } else {
        exam_global_ply_current_ply[ply] = 0;
    }
}

void ExamDispReset(void) {
    exam_disp_cursor_timer = -1;
    metFrameInit();
    metColorInit();
}

float examScore2Level(long score) {
    float ret_lvl = score / 140.0f;

    if (ret_lvl > 0) {
        ret_lvl += 0.2;
    } else {
        ret_lvl -= 0.2;
    }

    if (ret_lvl > 1.0) {
        ret_lvl = 1.0f;
    }

    if (ret_lvl < -1.0) {
        ret_lvl = -1.0f;
    }

    return ret_lvl;
}

#ifndef NON_MATCHING
INCLUDE_ASM("asm/nonmatchings/main/mbar", ExamDispOn);
#else
static void ExamDispOn(void) {
    int met_time;
    int i;
    float maxfr;
    float lev_tmp;
    u_int perd;
    u_char *moto_p;

    if (exam_disp_cursor_timer < 0) {
        return metFrameInit();
    }
    exam_disp_cursor_timer += 1;
    if (exam_disp_cursor_timer >= 91) {
        exam_disp_cursor_timer = -1;
        metColorInit();
        for (i = 0; i < 3; i++) {
            exam_global_ply_current_ply[i] = 0;
        }
        met_time = 0;
    } else if (exam_disp_cursor_timer >= 61) {
        met_time = 90 - exam_disp_cursor_timer;
    } else {
        met_time = 30;
        if (met_time >= exam_disp_cursor_timer) {
            met_time = exam_disp_cursor_timer;
        }
    }
    maxfr = met_time / 30.0f;
    for (i = 0; i < 3; i++) {
        lev_tmp = examScore2Level(exam_global_ply_current->exam_score[i]);
        if (lev_tmp < 0.0f && lev_tmp < -maxfr) {
            lev_tmp = -maxfr;
        }
        if (0.0f < lev_tmp && maxfr < lev_tmp) {
            lev_tmp = maxfr;
        }
        if (exam_global_ply_current->exam_score[i] > 0) {
            metColorSet(i, maxfr);
        } else {
            metColorSet(i, -maxfr);
        }
        lev_tmp -= -1.0;
        lev_tmp = (lev_tmp / 2.0) * 280.0;
        metFrameCnt[i] = lev_tmp;
        if (exam_disp_cursor_timer < 30) {
            if (exam_global_ply_current->exam_score[i] > 0) {
                metFrameCntLight[i] = exam_disp_cursor_timer;
            } else {
                metFrameCntLight[i] = exam_disp_cursor_timer + 60;
            }
        } else {
            metFrameCntLight[i] = 0;
        }
    }
    for (i = 0; i < 3; i++) {
        if (exam_global_ply_current_ply[i] != 0) {
            if (exam_global_ply[i]->now_score > 0) {
                moto_p = scr_tenmetu_col[1];
            } else {
                moto_p = scr_tenmetu_col[2];
            }
            perd = exam_disp_cursor_timer * 16;
            if (perd & 0x100) {
                perd ^= 0xff;
            }
            perd &= 0xff;
            scr_tenmetu_col_dat[i][0] = ((moto_p[0] * perd) >> 9) + (moto_p[0] >> 1);
            scr_tenmetu_col_dat[i][1] = ((moto_p[1] * perd) >> 9) + (moto_p[1] >> 1);
            scr_tenmetu_col_dat[i][2] = ((moto_p[2] * perd) >> 9) + (moto_p[2] >> 1);
        }
    }
}
#endif

INCLUDE_ASM("asm/nonmatchings/main/mbar", hex2dec_mbar_tmp);

static u_long hex2decPlMi(long data) {
    u_long ret;
    int i;
    long plmichar;

    ret = 0;
    if (data == 0) {
        return ret;
    }
    plmichar = data < 1 ? 0 : 10;
    if (data < 0) {
        data *= -1;
        plmichar = 11;
    }
    i = 0;
    if (data == 0) {
        ret = plmichar;
    } else {
        while (data) {
            ret |= (data % 10) << (i * 4);
            data /= 10;
            if (++i >= 16u) {
                return ret;
            }
        }
        ret |= plmichar << (i * 4);
    }
    return ret;
}

void examNumDisp(sceGifPacket *ex_gif_pp, long score, short x, short y, int keta, u_char *coldat_pp, int plmi) {
    int i;
    u_char num;
    int first_f;
    EX_CHAR_DISP ex_ecd;

    first_f = FALSE;
    examCharBasic(&ex_ecd, &tim2spr_tbl_tmp1[27]);
    examCharKidoSet(&ex_ecd, coldat_pp[0], coldat_pp[1], coldat_pp[2]);
    
    if (plmi != 0) {
        score = hex2decPlMi(score);
    } else {
        score = hex2dec_mbar_tmp(score);
    }

    for (i = 0; i < keta; i++) {
        plmi = i + 1;
        num = (score >> ((keta - plmi) << 2)) & 0xf;

        if (num != 0 || first_f || i == (keta - 1)) {
            first_f = TRUE;
            examCharUVWHSet(&ex_ecd, num * 13, 0, 13, 24);
            examCharPosSet(&ex_ecd, x + (i * 15), y);
            examCharSet(&ex_ecd, ex_gif_pp);
        }
    }
}

#ifndef NON_MATCHING
INCLUDE_ASM("asm/nonmatchings/main/mbar", examScoreSet);
#else
/* Needs .rodata match */
static void examScoreSet(sceGifPacket *ex_gif_pp) {
    int i;
    int pos_dat[2][2] = {
        {530, 4},
        {530, 36},
    };

    for (i = 0; i < 2; i++) {
        if (exam_global_ply[i] == NULL) {
            continue;
        }
        if (exam_global_ply_current_ply[i]) {
            examNumDisp(ex_gif_pp, exam_global_ply[i]->now_score, pos_dat[i][0], pos_dat[i][1], 5, scr_tenmetu_col_dat[i], 1);
        } else {
            examNumDisp(ex_gif_pp, exam_global_ply[i]->score, pos_dat[i][0], pos_dat[i][1], 5, scr_tenmetu_col_dat[i], 0);
        }
    }
}
#endif

static void examLevelDisp(sceGifPacket *ex_gif_pp) {
    GLOBAL_PLY *exg_p;
    int old_fr, targ_fr;
    EX_CHAR_DISP ex_ecd;
    int plevel, i;

    for (i = 0; i < 4; i++) {
        exg_p = exam_global_ply[i];
        if (exg_p == NULL) {
            continue;
        }
        old_fr = conditionFramCnt[i];
        targ_fr = exg_p->rank_level * 20;
        if (old_fr < targ_fr) {
            old_fr += 2;
            if (targ_fr < old_fr) {
                old_fr = targ_fr;
            }
        } else {
            old_fr -= 2;
            if (old_fr < targ_fr) {
                old_fr = targ_fr;
            }
        }
        if (old_fr >= 241) {
            old_fr = 240;
        }
        conditionFramCnt[i] = old_fr;
    }
    plevel = conditionFramCnt[0] * 96 / 240;
    examCharBasic(&ex_ecd, &tim2spr_tbl_tmp1[28]);
    examCharUVWHSet(&ex_ecd, plevel, 0, 24, 88);
    examCharPosSet(&ex_ecd, 616, 136);
    examCharSet(&ex_ecd, ex_gif_pp);
}

void ExamDispSet(void) {
    sceGifPacket ex_gif;

    ExamDispOn();
    CmnGifOpenCmnPk(&ex_gif);
    examScoreSet(&ex_gif);
    examLevelDisp(&ex_gif);
    vsAnimationPoll();
    CmnGifCloseCmnPk(&ex_gif, 6);
}

void ExamDispSubt(void) {
    /* Empty */
}

void MbarInit(int stg) {
    mbar_ctrl_stage = stg;

    if (stg == 6) {
        mbar_ctrl_stage_selT = 1;
    } else {
        mbar_ctrl_stage_selT = stg;
    }
    
    MbarCharSetSub();
}

void MbarReset(void) {
    WorkClear(mbar_req_str, sizeof(mbar_req_str));
}

INCLUDE_RODATA("asm/nonmatchings/main/mbar", D_00393450);

INCLUDE_RODATA("asm/nonmatchings/main/mbar", D_00393458);

void MbarReq(MBAR_REQ_ENUM mm_req, TAPSET *ts_pp, int curr_time, SCR_TAP_MEMORY *tm_pp, int tm_cnt, 
             int lang, int tapdat_size, TAPDAT *tapdat_pp, GUI_CURSOR_ENUM guic) {
    PLAYER_INDEX pidx;

    if (ts_pp == NULL) {
        printf("MbarReq   TAPSET adrs is NULL\n");
        return;
    }

    pidx = Pcode2Pindex(ts_pp->player_code);
    mbar_req_str[pidx].mbar_req_enum = mm_req;
    mbar_req_str[pidx].tapset_pp = ts_pp;
    mbar_req_str[pidx].current_time = curr_time;
    mbar_req_str[pidx].scr_tap_memory_pp = tm_pp;
    mbar_req_str[pidx].scr_tap_memory_cnt = tm_cnt;
    mbar_req_str[pidx].lang = lang;

    mbar_req_str[pidx].tapdat_size = tapdat_size;
    mbar_req_str[pidx].tapdat_pp = tapdat_pp;
    mbar_req_str[pidx].gui_cursor_enum = guic;
}

void MbarSetCtrlTime(int mctime) {
    mbar_ctrl_time = mctime;
}

#ifndef NON_MATCHING
INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarCl1CharSet);
#else
static void MbarCl1CharSet(/* a0 4 */ int col_num, /* a1 5 */ int moto_num) {
    /* a0 4 */ MBA_CHAR_DATA *mbcd_col;
    /* a1 5 */ MBA_CHAR_DATA *mbcd_mot;
    /* v0 2 */ sceGsTex0 ColGsTex0;

    mbcd_col = &mba_char_data[col_num];
    mbcd_mot = &mba_char_data[moto_num];
    
    ColGsTex0 = *(sceGsTex0*)&mbcd_col->tim2_dat_pp->GsTex0;
    *mbcd_col->tim2_dat_pp = *mbcd_mot->tim2_dat_pp;

    PR_TEX0(mbcd_col->tim2_dat_pp).CBP = ColGsTex0.CBP;
}
#endif

static void MbarCharSetSub(void) {
    int i;

    for (i = 0; i < 7; i++) {
        MbarCl1CharSet(i + 15, i + 1);
    }
}

void MbarGifInit(void) {
    CmnGifADPacketMake(&mbar_gif, 0);
    sceGifPkAddGsAD(&mbar_gif, SCE_GS_TEXFLUSH, 0);
    sceGifPkAddGsAD(&mbar_gif, SCE_GS_TEST_1, SCE_GS_SET_TEST_1(1, 6, 0, 0, 0, 0, 1, 1));
    sceGifPkAddGsAD(&mbar_gif, SCE_GS_TEXA, SCE_GS_SET_TEXA(0, 1, 128));
    sceGifPkAddGsAD(&mbar_gif, SCE_GS_CLAMP_1, SCE_GS_SET_CLAMP_1(1, 1, 0, 0, 0, 0));
    sceGifPkAddGsAD(&mbar_gif, SCE_GS_PABE, 0);    
}

void MbarGifTrans(int pri) {
    CmnGifCloseCmnPk(&mbar_gif, pri);
}

void MbarCharSet(MBARR_CHR *mb_pp) {
    MBA_CHAR_DATA *mbcd_pp;
    float w, h;
    int x1, y1, x2, y2;

    mbcd_pp = &mba_char_data[mb_pp->mbc_enum];
    if (mbcd_pp->tim2_dat_pp == NULL) {
        return;
    }
    sceGifPkAddGsAD(&mbar_gif, SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(mb_pp->r, mb_pp->g, mb_pp->b, mb_pp->a, 0x3f800000));
    sceGifPkAddGsAD(&mbar_gif, SCE_GS_TEX0_1, mbcd_pp->tim2_dat_pp->GsTex0);
    sceGifPkAddGsAD(&mbar_gif, SCE_GS_TEX1_1, mbcd_pp->tim2_dat_pp->GsTex1);
    sceGifPkAddGsAD(&mbar_gif, SCE_GS_PRIM, SCE_GS_SET_PRIM(6, 0, 1, 0, 0, 0, 1, 0, 0));
    w = mbcd_pp->sclx * mb_pp->sclx * mbcd_pp->tim2_dat_pp->w;
    h = mbcd_pp->scly * mb_pp->scly * mbcd_pp->tim2_dat_pp->h;
    sceGifPkAddGsAD(&mbar_gif, SCE_GS_UV, SCE_GS_SET_UV(0, 0));
    w *= 0.5f;
    h *= 0.5f;
    x1 = (mb_pp->xp - w) * 16.0f;
    y1 = (mb_pp->yp - h) * 16.0f;
    sceGifPkAddGsAD(&mbar_gif, SCE_GS_XYZ2, SCE_GS_SET_XYZ2(0x6C00 + x1, 0x7900 + y1, 1));
    sceGifPkAddGsAD(&mbar_gif, SCE_GS_UV, SCE_GS_SET_UV(mbcd_pp->tim2_dat_pp->w << 4, mbcd_pp->tim2_dat_pp->h << 4));
    x2 = (mb_pp->xp + w) * 16.0f;
    y2 = (mb_pp->yp + h) * 16.0f;
    sceGifPkAddGsAD(&mbar_gif, SCE_GS_XYZ2, SCE_GS_SET_XYZ2(0x6C00 + x2, 0x7900 + y2, 1));
}

void MbarCharSet2(MBARR_CHR2 *mb_pp) {
    MBA_CHAR_DATA *mbcd_pp;
    int x1, y1, x2, y2;
    
    mbcd_pp = &mba_char_data[mb_pp->mbc_enum];
    if (mbcd_pp->tim2_dat_pp == NULL) {
        return;
    }
    sceGifPkAddGsAD(&mbar_gif, SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(mb_pp->r, mb_pp->g, mb_pp->b, mb_pp->a, 0x3f800000));
    sceGifPkAddGsAD(&mbar_gif, SCE_GS_TEX0_1, mbcd_pp->tim2_dat_pp->GsTex0);
    sceGifPkAddGsAD(&mbar_gif, SCE_GS_TEX1_1, mbcd_pp->tim2_dat_pp->GsTex1);
    sceGifPkAddGsAD(&mbar_gif, SCE_GS_PRIM, SCE_GS_SET_PRIM(6, 0, 1, 0, 0, 0, 1, 0, 0));
    sceGifPkAddGsAD(&mbar_gif, SCE_GS_UV, SCE_GS_SET_UV(0, 0));
    x1 = mb_pp->xp + mb_pp->ofsx;
    y1 = mb_pp->yp + mb_pp->ofsy;
    sceGifPkAddGsAD(&mbar_gif, SCE_GS_XYZ2, SCE_GS_SET_XYZ2(GS_X_COORD(x1), GS_Y_COORD(y1), 1));
    sceGifPkAddGsAD(&mbar_gif, SCE_GS_UV, SCE_GS_SET_UV(mbcd_pp->tim2_dat_pp->w << 4, mbcd_pp->tim2_dat_pp->h << 4));
    x2 = mb_pp->xp2 + mb_pp->ofsx2;
    y2 = mb_pp->yp2 + mb_pp->ofsy2;
    sceGifPkAddGsAD(&mbar_gif, SCE_GS_XYZ2, SCE_GS_SET_XYZ2(GS_X_COORD(x2), GS_Y_COORD(y2), 1));
}

void MbarWindowSet(MBWINDOW_ENUM wenum) {
    switch (wenum) {
    case MBWINDOW_NORMAL:
        sceGifPkAddGsAD(&mbar_gif, SCE_GS_SCISSOR_1, SCE_GS_SET_SCISSOR(0, 639, 0, 223));
        break;
    case MBWINDOW_UP:
        sceGifPkAddGsAD(&mbar_gif, SCE_GS_SCISSOR_1, SCE_GS_SET_SCISSOR(0, 525, 0, 111));
        break;
    case MBWINDOW_DOWN:
        sceGifPkAddGsAD(&mbar_gif, SCE_GS_SCISSOR_1, SCE_GS_SET_SCISSOR(0, 525, 112, 223));
        break;
    }
}

static int MbarGetDispPosX(int tick) {
    int pos;

    if (tick < 0) {
        return 13;
    }
    if (tick < 480) {
        pos = (tick * 25 / 24);
    } else {
        pos = ((tick - 384) * 25 / 24);
    }
    return pos + 13;
}

static int MbarGetDispPosY(int tick) {
    int v0, pos;

    pos = 0x1df - 1;

    if (tick < 0) {
        return mbar_pos_y_ofs + 0x17;
    }
    
    pos = (pos + 1) < tick;
    pos *= 0x19;
    v0 = mbar_pos_y_ofs + 0x17;
    return pos + v0;
}

static int MbarGetTimeArea(MBAR_REQ_STR *mr_pp) {
    int ret;

    ret = 0;
    if (mr_pp->mbar_req_enum == MBAR_NONE) {
        return ret;
    }
    if (mbar_ctrl_time < (mr_pp->current_time + mr_pp->tapset_pp->taptimeStart)) {
        return ret;
    }
    if (mbar_ctrl_time >= (mr_pp->current_time + mr_pp->tapset_pp->taptimeEnd)) {
        return ret;
    }
    if (mr_pp->mbar_req_enum & 0x33) {
        ret = mbar_ctrl_time - mr_pp->current_time - mr_pp->tapset_pp->taptimeStart;
    }
    if (mr_pp->mbar_req_enum & 0xc) {
        ret = mr_pp->tapset_pp->taptimeEnd - mr_pp->tapset_pp->taptimeStart;
    }
    return ret;
}

static int MbarGetTimeArea2(MBAR_REQ_STR *mr_pp) {
    int ret;

    ret = 0;
    if (mr_pp->mbar_req_enum == 0) {
        return ret;
    }
    if (mbar_ctrl_time >= (mr_pp->current_time + mr_pp->tapset_pp->taptimeEnd)) {
        return ret;
    }
    if (mr_pp->mbar_req_enum & 0x33) {
        ret = mbar_ctrl_time - mr_pp->current_time - mr_pp->tapset_pp->taptimeStart;
    }
    if (mr_pp->mbar_req_enum & 0xc) {
        ret = mr_pp->tapset_pp->taptimeEnd - mr_pp->tapset_pp->taptimeStart;
    }
    return ret;
}

int MbarGetStartTime(MBAR_REQ_STR *mr_pp) {
    return ((mr_pp->current_time + mr_pp->tapset_pp->taptimeStart - 24) / 96) * 96;
}

int MbarGetEndTime(MBAR_REQ_STR *mr_pp) {
    return (mr_pp->current_time + mr_pp->tapset_pp->taptimeEnd);
}

static int MbarGetStartTap(MBAR_REQ_STR *mr_pp) {
    int ret;

    ret = mr_pp->current_time + mr_pp->tapset_pp->taptimeStart;
    ret = (ret / 0x18 - 1) * 0x18;
    return -1 < ret ? ret : 0;
}

#ifndef NON_MATCHING
INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarSclRotMake);
#else
/* Needs .rodata match */
void MbarSclRotMake(MBARR_CHR *mbarr_pp, int mbtime) {
    float tmp_rate;

    mbarr_pp->sclx = 1.0f;
    mbarr_pp->scly = 1.0f;
    if (mbtime >= 96u) {
        return;
    }
    if (mbtime < 24) {
        tmp_rate = (24 - mbtime) / 24.0f + 1.0f;
        mbarr_pp->sclx = tmp_rate;
        mbarr_pp->scly = tmp_rate;
    }
    if (mbtime < 96) {
        tmp_rate = cosf(mbtime * 0.12345f / 96.0f);
        mbarr_pp->sclx *= tmp_rate;
    }
}
#endif

void MbarGuideLightMake(MBARR_CHR *mbarr_pp, int mbtime) {
    u_char col = 128;

    if (mbtime >= 0 && mbtime < 144) {
        col = (144 - mbtime) * 128 / 144 + 128u;
    }

    mbarr_pp->r = mbarr_pp->g = mbarr_pp->b = col;
}

int MbarFlashMake(MBARR_CHR *mbarr_pp, MBARR_CHR *mbarr_moto_pp, int mbtime, int fltype) {
    float fsize;
    u_char *colpp;
    u_char scale;

    if (mbtime < 0) {
        return 0;
    }
    if (mbtime >= 24) {
        return 0;
    }

    mbarr_pp->xp = mbarr_moto_pp->xp;
    mbarr_pp->yp = mbarr_moto_pp->yp + 100;
    
    fsize = 24 - mbtime;
    fsize += fsize;
    fsize /= 24.0f;
    fsize += 1.0f;
    mbarr_pp->scly = fsize;
    mbarr_pp->sclx = fsize;
    
    colpp = colp[fltype];
    scale = (mbtime * 128) / 24;
    mbarr_pp->r = (colpp[0] * scale) / 128;
    mbarr_pp->g = (colpp[1] * scale) / 128;
    mbarr_pp->b = (colpp[2] * scale) / 128;

    return 1;    
}

#ifndef NON_MATCHING
INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarBackSet);
#else
/* Needs .rodata match */
void MbarBackSet(MBAR_REQ_STR *mr_pp) {
    int i, stt, endt, sttap, curtime;
    MBARR_CHR mbarr;
    MBARR_CHR2 mbarr_chr2;
    int sttime, endtime;

    mbarr = (MBARR_CHR) {
        .mbc_enum = MBC_BALL,
        .xp = 13,
        .yp = 23,
        .sclx = 1.0f,
        .scly = 1.0f,
        .r = 128,
        .g = 128,
        .b = 128,
        .a = 128,
    };

    MbarWindowSet(MBWINDOW_UP);
    if (MbarGetTimeArea2(mr_pp) == 0) {
        return;
    }
    curtime = mbar_ctrl_time;
    stt = MbarGetStartTime(mr_pp);
    endt = MbarGetEndTime(mr_pp);
    sttap = MbarGetStartTap(mr_pp);
    if (mr_pp->mbar_req_enum & 0x200) {
        mbarr_chr2.mbc_enum = 0x24;
    } else {
        mbarr_chr2.mbc_enum = 0x25;
    }
    endtime = endt - stt - 1;
    if (curtime < stt) {
        sttime = 0;
    } else {
        sttime = curtime - stt;
    }

    mbarr_chr2.b = 0x80;
    mbarr_chr2.g = 0x80;
    mbarr_chr2.r = 0x80;
    mbarr_chr2.a = 0x20;
    mbarr_chr2.ofsx2 = 0;
    mbarr_chr2.ofsx = 0;
    mbarr_chr2.ofsy = -0xe;
    mbarr_chr2.ofsy2 = 0xe;
    
    if (sttime < endtime) {
        mbarr_chr2.xp = MbarGetDispPosX(sttime);
        mbarr_chr2.yp = MbarGetDispPosY(sttime);
        mbarr_chr2.xp2 = MbarGetDispPosX(endtime);
        mbarr_chr2.yp2 = MbarGetDispPosY(endtime);
        if (mr_pp->mbar_req_enum & 0x40) {
            mbarr_chr2.yp2 += 0x32;
            mbarr_chr2.yp += 0x32;
        } else if (mr_pp->mbar_req_enum & 0x800) {
            mbarr_chr2.yp2 += 0x19;
            mbarr_chr2.yp += 0x19;
        }
        if (mbarr_chr2.yp != mbarr_chr2.yp2) {
            mbarr_chr2.xp2 = MbarGetDispPosX(0x1df);
            mbarr_chr2.yp2 = MbarGetDispPosY(0x1df);
            if (mr_pp->mbar_req_enum & 0x40) {
                mbarr_chr2.yp2 += 0x32;
            } else if (mr_pp->mbar_req_enum & 0x800) {
                mbarr_chr2.yp2 += 0x19;
            }
            MbarCharSet2(&mbarr_chr2);
            mbarr_chr2.xp = MbarGetDispPosX(0x1e0);
            mbarr_chr2.yp = MbarGetDispPosY(0x1e0);
            mbarr_chr2.xp2 = MbarGetDispPosX(endtime);
            mbarr_chr2.yp2 = MbarGetDispPosY(endtime);
            if (mr_pp->mbar_req_enum & 0x40) {
                mbarr_chr2.yp2 += 0x32;
                mbarr_chr2.yp += 0x32;
            } else if (mr_pp->mbar_req_enum & 0x800) {
                mbarr_chr2.yp2 += 0x19;
                mbarr_chr2.yp += 0x19;
            }
        }
        MbarCharSet2(&mbarr_chr2);
    }
    for (endtime = stt + 0x18; endtime < endt; endtime += 0x18) {
        if ((endtime - 0x18) >= sttap) {
            sttime = endtime - stt;
            mbarr.xp = MbarGetDispPosX(sttime);
            mbarr.yp = MbarGetDispPosY(sttime);
            mbarr.mbc_enum = 0x16;
            if (((endtime / 0x18) & 0x3) == 0) {
                mbarr.mbc_enum = 0x17;
            }
            if (mr_pp->mbar_req_enum & 0x40) {
                mbarr.yp += 0x32;
            } else if (mr_pp->mbar_req_enum & 0x800) {
                mbarr.yp += 0x19;
            }
            if (mr_pp->mbar_req_enum & 0x80) {
                MbarGuideLightMake(&mbarr, curtime - endtime);
            }
            MbarCharSet(&mbarr);
        }
    }
}
#endif

INCLUDE_RODATA("asm/nonmatchings/main/mbar", D_00393488);

INCLUDE_RODATA("asm/nonmatchings/main/mbar", D_003934A0);

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarOthSet);

#ifndef NON_MATCHING
INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarCurSet);
#else
/* Needs .rodata match */
static void MbarCurSet(MBAR_REQ_STR *mr_pp) {
    MBARR_CHR mbarr;
    int curtime, gbalTapTime, sttime;

    mbarr = (MBARR_CHR) {
        .mbc_enum = 0,
        .xp = 0,
        .yp = 0,
        .sclx = 1.0f,
        .scly = 1.0f,
        .r = 128,
        .g = 128,
        .b = 128,
        .a = 128,
    }; 
    
    gbalTapTime = mr_pp->current_time;
    gbalTapTime += mr_pp->tapset_pp->taptimeStart;
    sttime = MbarGetStartTime(mr_pp);
    
    curtime = mbar_ctrl_time;
    curtime -= mr_pp->current_time;
    curtime -= mr_pp->tapset_pp->taptimeStart;

    gbalTapTime += curtime;
    gbalTapTime -= sttime;

    mbarr.mbc_enum = mr_pp->gui_cursor_enum + MBC_NONECUR;
    mbarr.xp = MbarGetDispPosX(gbalTapTime);
    mbarr.yp = MbarGetDispPosY(gbalTapTime);
    if (mr_pp->mbar_req_enum & 0x40) {
        mbarr.yp += 0x32;
    } else if (mr_pp->mbar_req_enum & 0x800) {
        mbarr.yp += 0x19;
    }
    MbarWindowSet(MBWINDOW_UP);
    MbarCharSet(&mbarr);
    mbarr.yp += 0x64;
    mbarr.a = 1;
    mbarr.b = 1;
    mbarr.g = 1;
    mbarr.r = 1;
    MbarWindowSet(MBWINDOW_DOWN);
    MbarCharSet(&mbarr);
}
#endif

static int MbarTapSubt(MBAR_REQ_STR *mr_pp) {
    int lng, nowp;

    if (MbarGetTimeArea(mr_pp) == 0) {
        return 0;
    }
    if (game_status.subtitle != SUBTITLE_ON) {
        return 1;
    }
    lng = mr_pp->tapset_pp->taptimeEnd - mr_pp->tapset_pp->taptimeStart;
    nowp = mbar_ctrl_time - mr_pp->current_time - mr_pp->tapset_pp->taptimeStart;
    SubtTapPrintWake(mr_pp->tapset_pp->tapsubt[mr_pp->lang], mr_pp->lang, lng, nowp);
    return 1;
}

/*static*/ void MbarPosOffsetSet(MBAR_REQ_STR *mr_pp) {
    mbar_pos_y_ofs = 0;
    if (MbarGetTimeArea2(mr_pp) == 0) {
        return;
    }
    if ((MbarGetEndTime(mr_pp) - MbarGetStartTime(mr_pp)) < 0x1e1) {
        mbar_pos_y_ofs = 12;
    }
}

void mbar_othon_frame_set(MBAR_REQ_STR* mr_pp) {
    int curtime;
    int mp_st;
    int mp_end;
    int mp_fix;

    curtime = mbar_ctrl_time - mr_pp->current_time - mr_pp->tapset_pp->taptimeStart;
    curtime += 0x30;
    if (curtime < 0) {
        return;
    }

    if (curtime >= 0x49) {
        return;
    }

    if (mr_pp->mbar_req_enum & 0x40) {
        mp_st = 0x32;
        mp_end = 0x5A;
        mp_fix = 0x5A;
    } else {
        mp_st = 0x5A;
        mp_end = 0x82;
        mp_fix = 0x32;
    }
    if (curtime < 0x18) {
        mp_fix = (((mp_end - mp_st) * curtime) / 24) + mp_st;
    }
    othon_frame = mp_fix;
}

void MbarDisp(void) {
    int i, j;

    MbarGifInit();
    for (i = 0; i < 4u; i++) {
        for (j = 0; j < 5u; j++) {
            if (mbar_req_str[j].mbar_req_enum == MBAR_NONE) {
                continue;
            }
            if (mbar_req_str[j].tapset_pp == NULL) {
                continue;
            }
            if (mbar_req_str[j].tapset_pp->coolup == -1) {
                continue;
            }
            (*marSetPrgTbl[i])(&mbar_req_str[j]);
        }
    }
    MbarGifTrans(7);
    if (game_status.subtitle == SUBTITLE_ON) {
        for (i = 0; i < 5u; i++) {
            if (mbar_req_str[i].mbar_req_enum == MBAR_NONE) {
                continue;
            }
            if (mbar_req_str[i].tapset_pp == NULL) {
                continue;
            }
            if (MbarTapSubt(&mbar_req_str[i]) != 0) {
                break;
            }
        }
    }
}

int MbarDispScene(void *para_pp, int frame, int first_f, int useDisp, int drDisp) {
    int i, j;
    float men_tmp;

    men_tmp = PrGetMendererRatio();
    PrSetMendererRatio(0.0f);

    PR_SCOPE
    VCLR_PARA vclr_para = {};
    DrawVramClear(&vclr_para, 0, 0, DNUM_NON, DNUM_VRAM2);
    PR_SCOPEEND

    ChangeDrawArea(DrawGetDrawEnvP(drDisp));
    MbarGifInit();

    for (i = 0; i < 5u; i++) {
        if (mbar_req_str[i].mbar_req_enum == MBAR_NONE) {
            continue;
        }
        if (mbar_req_str[i].tapset_pp == NULL) {
            continue;
        }
        if (mbar_req_str[i].tapset_pp->coolup == -1) {
            continue;
        }
        for (j = 0; j < 4u; j++) {
            (marSetPrgTbl[j])(&mbar_req_str[i]);
        }
    }

    MbarWindowSet(MBWINDOW_NORMAL);
    for (i = 0; i < 5u; i++) {
        if (mbar_req_str[i].mbar_req_enum == MBAR_NONE) {
            continue;
        }
        if (mbar_req_str[i].tapset_pp == NULL) {
            continue;
        }
        if (mbar_req_str[i].tapset_pp->coolup == -1) {
            continue;
        }
        mbar_othon_frame_set(&mbar_req_str[i]);
    }

    ExamDispOn();
    examScoreSet(&mbar_gif);
    examLevelDisp(&mbar_gif);
    vsAnimationPoll();
    MbarHookPoll();
    CmnGifADPacketMakeTrans(&mbar_gif);
    PrSetMendererRatio(men_tmp);

    if (game_status.subtitle == SUBTITLE_ON) {
        for (i = 0; i < 5u; i++) {
            if (mbar_req_str[i].mbar_req_enum == MBAR_NONE) {
                continue;
            }
            if (mbar_req_str[i].tapset_pp == NULL) {
                continue;
            }
            if (MbarTapSubt(&mbar_req_str[i]) != 0) {
                break;
            }
        }
    }

    return 0;
}

int MbarDispSceneDraw(void *para_pp, int frame, int first_f, int useDisp, int drDisp) {
    int i, j;
    float men_tmp;

    if (first_f == -2) {
        return 0;
    }
    if (first_f == -1) {
        return 0;
    }
    men_tmp = PrGetMendererRatio();
    PrSetMendererRatio(0.0f);

    PR_SCOPE
    VCLR_PARA vclr_para = {};
    DrawVramClear(&vclr_para, 0, 0, DNUM_NON, DNUM_VRAM2);
    PR_SCOPEEND

    ChangeDrawArea(DrawGetDrawEnvP(drDisp));
    MbarGifInit();

    for (i = 0; i < 5u; i++) {
        if (mbar_req_str[i].mbar_req_enum == MBAR_NONE) {
            continue;
        }
        if (mbar_req_str[i].tapset_pp == NULL) {
            continue;
        }
        if (mbar_req_str[i].tapset_pp->coolup == -1) {
            continue;
        }
        if (global_data.play_step == PSTEP_VS && i != 3) {
            continue;
        }
        for (j = 0; j < 4u; j++) {
            (marSetPrgTbl[j])(&mbar_req_str[i]);
        }
    }

    MbarWindowSet(MBWINDOW_NORMAL);
    CmnGifADPacketMakeTrans(&mbar_gif);
    PrSetMendererRatio(men_tmp);
    return 0;
}

int MbarDispSceneVsDraw(void *para_pp, int frame, int first_f, int useDisp, int drDisp) {
    if (first_f == -2) {
        return 0;
    }
    if (first_f == -1) {
        return 0;
    }
    vs_mouse_disp_flag = 1;
    return 0;
}

void MbarDispSceneVsDrawInit(void) {
    vs_mouse_disp_flag = 0;
}

#ifndef NON_MATCHING
INCLUDE_ASM("asm/nonmatchings/main/mbar", guidisp_init_pr);
#else
static void guidisp_init_pr(void) {
    GUIMAP *guim_pp;
    int i;

    PrSetFrameRate(60.0f);
    guime_hdl = PrInitializeScene(&DBufDc.draw01, "gui", 0xFFFFFFFF);
    guime_camera_hdl = PrInitializeCamera(cmnfGetFileAdrs(72));
    PrSelectCamera(guime_camera_hdl, guime_hdl);
    PrAnimateSceneCamera(guime_hdl, 0.0f);
    for (i = 0; i < 10u; i++) {
        guim_pp = &guimap[i];
        guim_pp->spmHdl = PrInitializeModel(cmnfGetFileAdrs(guim_pp->spmmap), guime_hdl);
        if (guim_pp->spamap >= 0) {
            guim_pp->spaHdl = PrInitializeAnimation(cmnfGetFileAdrs(guim_pp->spamap));
            PrLinkAnimation(guim_pp->spmHdl, guim_pp->spaHdl);
            PrAnimateModel(guim_pp->spmHdl, *guim_pp->frame_pp);
        }
        if (guim_pp->spamapP >= 0) {
            guim_pp->spaHdlP = PrInitializeAnimation(cmnfGetFileAdrs(guim_pp->spamapP));
            PrLinkPositionAnimation(guim_pp->spmHdl, guim_pp->spaHdlP);
            PrAnimateModelPosition(guim_pp->spmHdl, *guim_pp->frame_ppP);
        }
    }
    PrPreprocessSceneModel(guime_hdl);
}
#endif

INCLUDE_ASM("asm/nonmatchings/main/mbar", guidisp_draw_quit);

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarDispGuiScene);

INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarDispGuiSceneMbarArea);

TIM2_DAT* lessonTim2InfoGet(void) {
    return &tim2spr_tbl_tmp1[52];
}

#ifndef NON_MATCHING
INCLUDE_ASM("asm/nonmatchings/main/mbar", lessonCl2InfoGet);
#else /* requires .rodata migration */
TIM2_DAT* lessonCl2InfoGet(SCRRJ_LESSON_ROUND_ENUM type) {
    u_short le_num[10] = {
        0x2d, /* SCRRJ_LR_LESSON_1 */
        0x2e, /* SCRRJ_LR_LESSON_2 */
        0x2f, /* SCRRJ_LR_LESSON_3 */
        0x30, /* SCRRJ_LR_LESSON_4 */
        0x31, /* SCRRJ_LR_LESSON_5 */

        0x2d, /* SCRRJ_LR_ROUND_1 */
        0x2e, /* SCRRJ_LR_ROUND_2 */
        0x2f, /* SCRRJ_LR_ROUND_3 */
        0x30, /* SCRRJ_LR_ROUND_4 */
        0x31, /* SCRRJ_LR_ROUND_5 */
    };

    return &tim2spr_tbl_tmp1[le_num[type]];
}
#endif

#ifndef NON_MATCHING
INCLUDE_ASM("asm/nonmatchings/main/mbar", MbarDemoCharDisp);
#else
/* Needs .rodata match */
void MbarDemoCharDisp(void) {
    TIM2_DAT *tim2_dat_pp;
    SPR_PRIM spr_prim;

    spr_prim = (SPR_PRIM) {
        .x = 2298,
        .y = 2128,
        .scalex = 256,
        .scaley = 128,
        .u = 0,
        .v = 0,
        .w = 0,
        .h = 0,
    };

    SprInit();
    ChangeDrawArea(DrawGetDrawEnvP(2));
    tim2_dat_pp = &tim2spr_tbl_tmp1[53];
    spr_prim.w = tim2_dat_pp->w;
    spr_prim.h = tim2_dat_pp->h;
    SprClear();
    SprDispAlphaSet();
    SprPackSet(tim2_dat_pp);
    SprDispZABnclr();
    SprSetColor(128, 128, 128, 128);
    SprDispAlp(&spr_prim);
    SprFlash();
}
#endif

INCLUDE_RODATA("asm/nonmatchings/main/mbar", D_003934E8);

INCLUDE_RODATA("asm/nonmatchings/main/mbar", D_00393500);
