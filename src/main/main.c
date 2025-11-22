#include "main/main.h"

#include "dbug/dbgmsg.h"
#include "dbug/dbug.h"

#include "os/cmngifpk.h"
#include "os/mtc.h"
#include "os/syssub.h"
#include "os/system.h"
#include "os/tim2.h"
#include "os/usrmem.h"

#include "main/cdctrl.h"
#include "main/cmnfile.h"
#include "main/etc.h"
#include "main/fadectrl.h"
#include "main/p3str.h"
#include "main/scrctrl.h"
#include "main/stdat.h"
#include "main/wipe.h"

#include "menu/menu.h"

#include "iop_mdl/tapctrl_rpc.h"

#include <libpad.h>

#include <math.h>
#include <stdio.h>

/* .sdata */
extern int urawaza_skip_bottun;
extern int urawaza_levelsel_bottun;

/* data */
extern DBG_SELECT_STR dbg_select_str; /* = {
    .debug_on      = 0,
    .use_line      = 0xFFFFFFFF,
    .score_updown  = 0,
    .non_play      = 0,
}; */

extern u_char *dbg_tbl_msg[]; /* = {
    "AUTO", "BASE",
    "LV1",
    "LV2",
    "LV3",
    "LV4",
    "LV5",
    "LV6",
    "LV7", 
    "LV8", 
    "LV9", 
    "LV10",
    "LV11", 
    "LV12", 
    "LV13", 
    "LV14", 
    "LV15", 
    "LV16",
}; */

extern u_char *dbg_score_msg[]; /* = {
    "OFF", "ON"
}; */

extern DBG_MODE_STR dbg_mode_str[4/*0*/]; /* = {
    {
        .msg_pp = "START",
        .set_pp = NULL,
        .min = 0,
        .max = 0,
        .selmsg_pp = NULL,
    },
    {
        .msg_pp = "TABLE",
        .set_pp = &dbg_select_str.use_line,
        .min = 0xFFFFFFFF,
        .max = 17,
        .selmsg_pp = dbg_tbl_msg,
    },
    {
        .msg_pp = "SCORE DBUG",
        .set_pp = &dbg_select_str.score_updown,
        .min = 0,
        .max = 2,
        .selmsg_pp = dbg_score_msg,
    },
    {
        .msg_pp = "NON PLAY",
        .set_pp = &dbg_select_str.non_play,
        .min = 0,
        .max = 2,
        .selmsg_pp = dbg_score_msg,
    },
}; */

extern int overlay_loadaddr;
extern int uramen_end_flag;

/* ,bss */
extern MENU_STR menu_str;

extern int first_f; /* mainStart */

INCLUDE_RODATA("asm/nonmatchings/main/main", D_00393810);
INCLUDE_RODATA("asm/nonmatchings/main/main", D_00393820);

static void dbg_select_disp(void) {
    int selpos;
    DBG_MODE_STR *dbg_pp;
    int i;
    int numkun; /* TODO: can't find an use for this (v0). */

    selpos = 0;
    DbgMsgInit();

    while (1) {
        MtcWait(1);
        dbg_pp = &dbg_mode_str[selpos];
        if (pad[0].one & (SCE_PADRleft | SCE_PADRright)) {
            if (dbg_pp->set_pp == NULL) {
                break;
            }

            if (pad[0].one & SCE_PADRright) {
                (*dbg_pp->set_pp)++;
            } else {
                (*dbg_pp->set_pp)--;
            }

            if (*dbg_pp->set_pp < dbg_pp->min) {
                *dbg_pp->set_pp = dbg_pp->max - 1;
            }

            if (*dbg_pp->set_pp >= dbg_pp->max) {
                *dbg_pp->set_pp = dbg_pp->min;
            }
        } else {
            if (pad[0].one & SCE_PADLup) {
                selpos--;
            } else if (pad[0].one & SCE_PADLdown) {
                selpos++;
            }
        }

        if (selpos < 0) {
            selpos = 3;
        }
        if (selpos > PR_ARRAYSIZEU(dbg_mode_str) - 1) {
            selpos = 0;
        }

        DbgMsgClear();
        DbgMsgSetSize(16, 10);
        DbgMsgSetColor(0, 128, 0);
        DbgMsgPrint("= DEBUG MENU =", 1800, 1950);

        for (i = 0; i < PR_ARRAYSIZEU(dbg_mode_str); i++) {
            if (i == selpos) {
                DbgMsgSetColor(128, 128, 0);
            } else {
                DbgMsgSetColor(128, 128, 128);
            }

            DbgMsgPrint(dbg_mode_str[i].msg_pp, 1800, (1968 + (i*12)));
            if (dbg_mode_str[i].selmsg_pp != NULL) {
                DbgMsgPrint(dbg_mode_str[i].selmsg_pp[*dbg_mode_str[i].set_pp - dbg_mode_str[i].min], 2000, (1968 + (i*12)));
            }
        }

        DbgMsgFlash();
    }

    MtcWait(1);
}

INCLUDE_RODATA("asm/nonmatchings/main/main", D_00393840);
INCLUDE_RODATA("asm/nonmatchings/main/main", D_00393860);
INCLUDE_RODATA("asm/nonmatchings/main/main", D_00393880);
INCLUDE_RODATA("asm/nonmatchings/main/main", D_003938A8);
INCLUDE_RODATA("asm/nonmatchings/main/main", D_003938D0);
INCLUDE_RODATA("asm/nonmatchings/main/main", D_003938F8);

INCLUDE_RODATA("asm/nonmatchings/main/main", D_00393910);

#ifndef NON_MATCHING
INCLUDE_ASM("asm/nonmatchings/main/main", dummyPlay);
void dummyPlay(/* s0 16 */ int retTitle);
#else
static void dummyPlay(int retTitle) {
    int     mode;
    int     ret = 0;
    long    scoreTmp[2] = {};

    u_char *msgDmy[5] = {
        "TITLE   A or O or X EXIT",
        "REPLAY  A or O or X EXIT",
        "SINGLE  A .. COOL O .. GOOD  X .. NG",
        "VS MAN  A..1p win O..2p win X..EXIT",
        "VS COM  A or O .. CLEAR  X .. NG",
    };
    u_char msg_dat[80];

    if (retTitle != 0) {
        mode = 0;
    } else if (game_status.demo_flagG == DEMOF_REPLAY) {
        mode = 1;
    } else if (game_status.play_modeG == PLAY_MODE_SINGLE) {
        mode = 2;
    } else if (game_status.play_modeG == PLAY_MODE_VS_MAN) {
        mode = 3;
    } else {
        mode = 4;
    }

    game_status.bonusG = 0;

    DbgMsgInit();

    while (1) {
        u_char *pmd[3]   = { "SINGLE", "VS MAN", "VS COM" };
        u_char *ptype[2] = { "NORMAL", "EASY" };

        if (pad[0].one & 0x20) ret = 1;
        else if (pad[0].one & 0x10) ret = 3;
        else if (pad[0].one & 0x40) ret = 2;
        if (ret != 0) {
            break;
        }

        msg_dat[0] = '\0';
        DbgMsgClear();
        DbgMsgSetSize(16, 10);
        DbgMsgSetColor(128, 128, 128);

        DbgMsgPrint(msgDmy[mode], 1780, 1968);

        switch (mode) {
        case 2:
        case 3:
        case 4:
            if (pad[0].shot & 0x4) scoreTmp[0] += 10;
            if (pad[0].shot & 0x1) scoreTmp[0] -= 10;
            if (pad[0].shot & 0x8) scoreTmp[1] += 10;
            if (pad[0].shot & 0x2) scoreTmp[1] -= 10;
            if (scoreTmp[0] < 0) scoreTmp[0] = 0;
            if (scoreTmp[1] < 0) scoreTmp[1] = 0;

            sprintf(msg_dat, "STG:%d MODE:%s TYPE:%s ROUND:%d", game_status.play_stageG, pmd[game_status.play_modeG], ptype[game_status.play_table_modeG], game_status.roundG + 1);
            DbgMsgPrint(msg_dat, 1770, 2000);

            DbgMsgSetColor(0, 255, 0);
            DbgMsgPrint("SCORE 1P  L1 .. UP   L2 .. DOWN", 1770, 2020);
            DbgMsgPrint("SCORE 2P  R1 .. UP   R2 .. DOWN", 1770, 2032);

            DbgMsgSetColor(255, 255, 0);
            sprintf(msg_dat, "1P:%5d    2P:%5d", (int)scoreTmp[0], (int)scoreTmp[1]);
            DbgMsgPrint(msg_dat, 1770, 0x7fe);
            break;
        case 1:
            sprintf(msg_dat, "STG:%d MODE:%s TYPE:%s ROUND:%d", mc_rep_str.play_stageS, pmd[mc_rep_str.play_modeS], ptype[mc_rep_str.play_typeS], mc_rep_str.roundS + 1);
            DbgMsgPrint(msg_dat, 1770, 2000);
        }

        DbgMsgFlash();
        MtcWait(1);
    }

    switch (mode) {
    case 2:
        if (ret == 1 || ret == 3) {
            u_int clrcnt;

            if (game_status.endingFlag == 1) {
                while (1) {
                    MtcWait(1);
                    if (pad[0].one & 0x70) {
                        break;
                    }

                    DbgMsgClear();
                    DbgMsgSetSize(16, 10);
                    DbgMsgSetColor(128, 128, 128);
                    DbgMsgPrint("ENDING   A or O or X  EXIT", 1800, 1968);
                    DbgMsgFlash();
                }
            } else if (game_status.endingFlag == 2 || game_status.endingFlag == 3 || game_status.endingFlag == 4) {
                while (1) {
                    MtcWait(1);
                    if (pad[0].one & 0x70) {
                        break;
                    }

                    DbgMsgClear();
                    DbgMsgSetSize(16, 10);
                    DbgMsgSetColor(128, 128, 128);
                    DbgMsgPrint("BONUS GAME   A or O or X  EXIT", 1800, 1968);
                    DbgMsgFlash();
                }
            }

            menu_str.sel_menu_enum = SEL_MENU_SAVE;

            mc_rep_str.play_stageS = game_status.play_stageG;
            mc_rep_str.play_modeS = game_status.play_modeG;
            mc_rep_str.play_typeS = game_status.play_typeG;
            mc_rep_str.play_table_modeS = game_status.play_table_modeG;
            mc_rep_str.roundS = game_status.roundG;
            
            if (ret == 1) {
                clrcnt = game_status.stClrCntGood[game_status.play_stageG];
                if (clrcnt != -1) {
                    clrcnt++;
                }
                game_status.stClrCntGood[game_status.play_stageG] = clrcnt;

                game_status.disp_level = DLVL_GOOD;
            } else {
                clrcnt = game_status.stClrCntCool[game_status.play_stageG];
                if (clrcnt != -1) {
                    clrcnt++;
                }
                game_status.stClrCntCool[game_status.play_stageG] = clrcnt;

                game_status.disp_level = DLVL_COOL;
            }

            game_status.scoreG[0] = scoreTmp[0];
            game_status.scoreG[1] = 0;
        } else {
            game_status.disp_level = DLVL_BAD;

            menu_str.sel_menu_enum = SEL_MENU_STAGESEL;
            game_status.scoreG[0] = 0;
            game_status.scoreG[1] = 0;
        }    
        break;
    case 4:
        if (ret == 1 || ret == 3) {
            u_int       clrcnt;
            GLOBAL_PLY *gply_pp;

            menu_str.sel_menu_enum = SEL_MENU_SAVE;

            mc_rep_str.play_stageS = game_status.play_stageG;
            mc_rep_str.play_modeS = game_status.play_modeG;
            mc_rep_str.play_typeS = game_status.play_typeG;
            mc_rep_str.roundS = game_status.roundG;
            mc_rep_str.play_table_modeS = game_status.play_table_modeG;

            clrcnt = game_status.stClrCntVs[game_status.play_stageG];
            if (clrcnt != -1) {
                clrcnt++;
            }
            game_status.stClrCntVs[game_status.play_stageG] = clrcnt;

            game_status.scoreG[0] = scoreTmp[0];
            game_status.scoreG[1] = scoreTmp[1];

            gply_pp = &global_data.global_ply[0];
            gply_pp->vsWin = 3;
            gply_pp->vsLost = 0;

            gply_pp = &global_data.global_ply[1];
            gply_pp->vsWin = 0;
            gply_pp->vsLost = 3;
        } else {
            menu_str.sel_menu_enum = SEL_MENU_STAGESEL;
            game_status.scoreG[0] = 0;
            game_status.scoreG[1] = 0;
        }
        break;
    case 3:
        if (ret == 1 || ret == 3) {
            menu_str.sel_menu_enum = SEL_MENU_SAVE;

            game_status.scoreG[0] = scoreTmp[0];
            game_status.scoreG[1] = scoreTmp[1];

            mc_rep_str.play_stageS = game_status.play_stageG;
            mc_rep_str.play_modeS = game_status.play_modeG;
            mc_rep_str.play_typeS = game_status.play_typeG;
            mc_rep_str.roundS = game_status.roundG;
            mc_rep_str.play_table_modeS = game_status.play_table_modeG;

            if (ret == 1) {
                GLOBAL_PLY *gply_pp;
                gply_pp = &global_data.global_ply[0];
                gply_pp->vsWin = 0;
                gply_pp->vsLost = 3;

                gply_pp = &global_data.global_ply[1];
                gply_pp->vsWin = 3;
                gply_pp->vsLost = 0;
            } else {
                GLOBAL_PLY *gply_pp;

                gply_pp = &global_data.global_ply[0];
                gply_pp->vsWin = 3;
                gply_pp->vsLost = 0;

                gply_pp = &global_data.global_ply[1];
                gply_pp->vsWin = 0;
                gply_pp->vsLost = 3;
            }
        } else {
            menu_str.sel_menu_enum = SEL_MENU_STAGESEL;
            game_status.scoreG[0] = 0;
            game_status.scoreG[1] = 0;
        }
        break;
    case 1:
        menu_str.sel_menu_enum = SEL_MENU_REPLAY;
        break;
    default:
        menu_str.sel_menu_enum = SEL_MENU_STAGESEL;
        break;
    }
}
#endif

extern char D_00393A00[]; /* rodata - "overlay module load in\n" */
extern char D_00393A18[]; /* rodata - "overlay module load out\n" */

int selPlayDisp(int sel_stage, int sel_disp, int firstf) {
    STDAT_DAT *stdat_dat_pp;
    int        ret = 0;

    ReportHeapUsage();
    printf("=== selPlayDisp stg:%d disp:%d ===\n", sel_stage, sel_disp);

    /* Load stage overlay */
    printf(D_00393A00);
    CdctrlRead(&stdat_rec[sel_stage].ovlfile, overlay_loadaddr, NULL);
    CdctrlReadWait();
    printf(D_00393A18);

    asm("sync.l");
    FlushCache(WRITEBACK_DCACHE);

    stdat_dat_pp = &stdat_rec[sel_stage].stdat_dat_pp[sel_disp];

    CdctrlGetFileSize(&stdat_dat_pp->intfile);
    CdctrlRead(&stdat_dat_pp->intfile, UsrMemAllocNext(), UsrMemAllocEndNext());
    CdctrlReadWait();

    cmnfTim2Trans();

    global_data.play_stageL = sel_stage;
    GlobalPlySet(&global_data, stdat_dat_pp->play_step, sel_stage);
    GlobalTimeInit(&global_data);
    GlobalSetTempo(&global_data, stdat_rec[sel_stage].stdat_dat_pp[sel_disp].tempo);

    ScrCtrlInit(stdat_dat_pp, (void*)UsrMemGetAdr(0));

    do {
        MtcWait(1);
    } while (!ScrCtrlInitCheck());

    if (!firstf) {
        while (!WipeEndCheck()) {
            MtcWait(1);
        }
    }

    if (firstf) {
        FadeCtrlReq(FMODE_BLACK_IN, 120);
    }

    ScrCtrlGoLoop();

    if (!firstf) {
        WipeOutReq();
    }

    PrSetPostureWorkArea(UsrMemAllocNext(), UsrMemAllocEndNext() - UsrMemAllocNext());
    DrawCtrlInit(stdat_dat_pp->ev_pp, global_data.draw_tbl_top, (void*)UsrMemGetAdr(0));
    PrSetPostureWorkArea(NULL, NULL);

    DrawCtrlTimeSet(0);
    MtcWait(1);

    ReportHeapUsage();

    if (firstf) {
        MtcWait(120);
    }

    while (1) {
        MtcWait(1);

        if ((pad[0].one & SCE_PADstart) && WipeEndCheck()) {
            ret = 1;
            break;
        }

        if (ScrEndCheckScore()) {
            break;
        }

        if (ScrEndCheckFadeOut() && global_data.demo_flagL == DEMOF_DEMO) {
            ret = 2;
            break;
        }
    }

    if (ret == 2) {
        CdctrlSndFadeOutWait(120);
        ret = 0;
    }

    DrawCtrlQuit();
    CdctrlWP2SetVolume(0);
    CdctrlWp2FileEnd();
    ScrCtrlQuit();
    CdctrlMasterVolSet(0x3fff);

    return ret;
}

INCLUDE_RODATA("asm/nonmatchings/main/main", D_00393A00);
INCLUDE_RODATA("asm/nonmatchings/main/main", D_00393A18);

static void SpHatChangeSub(void) {
    PADD *pad_pp;

    hat_change_enum = HCNG_AUTO;

    if (game_status.roundG < TRND_R4) {
        return;
    }

    pad_pp = &pad[0];

    if (pad_pp->ana[1] < 0x40) {
        hat_change_enum = HCNG_R1;
    } else if (pad_pp->ana[0] >= 0xc0) {
        hat_change_enum = HCNG_R2;
    } else if (pad_pp->ana[1] >= 0xc0) {
        hat_change_enum = HCNG_R3;
    } else if (pad_pp->ana[0] < 0x40) {
        hat_change_enum = HCNG_R4;
    }

    PR_SCOPE
    int           rt2t_r1[3] = { 0xb6, 0xb7, 0xb8 };
    int           rt2t_r2[3] = { 0xb9, 0xba, 0xbb };
    int           rt2t_r3[3] = { 0xbc, 0xbd, 0xbe };
    int           rt2t_r4[3] = { 0xbf, 0xc0, 0xc1 };
    RT2TRANS_STR  rt2trans_str[4] = {
        { .num = 3, .data_pp = rt2t_r1 },
        { .num = 3, .data_pp = rt2t_r2 },
        { .num = 3, .data_pp = rt2t_r3 },
        { .num = 3, .data_pp = rt2t_r4 },
    };
    
    RT2TRANS_STR *rt2trans_str_pp = &rt2trans_str[GetHatRound()];
    int           i;

    for (i = 0; i < rt2trans_str_pp->num; i++) {
        Tim2Trans(GetIntAdrsCurrent(rt2trans_str_pp->data_pp[i]));
    }
    PR_SCOPEEND
}

extern char D_003996C0[]; /* sdata - "DEBUG" */

int selPlayDispTitleDisp(int sel_stage, int sel_disp, int ovl_load) {
    STDAT_DAT *stdat_dat_pp;
    int        ret = 0;

    ReportHeapUsage();
    printf("=== selPlayDisp stg:%d disp:%d ===\n", sel_stage, sel_disp);

    if (ovl_load) {
        printf(D_00393A00);
        CdctrlRead(&stdat_rec[sel_stage].ovlfile, overlay_loadaddr, NULL);
        CdctrlReadWait();
        printf(D_00393A18);
    }

    asm("sync.l");
    FlushCache(WRITEBACK_DCACHE);

    stdat_dat_pp = &stdat_rec[sel_stage].stdat_dat_pp[sel_disp];

    global_data.play_stageL = sel_stage;
    GlobalPlySet(&global_data, stdat_dat_pp->play_step, sel_stage);
    GlobalTimeInit(&global_data);
    GlobalSetTempo(&global_data, stdat_rec[sel_stage].stdat_dat_pp[sel_disp].tempo);

    ScrCtrlInit(stdat_dat_pp, (void*)UsrMemGetAdr(0));

    do {
        MtcWait(1);
    } while (!ScrCtrlInitCheck());

    ScrCtrlGoLoop();
    WipeOutReq();

    PrSetPostureWorkArea(UsrMemAllocNext(), UsrMemAllocEndNext() - UsrMemAllocNext());
    DrawCtrlInit(stdat_dat_pp->ev_pp, global_data.draw_tbl_top, (void*)UsrMemGetAdr(0));
    PrSetPostureWorkArea(NULL, NULL);

    DrawCtrlTimeSet(0);
    MtcWait(1);

    ReportHeapUsage();

    while (1) {
        MtcWait(1);

        if (ScrEndCheckTitle()) {
            ret = 1;
            break;
        }
        
        if (ScrEndCheckScore()) {
            break;
        }

        SpHatChangeSub();

        if (pad[0].one & SCE_PADLdown) {
            dbg_select_str.debug_on ^= 1;
        }

        if (dbg_select_str.debug_on) {
            sceGifPacket dbgPk;

            DbgMsgInit();
            DbgMsgClear();

            CmnGifOpenCmnPk(&dbgPk);
            DbgMsgClearUserPkt(&dbgPk);
            DbgMsgPrintUserPkt(D_003996C0, 1730, 1948, &dbgPk);
            CmnGifCloseCmnPk(&dbgPk, 7);
        }
    }

    if (ret != 0) {
        MtcWait(60);
    }

    DrawCtrlQuit();
    CdctrlWP2SetVolume(0);
    CdctrlWp2FileEnd();
    ScrCtrlQuit();
    return ret;
}

void xtrView(FILE_STR *file_str_pp) {
    int timer;
    int seek_top;

    timer = 0;

    CdctrlXTRset(file_str_pp, UsrMemAllocNext());
    seek_top = getTopSeekPos();

    CdctrlWP2Play();
    CdctrlWP2SetVolume(0x3fff);

    while (1) {
        MtcWait(1);

        if (pad[0].one & SCE_PADstart || CdctrlWP2PlayEndCheck() || timer >= 6540) {
            break;
        }

        CdctrlWp2GetSampleTmpBuf();

        timer = CdctrlWp2CdSample2Frame(CdctrlWp2GetSampleTmp() - seek_top);
        p3StrPoll(timer);

        timer++;
    }

    CdctrlWp2FileEnd();
    p3StrQuitSd();
}

void logoDispOne(SPR_PRIM *sprm_pp, TIM2_DAT *tmd_pp) {
    int timer;

    sprm_pp->w = tmd_pp->w;
    sprm_pp->h = tmd_pp->h;
    FadeCtrlReq(FMODE_BLACK_IN, 30);

    timer = 29;
    while (timer != -1) {
        timer--;

        SprClear();
        SprPackSet((SPR_DAT*)tmd_pp);
        SprDisp(sprm_pp);
        SprFlash();

        MtcWait(1);
    }

    timer = 119;
    while (timer != -1) {
        timer--;

        SprClear();
        SprPackSet((SPR_DAT*)tmd_pp);
        SprDisp(sprm_pp);
        SprFlash();

        MtcWait(1);
    }

    FadeCtrlReq(FMODE_BLACK_OUT, 30);

    timer = 29;
    while (timer != -1) {
        timer--;

        SprClear();
        SprPackSet((SPR_DAT*)tmd_pp);
        SprDisp(sprm_pp);
        SprFlash();

        MtcWait(1);
    }
}

static void uramenFileSearchTask(void *x) {
    printf("file search in\n");
    stDatFirstFileSearch();
    printf("file search out\n");

    uramen_end_flag = FALSE;
    MtcExit();
}

static void uramenFileSearchSet(void) {
    uramen_end_flag = TRUE;
    MtcExec(uramenFileSearchTask, MTC_TASK_03);
}

static void uramenFileSearchEnd(void) {
    if (uramen_end_flag) {
        while (uramen_end_flag) {
            MtcWait(1);
        }
    }
}

/* note: has inline from 'data/logo/logo_tm2.h' */
void startUpDisp(void) {
    /* Splash screens TIM2 data */
    /* TODO: match .data (see symbol_addrs.txt) */
    extern TIM2_DAT tim2spr_tbl_tmp0[2]; /* static */ /* =
    {
        { .GsTex0 = 0x20050A022531A800, .GsTex1 = 0x260, .w = 320, .h = 224 }, / NanaOn-Sha /
        { .GsTex0 = 0x20150A81A5422968, .GsTex1 = 0x260, .w = 440, .h = 52  }, / SCEI presents /
    }; */

    /* Splash screens sprite data */
    SPR_PRIM spr_prim[2] = {
        { .x = 2048, .y = 2048, .scalex = 256, .scaley = 128 },
        { .x = 2048, .y = 2048, .scalex = 512, .scaley = 256 }
    };

    UsrMemClear();
    SpuBankSet();

    CdctrlRead(&file_str_logo_file, UsrMemAllocNext(), 0);
    CdctrlReadWait();

    SprInit();
    MenuMemCardCheck();

    uramenFileSearchSet();
    logoDispOne(&spr_prim[0], &tim2spr_tbl_tmp0[1]);
    logoDispOne(&spr_prim[1], &tim2spr_tbl_tmp0[0]);
    uramenFileSearchEnd();

    UsrMemClear();
    SpuBankSet();
}

INCLUDE_ASM("asm/nonmatchings/main/main", selPlayDispType);

INCLUDE_ASM("asm/nonmatchings/main/main", selPlayDispSetPlay);

int selPlayDispSetPlayOne(int sel_stage) {
    int        i;
    STDAT_DAT *stdat_dat_pp;
    int        fsize;
    int        ret;
    int        decp;

    ret = 0;

    printf(D_00393A18);
    CdctrlRead(&stdat_rec[sel_stage].ovlfile, overlay_loadaddr, NULL);
    printf(D_00393A00);
    CdctrlReadWait();

    i = stdat_rec[sel_stage].stdat_dat_num - 1;
    stdat_dat_pp = &stdat_rec[sel_stage].stdat_dat_pp[i];

    fsize = CdctrlGetFileSize(&stdat_dat_pp->intfile);
    CdctrlReadOne(&stdat_dat_pp->intfile, UsrMemEndAlloc(((fsize + 2047) / 2048) * 2048), NULL);
    CdctrlReadWait();

    decp = UsrMemAllocEndNext();
    UsrMemEndFree();

    CdctrlMemIntgDecode(decp, UsrMemAllocNext());

    cmnfTim2Trans();

    if (stdat_dat_pp->play_step == PSTEP_HOOK) {
        ret = selPlayDispType(sel_stage, i, CBE_HOOK);
    } else if (global_data.play_modeL == PLAY_MODE_SINGLE) {
        ret = selPlayDispType(sel_stage, i, CBE_SINGLE);
    } else if (global_data.play_modeL == PLAY_MODE_VS_MAN) {
        ret = selPlayDispType(sel_stage, i, CBE_VS_MAN);
    } else if (global_data.play_modeL == PLAY_MODE_VS_COM) {
        ret = selPlayDispType(sel_stage, i, CBE_VS_COM);
    }

    UsrMemClearTop();
    return ret;
}

INCLUDE_ASM("asm/nonmatchings/main/main", gamePlayDisp);
int gamePlayDisp(void);

void titleDisp(int firstf) {
    STDAT_DAT *stdat_dat_pp;
    int        fsize, decp;
    int        loop;
    int        deramode;

    deramode = 0;
    loop = 0;

    while (1) {
        UsrMemClear();
        SpuBankSet();

        game_status.demo_flagG = DEMOF_OFF;
        GlobalLobcalCopy();

        stdat_dat_pp = stdat_rec[19].stdat_dat_pp;
        fsize = CdctrlGetFileSize(&stdat_dat_pp->intfile);
        CdctrlReadOne(&stdat_dat_pp->intfile, UsrMemEndAlloc(((fsize + 2047) / 2048) * 2048), NULL);
        CdctrlReadWait();

        if (loop == 0) {
            selPlayDisp(0, 0, firstf);
            WipeInReqSame();
            SetBackColor(0xff, 0xff, 0xff);
        }
        MtcWait(2);

        decp = UsrMemAllocEndNext();
        UsrMemClearTop();
        UsrMemEndFree();
        CdctrlMemIntgDecode(decp, UsrMemAllocNext());

        game_status.demo_flagG = DEMOF_OFF;
        GlobalLobcalCopy();

        if (selPlayDispTitleDisp(19, deramode, loop)) {
            SetBackColor(0, 0, 0);
            break;
        }

        firstf = 0;
        SetBackColor(0, 0, 0);
        deramode ^= 1;

        WipeInReq();
        MtcWait(2);

        SpuBankSet();
        UsrMemClear();

        game_status.demo_flagG = DEMOF_DEMO;

        loop = gamePlayDisp();
        WipeInReq();
        MtcWait(2);
    }
}

int urawazaKeyCheck(void) {
    PADD *pad_pp;
    int change_tbl[17] = {
        1,  3,  5,  7,
        0,  10, 12, 14,
        16, 15, 13, 11,
        9,  8,  6,  4,  2,
    };
    int   ud_d, ret;
    float pos;

    pad_pp = &pad[0];

    /* R3 button */
    if (!(pad_pp->shot & SCE_PADj)) {
        ret = -1;
    } else {
        if (pad_pp->ana[PAD_ANA_RY] <  (128 - 64) || pad_pp->ana[PAD_ANA_RX] >= (64 + 128) ||
            pad_pp->ana[PAD_ANA_RY] >= (64 + 128) || pad_pp->ana[PAD_ANA_RX] <  (128 - 64)) {
            int rx = pad_pp->ana[PAD_ANA_RX] - 128;
            int ry = pad_pp->ana[PAD_ANA_RY] - 128;
            pos = atan2(-rx, ry);
            pos = (pos + 3.1415927f);
            pos = (pos * 17.0f) / 6.2831855f;
            ud_d = pos;
        } else {
            ud_d = randMakeMax(17);
        }

        if (ud_d < 0) {
            ud_d = 0;
        }
        if (ud_d >= 17) {
            ud_d = 0;
        }

        ud_d = change_tbl[ud_d];
        printf("level fix:%d\n", ud_d);
    
        ret = ud_d;
    }

    return ret;
}

extern char D_003996D0[]; /* sdata - "ura:%d" */

void ura_check(void) {
    u_char msg_tmp[32];
    int    ret;

    DbgMsgInit();

    while (1) {
        MtcWait(1);

        ret = urawazaKeyCheck();
        if (ret < 0) {
            sprintf(msg_tmp, "ura:NOUSE");
        } else {
            sprintf(msg_tmp, D_003996D0, ret);
        }

        DbgMsgClear();

        DbgMsgSetSize(22, 10);
        DbgMsgSetColor(128, 128, 128);
        DbgMsgPrint(msg_tmp, 1820, 1948);

        DbgMsgFlash();
    }
}

void mainStart(void *xx) {
    /* sdata 3996d8 */ extern int first_f;
    int retTitle;

    mccReqInit();
    CdctrlInit();

    PrInitializeModule(DBufDc.draw01.zbuf1);
    UsrPrInitScene();

    hat_change_enum = HCNG_AUTO;

    TapCtInit();
    TapCt(0, 0, 0);

    TimeCallbackSet();
    GlobalInit();
    UsrMemClear();
    SpuBankSetAll();

    menu_str.sel_menu_enum = SEL_MENU_STAGESEL;
    menu_str.mc_rep_str_p = &mc_rep_str;
    menu_str.game_status_p = &game_status;

    cmnfTim2Trans();
    wipeSndFileTrans();

    CdctrlReadWait();
    printf("int read end\n");

    startUpDisp();

    while (1) {
        urawaza_levelsel_bottun = -1;
        titleDisp(first_f);

        WipeInReq();
        MtcWait(2);

        first_f = 0;

        menu_str.sel_menu_enum = SEL_MENU_STAGESEL;
        game_status.demo_flagG = DEMOF_OFF;

        while (1) {
            UsrMemClear();
            SpuBankSet();

            urawaza_levelsel_bottun = -1;
            urawaza_skip_bottun = 0;

            CdctrlRead(&file_str_menu_file, UsrMemAllocNext(), UsrMemAllocEndNext());
            CdctrlReadWait();

            while (!WipeEndCheck()) {
                MtcWait(1);
            }
            WipeOutReq();

            PrSetStage(0);
            game_status.play_typeG = PLAY_TYPE_NORMAL;

            retTitle = MenuCtrl(&menu_str);

            /*
             * Cheat code: L1 + R1
             *
             * Skip cutscenes + Boxy Boy practice.
             */
            if (pad[0].shot & SCE_PADL1 &&
                pad[0].shot & SCE_PADL2) {
                urawaza_skip_bottun = 1;
            } else {
                urawaza_skip_bottun = 0;
            }

            /*
             * Cheat code: L2 + R2
             *
             * Enable 'shuriken' mode.
             */
            if (pad[0].shot & SCE_PADR2 &&
                pad[0].shot & SCE_PADR1 &&
                game_status.play_modeG == PLAY_MODE_SINGLE) {
                game_status.play_typeG = PLAY_TYPE_ONE;
            } else {
                game_status.play_typeG = PLAY_TYPE_NORMAL;
            }

            /*
             * Cheat code: R3 + Right analog stick
             *
             * Change stage difficulty, can only be
             * used given the following conditions:
             *    - Singleplayer mode.
             *    - 'Shuriken' mode is disabled.
             *    - Max round/circuit (Yellow hat).
             *    - Current stage is not a demo.
             *
             *    (TODO): Document:
             *      - game_status.play_table_modeG != PLAY_TABLE_EASY
             */
            if (game_status.play_modeG == PLAY_MODE_SINGLE &&
                game_status.play_typeG == PLAY_TYPE_NORMAL &&
                game_status.roundG >= TRND_R4 &&
                game_status.demo_flagG == DEMOF_OFF &&
                game_status.play_table_modeG != PLAY_TABLE_EASY) {
                urawaza_levelsel_bottun = urawazaKeyCheck();
            } else {
                urawaza_levelsel_bottun = -1;
            }

            if (game_status.play_typeG == PLAY_TYPE_ONE) {
                if (game_status.endingFlag == 2) {
                    game_status.endingFlag = 0;
                }
            
                if (game_status.endingFlag == 3) {
                    game_status.endingFlag = 0;
                }
            
                if (game_status.endingFlag == 4) {
                    game_status.endingFlag = 0;
                }
            }

            if (game_status.play_table_modeG == PLAY_TABLE_EASY) {
                if (game_status.endingFlag == 2) {
                    game_status.endingFlag = 0;
                }
            
                if (game_status.endingFlag == 3) {
                    game_status.endingFlag = 0;
                }
            
                if (game_status.endingFlag == 4) {
                    game_status.endingFlag = 0;
                }
            }

            if (dbg_select_str.debug_on && !retTitle) {
                dbg_select_disp();
            }

            WipeInReq();
            UsrMemClear();
            SpuBankSet();

            if (retTitle) {
                break;
            }

            if (dbg_select_str.debug_on && dbg_select_str.non_play) {
                while (!WipeEndCheck()) {
                    MtcWait(1);
                }
                WipeOutReq();

                dummyPlay(retTitle);
            } else {
                gamePlayDisp();
            }

            WipeInReq();
            MtcWait(2);

            urawaza_levelsel_bottun = -1;
        }
    }
}
