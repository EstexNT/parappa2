#include "main/scrctrl.h"

#include "dbug/dbgmsg.h"

#include "os/cmngifpk.h"
#include "os/mtc.h"
#include "os/syssub.h"
#include "os/system.h"
#include "os/usrmem.h"

#include "main/mbar.h"
#include "main/mcctrl.h"
#include "main/p3str.h"
#include "main/sprite.h"

#include "iop_mdl/tapctrl_rpc.h"

#include <prlib/prlib.h>

#include <libpad.h>

#include <stdio.h>

/* sdata 399438 */ extern int titleStartKey; /* static */
/* sdata 39943c */ extern int fadeoutStartKey; /* static */
/* sdata 399440 */ extern int gameEndWaitLoop; /* static */
/* sdata 399444 */ extern int replayGuiOffFlag; /* static */
/* sdata 399448 */ extern int jimakuWakuOff; /* static */
/* sdata 39944c */ extern int currentTblNumber;
/* sdata 399450 */ extern int vs_tapdat_tmp_cnt;
/* sdata 399454 */ extern int scrJimakuLine;
/* sdata 399458 */ extern int scrDrawLine;
/* sdata 39945c */ extern int scrMbarLine;
/* sdata 399460 */ extern int scrRefLineTime;
// /* data 183258 */ static EXH_STR exh_str_normal[];
// /* data 1832a8 */ static EXH_STR exh_str_original[];
// /* data 1832e0 */ static EXH_STR exh_str_hane[];
// /* data 183310 */ static EXH_STR exh_str_hook[];
/* data 183320 */ static u_int thnum_tbl[]; /* static */
/* data 183330 */ extern TCL_CTRL tcl_ctrl[4][33];
// /* data 183b70 */ static SCRPRGSTR scrprgstr[];
// /* data 183b88 */ static SCRPRGSTR scrprgstr_hook[];
// /* data 183b90 */ static TIM2_DAT tim2spr_tbl[];
// /* data 183bd0 */ static BN_NUM_TYPE bn_num_type[];
/* data 183c10 */ extern LERO_TIM2_PT lero_tim2_pt[]; /* static */
/* data 183c68 */ extern LERO_POS_STR lero_pos_str[][2]; /* static */
/* data 183d58 */ extern SCR_SND_DBUFF scr_snd_dbuff;
/* bss 1c5f030 */ extern SNDTAP *scr_sndtap_pp[4]; /* static */
/* data 183d70 */ extern SCORE_STR score_str;
/* bss 1c5f040 */ extern SCORE_INDV_STR score_indv_str[5];
/* data 183d88 */ extern TAPDAT vs_tapdat_tmp[64];
/* sbss 399a38 */ extern int follow_scr_tap_memory_cnt; /* static */
/* sbss 399a3c */ extern int follow_scr_tap_memory_cnt_load; /* static */
/* bss 1c63a68 */ extern SCR_TAP_MEMORY follow_scr_tap_memory[256]; /* static */
/* bss 1c64668 */ extern TAP_GROUPE_STR tap_groupe_str[5]; /* static */
// /* bss 1c646b8 */ static COMMAKE_STR commake_str[32];
// /* sbss 399a40 */ static int commake_str_cnt;
// /* bss 1c647b8 */ static EXAM_CHECK exam_check[3];
// /* bss 1c65b00 */ static u_char yaku_tmp_buf[36];
/* bss 1c65b28 */ extern BNG_STR bng_str; /* static */

static void exam_tbl_updownInit(SCORE_INDV_STR *sindv_pp);
static void exam_tbl_updownSet(SCORE_INDV_STR *sindv_pp, int now, int sikiichi, int oth);
static int  exam_tbl_updownChange(SCORE_INDV_STR *sindv_pp, TAP_CTRL_LEVEL_ENUM clv, TAP_ROUND_ENUM round, int coolf);
static void followTapInit(void);
static void followTapSave(SCORE_INDV_STR *sindv_pp);
static SCR_TAP_MEMORY* followTapLoad(int pos, int time);
static void ScrLincChangTbl(int line);
static void ScrLincChangTblRef(int line, int ck_time);
static int  tapLevelChangeSub(void);
static void tapLevelChange(SCORE_INDV_STR *sindv_pp);
static int  useIndevCodeGet(void);
static int  targetTimeGet(int line, int time, int codeAll);
static int  otehon_all_make(EXAM_CHECK *ec_pp);
static int  treateTimeChange(int time);
static int  thnum_get(int p96_num, CK_TH_ENUM ckth);
static int  MapNormalNumGet(int time);
static void on_th_make(EXAM_CHECK *ec_pp, CK_TH_ENUM ckth);
/* static */ int  exh_normal_add(EXAM_CHECK *ec_pp);
/* static */ int  exh_normal_sub(EXAM_CHECK *ec_pp);
static int  exh_nombar_sub(EXAM_CHECK *ec_pp);
/* static */ int  exh_mbar_key_out(EXAM_CHECK *ec_pp);
/* static */ int  exh_mbar_time_out(EXAM_CHECK *ec_pp);
/* static */ int  exh_mbar_num_out(EXAM_CHECK *ec_pp);
static int  exh_yaku(EXAM_CHECK *ec_pp, int hane_flag);
/* static */ int  exh_yaku_original(EXAM_CHECK *ec_pp);
/* static */ int  exh_yaku_hane(EXAM_CHECK *ec_pp);
static int  exh_allkey_out(EXAM_CHECK *ec_pp);
/* static */ int  exh_allkey_out_nh(EXAM_CHECK *ec_pp);
/* static */ int  exh_command(EXAM_CHECK *ec_pp);
/* static */ int  exh_renda_out(EXAM_CHECK *ec_pp);
static int  manemane_check_sub(EXAM_CHECK *ec_pp);
static int  manemane_check(EXAM_CHECK *ec_pp);
/* static */ int  exh_mane(EXAM_CHECK *ec_pp);
/* static */ int  exh_all_add(EXAM_CHECK *ec_pp);
static TAPSET* IndvGetTapSetAdrs(SCORE_INDV_STR *sindv_pp);
static int  nextExamTime(void);
static SCORE_INDV_STR* GetSindvPcodeLine(PLAYER_CODE pcode);
static void ExamScoreCheck(SCORE_INDV_STR *sindv_pp);
static int  ExamScoreCheckSame(SCORE_INDV_STR *sindv_pp);
static int  levelChangeCheck(RANK_LEVEL lvl0, RANK_LEVEL lvl1);
static int  levelUpRank(RANK_LEVEL lvl);
static int  levelDownRank(RANK_LEVEL lvl);
static void ScrCtrlIndvJob(void);
static void ScrTimeRenew(SCR_MAIN *scr_main_pp);
static int  otehonSetCheck(void);
static void bonusGameInit(void);
static int  bonusGameCntPls(void);
static void bonusPointSave(void);
static void bonusGameParaReq(BNG_ACT_P_ENUM actnum);
static void bonusGameKoamaReq(int kotamaNum, BNG_ACT_K_ENUM actnum);
static int  bonus_minus_point_sub(int wtime);
static int  bonus_pls_point_sub(int wtime);
static void bonusGameCtrl(int time);
static u_long hex2dec(u_long data);
static void bnNumberDisp(sceGifPacket *gif_pp, long score, short x, short y, int keta, int tate, int type);
static void bonusScoreDraw(void);
static void set_lero_gifset(sceGifPacket *gifpk_pp, LERO_TIM2_PT *let2_pp, short xp, short yp);
static void LessonRoundDisp(SCRRJ_LESSON_ROUND_ENUM type);

int GetCurrentTblNumber(void) {
    return currentTblNumber;
}

DISP_LEVEL RANK_LEVEL2DISP_LEVEL(RANK_LEVEL lvl) {
    DISP_LEVEL lvl_tbl[17] = {
        DLVL_COOL,  DLVL_COOL,
        DLVL_GOOD,  DLVL_GOOD,  DLVL_GOOD,
        DLVL_BAD,   DLVL_BAD,   DLVL_BAD,
        DLVL_AWFUL, DLVL_AWFUL, DLVL_AWFUL,
        DLVL_MAX,   DLVL_MAX,   DLVL_MAX,
        DLVL_MAX,   DLVL_MAX,   DLVL_MAX
    };

    return lvl_tbl[lvl];
}

DISP_LEVEL RANK_LEVEL2DISP_LEVEL_HK(RANK_LEVEL lvl) {
    DISP_LEVEL lvl_tbl[15] = {
        DLVL_COOL,  DLVL_COOL,
        DLVL_GOOD,  DLVL_GOOD,  DLVL_GOOD,
        DLVL_BAD,   DLVL_BAD,   DLVL_BAD,
        DLVL_AWFUL, DLVL_AWFUL, DLVL_AWFUL,
        DLVL_MAX,   DLVL_MAX,   DLVL_MAX, DLVL_HK_MAX
    };

    return lvl_tbl[lvl];
}

void ScrTapDbuffCtrlInit(void *data_top, int bk0, int bk1) {
    scr_snd_dbuff.bank[0]      = bk0;
    scr_snd_dbuff.bank[1]      = bk1;

    scr_snd_dbuff.data_top     = data_top;
    scr_snd_dbuff.next_index   = 0;

    scr_snd_dbuff.sndrec_pp[0] = NULL;
    scr_snd_dbuff.sndrec_pp[1] = NULL;
}

u_int ScrTapDbuffSet(SNDREC *sndrec_pp) {
    u_int ret;
    u_int id;

    id = scr_snd_dbuff.next_index & 1;
    if (scr_snd_dbuff.sndrec_pp[id] == sndrec_pp) {
        ret = scr_snd_dbuff.next_index;
        scr_snd_dbuff.next_index++;
        return ret;
    }

    if (scr_snd_dbuff.sndrec_pp[id ^ 1] == sndrec_pp) {
        return id ^ 1;
    }

    ScrTapDataTrans(sndrec_pp, scr_snd_dbuff.bank[id], scr_snd_dbuff.data_top);
    ret = id;

    scr_snd_dbuff.sndrec_pp[id] = sndrec_pp;
    scr_snd_dbuff.next_index++;
    return ret;
}

void ScrTapDbuffSetSp(SNDREC *sndrec_pp, int id) {
    if (id < 0) {
        return;
    }

    scr_snd_dbuff.next_index = id;
    ScrTapDataTrans(sndrec_pp, scr_snd_dbuff.bank[id & 1 ^ 1], scr_snd_dbuff.data_top);
    scr_snd_dbuff.sndrec_pp[id & 1 ^ 1] = sndrec_pp;
}

void ScrTapDbuffClear(void) {
    scr_snd_dbuff.next_index = 0;
    scr_snd_dbuff.sndrec_pp[0] = NULL;
    scr_snd_dbuff.sndrec_pp[1] = NULL;
}

void ScrTapCtrlInit(void *data_top) {
    ScrTapDbuffCtrlInit(data_top, 1, 2);
}

void ScrTapDataTrans(SNDREC *sndrec_pp, int bank, void *data_top) {
    if (sndrec_pp->bd_num >= 0) {
        TapCt(0x8030 | bank, (int)GetIntAdrsCurrent(sndrec_pp->bd_num), GetIntSizeCurrent(sndrec_pp->bd_num));
        TapCt(0x8040 | bank, (int)GetIntAdrsCurrent(sndrec_pp->hd_num), GetIntSizeCurrent(sndrec_pp->hd_num));
    }

    scr_sndtap_pp[bank] = sndrec_pp->sndtap_pp;

    TapCt(0x90, PR_CONCAT(0x3fff, 0x3fff), 0);
    TapCt(0xa0, PR_CONCAT(0x3fff, 0x3fff), 0);
}

int ScrTapDataTransCheck(void) {
    return TapCt(0x8070, 0, 0);
}

void ScrTapReq(int id, int box, int num) {
    int     use_chan;
    SNDTAP *tp_pp;

    if (id == -1) {
        use_chan = 0;
    } else {
        use_chan = scr_snd_dbuff.bank[id & 1];
    }

    tp_pp = scr_sndtap_pp[use_chan] + num;

    TapCt(0xf0 | use_chan, box, tp_pp->volume);
    TapCt(0xd0 | use_chan, box, tp_pp->prg + tp_pp->key * 256);
}

void ScrTapReqStop(int box) {
    TapCt(0xe0, box, 0);
}

static void exam_tbl_updownInit(SCORE_INDV_STR *sindv_pp) {
    int i;

    if (sindv_pp->global_ply == NULL) {
        return;
    }

    for (i = 0; i < PR_ARRAYSIZE(sindv_pp->global_ply->exam_tbl_updown); i++) {
        sindv_pp->global_ply->exam_tbl_updown[i] = 0;
    }
}

static void exam_tbl_updownSet(SCORE_INDV_STR *sindv_pp, int now, int sikiichi /* Threshold */, int oth) {
    int hikaku; /* Comparison variable */

    if (sindv_pp->global_ply == NULL) {
        return;
    }

    if (oth >= sikiichi / 2) {
        sindv_pp->global_ply->exam_tbl_updown[0]++;
    } else {
        sindv_pp->global_ply->exam_tbl_updown[0]--;
    }

    hikaku = sikiichi / 2;

    if (now > hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[1]++;
    }
    if (now >= hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[2]++;
    }
    if (now <= hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[3]++;
    }
    if (now < hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[4]++;
    }

    hikaku = oth;

    if (now > hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[5]++;
    }
    if (now >= hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[6]++;
    }
    if (now <= hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[7]++;
    }
    if (now < hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[8]++;
    }

    hikaku = 0;

    if (now > hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[9]++;
    }
    if (now >= hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[10]++;
    }
    if (now <= hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[11]++;
    }
    if (now < hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[12]++;
    }

    hikaku = sikiichi;

    if (now > hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[13]++;
    }
    if (now >= hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[14]++;
    }
    if (now <= hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[15]++;
    }
    if (now < hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[16]++;
    }

    hikaku = (sikiichi - oth) / 2 + oth;

    if (now > hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[17]++;
    }
    if (now >= hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[18]++;
    }
    if (now <= hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[19]++;
    }
    if (now < hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[20]++;
    }

    hikaku = oth / 2;

    if (now > hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[21]++;
    }
    if (now >= hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[22]++;
    }
    if (now <= hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[23]++;
    }
    if (now < hikaku) {
        sindv_pp->global_ply->exam_tbl_updown[24]++;
    }
}

static int exam_tbl_updownChange(SCORE_INDV_STR *sindv_pp, TAP_CTRL_LEVEL_ENUM clv, TAP_ROUND_ENUM round, int coolf) {
    TCL_CTRL *tcl_ctrl_pp;
    int       ret = 0;
    int       udc;

    if (sindv_pp->global_ply == NULL) {
        return 0;
    }

    if (coolf) {
        tcl_ctrl_pp = &tcl_ctrl[round][32];
    } else {
        if (sindv_pp->global_ply->exam_tbl_updown[0] < 0) {
            tcl_ctrl_pp = &tcl_ctrl[round][clv];
        } else {
            tcl_ctrl_pp = &tcl_ctrl[round][clv + TCT_MAX];
        }
    }

    udc = 0;

    if (tcl_ctrl_pp->tcl_do_enum_up != TCL_DO_NONE) {
        udc = sindv_pp->global_ply->exam_tbl_updown[tcl_ctrl_pp->tcl_do_enum_up];
    }
    if (tcl_ctrl_pp->tcl_do_enum_down != TCL_DO_NONE) {
        udc -= sindv_pp->global_ply->exam_tbl_updown[tcl_ctrl_pp->tcl_do_enum_down];
    }

    if (udc < 0) {
        if (tcl_ctrl_pp->min < 0) {
            ret = tcl_ctrl_pp->min;
        } else {
            if (udc < -tcl_ctrl_pp->min) {
                ret = -tcl_ctrl_pp->min;
            } else {
                ret = udc;
            }
        }
    } else if (udc > 0) {
        if (tcl_ctrl_pp->max < 0) {
            ret = -tcl_ctrl_pp->max;
        } else {
            if (udc > tcl_ctrl_pp->max) {
                ret = tcl_ctrl_pp->max;
            } else {
                ret = udc;
            }
        }
    }

    return ret;
}

void vsTapdatSetMemorySave(void) {
    int        i;
    VSOTHSAVE  vsothsave_tmp;
    TAPDAT    *tapdat_pp;

    tapdat_pp = vs_tapdat_tmp;

    WorkClear(vsothsave_tmp, sizeof(vsothsave_tmp));

    for (i = 0; i < vs_tapdat_tmp_cnt; i++, tapdat_pp++) {
        int time = tapdat_pp->time / 24;
        if (time >= 32) {
            printf("OTH SAVE OVER\n");
        } else {
            vsothsave_tmp[time] = *(u_char*)&tapdat_pp->KeyIndex;
        }
    }

    mccReqVSOTHSAVEset(&vsothsave_tmp);
}

void vsTapdatSetMemoryLoad(void) {
    VSOTHSAVE vsothsave_tmp;
    int       i;

    WorkClear(&vsothsave_tmp, sizeof(vsothsave_tmp));

    if (mccReqVSOTHSAVEget(&vsothsave_tmp)) {
        vs_tapdat_tmp_cnt = 0;

        for (i = 0; i < 32; i++) {
            if (vsothsave_tmp[i] != 0) {
                vs_tapdat_tmp[vs_tapdat_tmp_cnt].time = i * 24;
                vs_tapdat_tmp[vs_tapdat_tmp_cnt].tapct[0].actor = -1;
                vs_tapdat_tmp[vs_tapdat_tmp_cnt].tapct[0].sound = -1;
                vs_tapdat_tmp[vs_tapdat_tmp_cnt].KeyIndex = vsothsave_tmp[i];
                vs_tapdat_tmp_cnt++;
            }
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/main/scrctrl", vsTapdatSet);

void vsTapdatSetMoto(SCORE_INDV_STR *sindv_pp) {
    int     i;
    TAPSET *tapset_pp;
    TAPDAT *tapdat_pp;

    vs_tapdat_tmp_cnt = 0;
    printf("reset moto!!\n");

    tapset_pp = IndvGetTapSetAdrs(sindv_pp);
    tapdat_pp = tapset_pp->tapdat_pp;

    for (i = 0; i < tapset_pp->tapdat_size; i++, tapdat_pp++) {
        if (tapdat_pp->KeyIndex != 0) {
            if (global_data.play_typeL == PLAY_TYPE_ONE) {
                vs_tapdat_tmp[vs_tapdat_tmp_cnt].KeyIndex = 1;
            } else {
                vs_tapdat_tmp[vs_tapdat_tmp_cnt].KeyIndex = tapdat_pp->KeyIndex;
            }

            vs_tapdat_tmp[vs_tapdat_tmp_cnt].time = tapdat_pp->time;
            vs_tapdat_tmp[vs_tapdat_tmp_cnt].tapct[0].actor = -1;
            vs_tapdat_tmp[vs_tapdat_tmp_cnt].tapct[0].sound = -1;

            vs_tapdat_tmp_cnt++;
        }
    }
}

static void followTapInit(void) {
    follow_scr_tap_memory_cnt = 0;
    follow_scr_tap_memory_cnt_load = 0;
    WorkClear(follow_scr_tap_memory, sizeof(follow_scr_tap_memory));
}

static void followTapSave(SCORE_INDV_STR *sindv_pp) {
    int i;

    follow_scr_tap_memory_cnt = 0;
    follow_scr_tap_memory_cnt_load = 0;

    for (i = 0; i < sindv_pp->scr_tap_memory_cnt; i++) {
        if (sindv_pp->scr_tap_memory[i].onKey) {
            follow_scr_tap_memory[follow_scr_tap_memory_cnt] = sindv_pp->scr_tap_memory[i];
            follow_scr_tap_memory_cnt++;
        }
    }
}

static SCR_TAP_MEMORY* followTapLoad(int pos, int time) {
    if (follow_scr_tap_memory_cnt <= pos) {
        return NULL;
    }
    if (follow_scr_tap_memory[pos].ofs_frame > time) {
        return NULL;
    }
    if (follow_scr_tap_memory_cnt_load != pos) {
        return NULL;
    }

    follow_scr_tap_memory_cnt_load = pos + 1;
    return &follow_scr_tap_memory[pos];
}

static void ScrLincChangTbl(int line) {
    scrJimakuLine = line;
    scrDrawLine   = line;
    scrMbarLine   = line;

    DrawCtrlTblChange(GetDrawLine(line));
}

static void ScrLincChangTblRef(int line, int ck_time) {
    scrDrawLine |= 0x8000;
    scrMbarLine |= 0x8000;
    scrRefLineTime = ck_time;

    DrawCtrlTblChange(GetDrawLine(line));
}

void ScrLineSafeRefMode(void) {
    if (scrDrawLine & 0x8000) {
        scrDrawLine &= ~0x8000;
        DrawCtrlTblChange(GetDrawLine(scrDrawLine));
    }

    if (scrMbarLine & 0x8000) {
        scrMbarLine &= ~0x8000;
    }
}

int ScrDrawTimeGet(int line) {
    if (line & 0x8000) {
        return scrRefLineTime;
    }

    return score_str.stdat_dat_pp->scr_pp->scr_ctrl_pp[line].lineTime;
}

int ScrDrawTimeGetFrame(int line) {
    if (line & 0x8000) {
        return (scrRefLineTime * 3600.0f + GetLineTempo(line) * 96.0f * 0.5f) / (GetLineTempo(line) * 96.0f);
    }

    return score_str.stdat_dat_pp->scr_pp->scr_ctrl_pp[line].lineTimeFrame;
}

void KeyCntClear(int *key_pp) {
    int i;

    for (i = 0; i < 7; i++) {
        *key_pp++ = -1;
    }
}

SCRREC* ScrCtrlCurrentSearch(SCORE_INDV_STR *sindv_pp, int index, int frame) {
    SCRREC *scrrec_pp;

    if (score_str.stdat_dat_pp->scr_pp->scr_ctrl_num <= index) {
        printf("ScrCtrlCurrentSearch index[%d] is over!!\n", index);
        return NULL;
    }

    scrrec_pp = sindv_pp->top_scr_ctrlpp[index].scrrec_pp;
    if (scrrec_pp == NULL) {
        printf("ScrCtrlCurrentSearch index[%d] is NULL line!!\n", index);
        return NULL;
    }

    while (1) {
        if (scrrec_pp->job == SCRRJ_ENDJOB || scrrec_pp->job == SCRRJ_ENDGAME) {
            break;
        }
        if (scrrec_pp->job == SCRRJ_PLY && scrrec_pp->data >= frame) {
            break;
        }

        scrrec_pp++;
    }

    return scrrec_pp;
}

void ScrCtrlIndvInit(STDAT_DAT *sdat_pp) {
    int             i, j;
    int             dare;

    SCORE_INDV_STR *sindv_pp;
    GLOBAL_PLY     *gply_pp;

    WorkClear(score_indv_str, sizeof(score_indv_str));
    sindv_pp = score_indv_str;

    for (i = 0; i < PR_ARRAYSIZE(score_indv_str); i++, sindv_pp++) {
        dare = -1;
        gply_pp = global_data.global_ply;

        for (j = 0; j < PR_ARRAYSIZE(global_data.global_ply); j++) {
            if (gply_pp[j].player_code == PR_BIT(i)) {
                dare = j;
                break;
            }
        }

        if (dare >= 0) {
            KeyCntClear(sindv_pp->keyCnt);
            tapReqGroupTapClear(Pcode2Pindex(sindv_pp->plycode));

            sindv_pp->keyCntCom = 0;
            sindv_pp->status = SCS_USE;
            sindv_pp->plycode = PR_BIT(i);
            sindv_pp->global_ply = &gply_pp[dare];
            sindv_pp->top_scr_ctrlpp = sdat_pp->scr_pp->scr_ctrl_pp;

            sindv_pp->current_scrrec_pp = ScrCtrlCurrentSearch(sindv_pp, global_data.draw_tbl_top, 0);
            sindv_pp->useLine = global_data.draw_tbl_top;

            sindv_pp->global_ply->exam_tbl_up = 0;
            sindv_pp->global_ply->exam_tbl_dw = 0;

            exam_tbl_updownInit(sindv_pp);

            for (j = 0; j < PR_ARRAYSIZE(sindv_pp->sjob); j++) {
                sindv_pp->sjob[j] = -1;
            }

            sindv_pp->tapset_pos = -1;
        }
    }
}

void ScrCtrlExamClear(SCR_EXAM_STR *sexam_pp) {
    int i;

    sexam_pp->exam_enum  = EXAM_NONE;
    sexam_pp->exam_start = -1;
    sexam_pp->exam_do    = EXAM_DO_NON;

    for (i = 0; i < PR_ARRAYSIZE(sexam_pp->scr_exam_job); i++) {
        sexam_pp->scr_exam_job[i].goto_time = -1;
        sexam_pp->scr_exam_job[i].goto_line = SCRLINE_NODATA;
        sexam_pp->scr_exam_job[i].goto_job_time = -1;
        sexam_pp->scr_exam_job[i].goto_job = -1;
    }
}

void ScrCtrlExamClearIndv(SCR_EXAM_STR *sexam_pp) {
    sexam_pp->exam_do = EXAM_DO_NON;
    sexam_pp->exam_start = -1;
}

int ScrCtrlIndvNextTime(SCORE_INDV_STR *sindv_pp, int Ncnt) {
    SCRREC *cur_pp = sindv_pp->current_scrrec_pp;

    while (1) {
        if (cur_pp->job == SCRRJ_ENDJOB) {
            return cur_pp->data;
        }
        if (cur_pp->job == SCRRJ_ENDGAME) {
            return cur_pp->data;
        }

        if (cur_pp->job == SCRRJ_PLY) {
            if (--Ncnt == 0) {
                return cur_pp->data;
            }
        }

        cur_pp++;
    }
}

int ScrCtrlIndvNextReadLine(SCORE_INDV_STR *sindv_pp, int ckf) {
    int tmp_tapstr_pos, tmp_tapset_pos;
    int cktime;

    if (sindv_pp->scrdat_pp == NULL) {
        return 0;
    }

    tmp_tapstr_pos = global_data.tapLevel;
    tmp_tapset_pos = sindv_pp->tapset_pos;

    cktime = sindv_pp->scrdat_pp->tapstr[tmp_tapstr_pos].tapset_pp[tmp_tapset_pos].taptimeStart;

    while (1) {
        tmp_tapset_pos++;

        if (tmp_tapset_pos >= sindv_pp->scrdat_pp->tapstr[tmp_tapstr_pos].tapset_size) {
            if (ckf == 0) {
                sindv_pp->tapset_pos = -1;
            }

            return 1;
        }

        if (ckf == 2) {
            if (sindv_pp->scrdat_pp->tapstr[tmp_tapstr_pos].tapset_pp[tmp_tapset_pos].taptimeStart >= cktime) {
                return 0;
            }
        } else {
            if (sindv_pp->scrdat_pp->tapstr[tmp_tapstr_pos].tapset_pp[tmp_tapset_pos].player_code == sindv_pp->plycode) {
                if (ckf == 0) {
                    sindv_pp->tapset_pos = tmp_tapset_pos;
                }

                return 0;
            }
        }
    }
}

int getLvlTblRand(TAPLVL_DAT *taplvl_dat_pp) {
    int rand_tmp;
    int ret;
    int i;
    int check;

    ret = 0;
    rand_tmp = randMakeMax(100);
    check = 0;

    for (i = 0; i < 17; i++, ret++) {
        check += taplvl_dat_pp->per[i];
        if (check > rand_tmp) {
            break;
        }
    }

    return ret;
}

int tapLevelChangeSub(void) {
    int         add_move;
    TAPLVL_STR *taplvl_str_pp;
    int         lvl_num;

    if (global_data.tapLevelCtrl != LM_AUTO) {
        return global_data.tapLevel;
    }
    if (score_str.stdat_dat_pp->taplvl_str_pp == NULL) {
        return global_data.tapLevel;
    }

    lvl_num = global_data.roundL;
    add_move = global_data.tap_ctrl_level;

    if (global_data.play_table_modeL == PLAY_TABLE_EASY) {
        lvl_num = lvl_num + TRND_MAX;
    }

    taplvl_str_pp = &score_str.stdat_dat_pp->taplvl_str_pp[lvl_num];
    add_move = getLvlTblRand(&taplvl_str_pp->taplvl_dat[add_move]);

    global_data.tapLevel = add_move;
    return global_data.tapLevel;
}

void tapLevelChange(SCORE_INDV_STR *sindv_pp) {
    int add_move;
    int old_num;

    int tmp_lv;
    int tmp_hklv;

    if (!(sindv_pp->global_ply->flags & GPLAY_TBLCNG_REQ)) {
        return;
    }

    tmp_lv = exam_tbl_updownChange(sindv_pp, global_data.tap_ctrl_level, global_data.roundL, (RANK_LEVEL2DISP_LEVEL(sindv_pp->global_ply->rank_level) == DLVL_COOL));

    sindv_pp->global_ply->exam_tbl_up = 0;
    sindv_pp->global_ply->exam_tbl_dw = 0;

    exam_tbl_updownInit(sindv_pp);

    printf("----- LEVEL CHANGE ----\n");
    printf(" CTRL LEVEL before[%d]\n", global_data.tap_ctrl_level);

    add_move = tmp_lv + global_data.tap_ctrl_level;

    if (add_move < 0) {
        add_move = TCT_LV00;
    }
    if (add_move > 15) {
        add_move = TCT_LV15;
    }

    global_data.tap_ctrl_level = add_move;

    printf("            after [%d]\n\n", add_move);
    printf(" PLAY LEVEL before[%d]\n", global_data.tapLevel);

    old_num  = global_data.tapLevel;
    add_move = tapLevelChangeSub();

    if (global_data.demo_flagL != DEMOF_REPLAY) {
        if (global_data.tapLevelCtrl == LM_AUTO) {
            if (sindv_pp->scrdat_pp != NULL) {
                tmp_hklv = inCmnHook2GameCheck(sindv_pp->scrdat_pp->sndrec_num);
                if (tmp_hklv >= 0) {
                    add_move = tmp_hklv;
                    global_data.tapLevel = tmp_hklv;
                }
            }
        }

        /* Stage 8 specific logic */
        if (global_data.play_step == PSTEP_GAME && global_data.play_stageL == 8) {
            if (sindv_pp->scrdat_pp != NULL) {
                if (sindv_pp->scrdat_pp->sndrec_num > 23 &&
                    sindv_pp->scrdat_pp->sndrec_num < 27) {
                    add_move = old_num;
                    printf("st8 sp same num:%d sndLine:%d\n", old_num, sindv_pp->scrdat_pp->sndrec_num);
                }
            } else {
                add_move = old_num;
                printf("st8 sp same num:%d\n", old_num);
            }
        }

        mccReqLvlSet(add_move);
    } else {
        add_move = mccReqLvlGet();
    }

    global_data.tapLevel = add_move;

    if (old_num != add_move) {
        selectIndvTapResetPlay(1);
    }

    printf("            after [%d]\n\n", add_move);
}

INCLUDE_ASM("asm/nonmatchings/main/scrctrl", ScrCtrlIndvNextRead);
#if 0
void ScrCtrlIndvNextRead(/* s0 16 */ SCORE_INDV_STR *sindv_pp, /* a1 5 */ int tap_res_f)
{
    int i;
    /* s2 18 */ int endf;
    /* v1 3 */ int j;
    /* a3 7 */ short Rsub;
    /* t0 8 */ int Rdata;
    /* t1 9 */ int sj_data1;
    /* t2 10 */ int sj_data2;
    /* a0 4 */ int xx;
    /* t4 12 */ int yari_ff;

    SCRREC *scrrec_pp;

    sindv_pp->keyCntCom = 0;
    sindv_pp->scr_tap_memory_cnt = 0;
    sindv_pp->scr_tap_vib_on = 0;
    sindv_pp->cansel_flag = 0;

    if (tap_res_f)
    {
        KeyCntClear(sindv_pp->keyCnt);
        tapReqGroupTapClear(Pcode2Pindex(sindv_pp->plycode));
    }

    ScrCtrlExamClearIndv(&sindv_pp->scr_exam_str);

    if ((sindv_pp->status & 1) == 0)
    {
        sindv_pp->scrdat_pp = NULL;
        sindv_pp->sndId = -1;

        for (i = 0; i < 24; i++)
        {
            sindv_pp->sjob[i] = -1;
        }

        ScrCtrlExamClear(&sindv_pp->scr_exam_str);
        return;
    }

    if (sindv_pp->tapset_pos == -1 || ScrCtrlIndvNextReadLine(sindv_pp, 0))
    {
        ScrCtrlExamClear(&sindv_pp->scr_exam_str);

        scrrec_pp = sindv_pp->current_scrrec_pp;
        sindv_pp->scrdat_pp = NULL;
        sindv_pp->sndId = -1;

        for (i = 0; i < 24; i++)
        {
            sindv_pp->sjob[i] = -1;
        }

        while (1)
        {
            if (scrrec_pp->job == 7)
                break;

            if (scrrec_pp->job != 0)
            {
                if (scrrec_pp->sub & sindv_pp->plycode)
                {
                    sindv_pp->current_time = scrrec_pp->data;
                    sindv_pp->cursor_num = scrrec_pp->jobd1;
                    sindv_pp->tap_follow_enum = TAP_FOLLOW_NONE;

                    endf = 0;
                    
                    scrrec_pp++;

                    while (1)
                    {
                        Rsub = scrrec_pp->sub;
                        Rdata = scrrec_pp->data;
                        sj_data1 = scrrec_pp->jobd1;
                        sj_data2 = scrrec_pp->jobd2;

                        switch (scrrec_pp->job)
                        {
                        case 0:
                        case 7:
                        case 8:
                            endf = 1;
                            break;
                        case 2:
                            sindv_pp->scr_exam_str.vsPlayer = Rdata;
                            sindv_pp->scr_exam_str.exam_enum = Rsub;
                            sindv_pp->scr_exam_str.exam_start = 0;
                            break;
                        case 3:
                            sindv_pp->scr_exam_str.scr_exam_job[0].goto_time = Rdata;
                            sindv_pp->scr_exam_str.scr_exam_job[0].goto_line = Rsub;
                            break;
                        case 4:
                            sindv_pp->scr_exam_str.scr_exam_job[0].goto_job_time = Rdata;
                            sindv_pp->scr_exam_str.scr_exam_job[0].goto_job = Rsub;
                            break;
                        case 5:
                            sindv_pp->scr_exam_str.scr_exam_job[1].goto_time = Rdata;
                            sindv_pp->scr_exam_str.scr_exam_job[1].goto_line = Rsub;
                            break;
                        case 6:
                            sindv_pp->scr_exam_str.scr_exam_job[1].goto_job_time = Rdata;
                            sindv_pp->scr_exam_str.scr_exam_job[1].goto_job = Rsub;
                            break;
                        case 9:
                            if (Rsub == 9)
                            {
                                yari_ff = 0;

                                for (j = 0; j < 4; i++)
                                {
                                    if (sindv_pp->sjob[j] != -1)
                                    {
                                        sindv_pp->sjob_data[j][0] = sj_data1;
                                        sindv_pp->sjob_data[j][1] = sj_data2;
                                        yari_ff = 1;
                                    }
                                }

                                if (!yari_ff)
                                    printf("ERROR! SUBJOB_NO_ID OVER!\n");
                            }
                            else
                            {
                                sindv_pp->sjob[Rsub] = Rdata;
                                sindv_pp->sjob_data[Rsub][0] = sj_data1;
                                sindv_pp->sjob_data[Rsub][1] = sj_data2;
                            }
                            break;
                        case 0xB:
                            sindv_pp->scr_exam_str.scr_exam_job[2].goto_line = sj_data1;
                            sindv_pp->scr_exam_str.scr_exam_job[2].goto_job_time = Rdata;
                            sindv_pp->scr_exam_str.scr_exam_job[2].goto_job = Rsub;
                            break;
                        case 0xC:
                            sindv_pp->scr_exam_str.scr_exam_job[3].goto_line = sj_data1;
                            sindv_pp->scr_exam_str.scr_exam_job[3].goto_job_time = Rdata;
                            sindv_pp->scr_exam_str.scr_exam_job[3].goto_job = Rsub;
                            break;
                        case 0xD:
                            sindv_pp->scr_exam_str.scr_exam_job[sj_data2].goto_line = Rsub;
                            sindv_pp->scr_exam_str.scr_exam_job[sj_data2].goto_time = Rdata;
                            break;
                        case 0xE:
                            sindv_pp->scr_exam_str.scr_exam_job[sj_data2].goto_job = Rsub;
                            sindv_pp->scr_exam_str.scr_exam_job[sj_data2].goto_job_time = Rdata;
                            break;
                        default:
                            break;
                        }

                        if (endf)
                        {
                            if (sindv_pp->global_ply == NULL)
                                printf("global_play is NULL [%d]\n", sindv_pp->plycode);
                            else
                                tapLevelChange(sindv_pp);

                            ScrCtrlIndvNextReadLine(sindv_pp, 0);
                            return;
                        }

                        scrrec_pp++;
                    }
                }

                scrrec_pp++;
            }
        }

        sindv_pp->status |= 8;
        sindv_pp->current_time = scrrec_pp->data;
    }
}
#endif

void intIndvStatusSet(SCORE_INDV_STR *sindv_pp, u_int CKF, u_int STF, u_int UNF) {
    if (sindv_pp->status & CKF) {
        sindv_pp->status |= STF;
        sindv_pp->status &= ~UNF;
    }
}

void allIndvNextContinue(void) {
    int             i;
    SCORE_INDV_STR *sindv_pp = score_indv_str;

    for (i = 0; i < PR_ARRAYSIZE(score_indv_str); i++, sindv_pp++) {
        intIndvStatusSet(sindv_pp, SCS_PAUSE, SCS_PAUSE_END, SCS_PAUSE);
    }
}

void allIndvGoContinue(void) {
    int             i;
    SCORE_INDV_STR *sindv_pp = score_indv_str;

    for (i = 0; i < PR_ARRAYSIZE(score_indv_str); i++, sindv_pp++) {
        intIndvStatusSet(sindv_pp, SCS_PAUSE_END, 0, SCS_PAUSE_END);
    }
}

void otherIndvPause(int num) {
    int             i;
    SCORE_INDV_STR *sindv_pp = score_indv_str;

    for (i = 0; i < PR_ARRAYSIZE(score_indv_str); i++, sindv_pp++) {
        if (i != num) {
            intIndvStatusSet(sindv_pp, SCS_USE, SCS_PAUSE, 0);
        }
    }
}

void otherIndvTapReset(int num) {
    int             i;
    SCORE_INDV_STR *sindv_pp = score_indv_str;

    for (i = 0; i < PR_ARRAYSIZE(score_indv_str); i++, sindv_pp++) {
        if (i != num) {
            if (sindv_pp->status & SCS_USE) {
                KeyCntClear(sindv_pp->keyCnt);
                    
                sindv_pp->keyCntCom = 0;
                sindv_pp->scr_tap_memory_cnt = 0;
                sindv_pp->scr_tap_vib_on = 0;
                sindv_pp->cansel_flag = 0;
                    
                tapReqGroupTapClear(Pcode2Pindex(sindv_pp->plycode));
            }
        }
    }
}

void selectIndvTapResetPlay(int num) {
    SCORE_INDV_STR *sindv_pp = &score_indv_str[num];
    TAPSET         *tapset_pp;

    if (!(sindv_pp->status & SCS_USE)) {
        return;
    }

    tapset_pp = IndvGetTapSetAdrs(sindv_pp);
    if (tapset_pp == NULL || tapset_pp->coolup == -1) {
        return;
    }

    KeyCntClear(sindv_pp->keyCnt);

    sindv_pp->keyCntCom = 0;
    sindv_pp->scr_tap_memory_cnt = 0;
    sindv_pp->scr_tap_vib_on = 0;
    sindv_pp->cansel_flag = 0;

    tapReqGroupTapClear(Pcode2Pindex(sindv_pp->plycode));
}

void IndivMoveChange(SCORE_INDV_STR *sindv_pp, int goto_time, SCRLINE_ENUM goto_line) {
    int j;

    KeyCntClear(sindv_pp->keyCnt);
    tapReqGroupTapClear(Pcode2Pindex(sindv_pp->plycode));

    sindv_pp->top_scr_ctrlpp = score_str.stdat_dat_pp->scr_pp->scr_ctrl_pp;
    sindv_pp->current_scrrec_pp = ScrCtrlCurrentSearch(sindv_pp, goto_line, goto_time);

    for (j = 0; j < PR_ARRAYSIZE(sindv_pp->sjob); j++) {
        sindv_pp->sjob[j] = -1;
    }

    sindv_pp->useLine = goto_line;
    sindv_pp->tapset_pos = -1;

    ScrCtrlIndvNextRead(sindv_pp, 1);
}

void useIndevAllMove(int goto_time, SCRLINE_ENUM goto_line) {
    int             i;
    SCORE_INDV_STR *sindv_pp = score_indv_str;

    for (i = 0; i < PR_ARRAYSIZE(score_indv_str); i++, sindv_pp++) {
        if (sindv_pp->status & SCS_USE) {
            sindv_pp->plycode = PR_BIT(i);
            sindv_pp->status  = SCS_USE;

            IndivMoveChange(sindv_pp, goto_time, goto_line);
        }
    }
}

static int useIndevCodeGet(void) {
    int             i;
    int             ret      = 0;
    SCORE_INDV_STR *sindv_pp = score_indv_str;

    for (i = 0; i < PR_ARRAYSIZE(score_indv_str); i++, sindv_pp++) {
        if (sindv_pp->status & SCS_USE) {
            ret |= PR_BIT(i);
        }
    }

    return ret;
}

static int targetTimeGet(int line, int time, int codeAll) {
    SCRREC *scrrec_pp;
    int     max;
    int     i;
    int     ret;
    int     pcode_tmp;

    ret       = 0;
    pcode_tmp = 0;

    max       = score_str.stdat_dat_pp->scr_pp->scr_ctrl_pp[line].scrrec_num;
    scrrec_pp = score_str.stdat_dat_pp->scr_pp->scr_ctrl_pp[line].scrrec_pp;

    for (i = 0; i < max; i++, scrrec_pp++) {
        if (scrrec_pp->job == SCRRJ_ENDJOB || scrrec_pp->job == SCRRJ_ENDGAME) {
            break;
        }

        if (scrrec_pp->job == SCRRJ_PLY) {
            pcode_tmp = scrrec_pp->sub & codeAll;
            if (scrrec_pp->data < time) {
                pcode_tmp = 0;
            }
        }

        if (scrrec_pp->job == SCRRJ_SUBJOB && scrrec_pp->sub == 2) {
            if (pcode_tmp != 0) {
                ret = scrrec_pp->data;
                break;
            }
        }
    }

    return ret;
}

void useIndevSndKill(void) {
    int             i;
    SCORE_INDV_STR *sindv_pp = score_indv_str;

    for (i = 0; i < PR_ARRAYSIZE(score_indv_str); i++, sindv_pp++) {
        if (sindv_pp->status & SCS_USE) {
            TapCt(0xe0, i, 0);
        }
    }
}

void useAllClearKeySnd(void) {
    int i;

    for (i = 0; i < 12; i++) {
        TapCt(0xe0, i, 0);
    }
}

int useIndevSndKillOther(int num) {
    int             i;
    SCORE_INDV_STR *sindv_pp = score_indv_str;

    for (i = 0; i < PR_ARRAYSIZE(score_indv_str); i++, sindv_pp++) {
        if (i != num) {
            if (sindv_pp->status & SCS_USE) {
                TapCt(0xe0, i, 0);
            }
        }
    }

    return 0;
}

int TapKeyCheckNum(TAPSET *tapset_pp, int keyId, int ng_f) {
    int     i;
    int     ret;

    int     tmp_size;
    TAPDAT *tmp_tapdat_pp;

    ret = 0;

    if (ng_f) {
        tmp_size      = tapset_pp->tapdatNG_size;
        tmp_tapdat_pp = tapset_pp->tapdatNG_pp;
    } else {
        tmp_size      = tapset_pp->tapdat_size;
        tmp_tapdat_pp = tapset_pp->tapdat_pp;
    }

    for (i = 0; i < tmp_size; i++) {
        if (tmp_tapdat_pp[i].KeyIndex == keyId) {
            ret++;
        }
    }

    return ret;
}

TAPDAT* TapKeyGetDatPP(TAPSET *tapset_pp, int keyId, int keyCnt, int ng_f, u_char *keyNumSave) {
    int     i;
    int     cnt;
    int     tmp_size;
    TAPDAT *tmp_tapdat_pp;

    cnt = 0;
    *keyNumSave = 0;

    if (ng_f) {
        tmp_size      = tapset_pp->tapdatNG_size;
        tmp_tapdat_pp = tapset_pp->tapdatNG_pp;
    } else {
        tmp_size      = tapset_pp->tapdat_size;
        tmp_tapdat_pp = tapset_pp->tapdat_pp;
    }

    for (i = 0; i < tmp_size; i++) {
        if (tmp_tapdat_pp[i].KeyIndex == keyId) {
            if (cnt == keyCnt) {
                *keyNumSave = i;
                return &tmp_tapdat_pp[i];
            }

            cnt++;
        }
    }

    return NULL;
}

void tapReqGroupInit(void) {
    WorkClear(tap_groupe_str, sizeof(tap_groupe_str));
}

void tapReqGroupTapClear(PLAYER_INDEX pindex) {
    WorkClear(&tap_groupe_str[pindex], sizeof(TAP_GROUPE_STR));
}

void tapReqGroup(TAPCT *tapct_pp, PLAYER_INDEX pindex, int sndId, u_char *tappress_pp) {
    if (pindex >= PINDEX_MAX) {
        printf("tap group PINDEX over[%d]\n", pindex);
    }

    tap_groupe_str[pindex].timer       = 0;
    tap_groupe_str[pindex].sndId       = sndId;
    tap_groupe_str[pindex].tapct_pp    = tapct_pp;
    tap_groupe_str[pindex].tappress_pp = tappress_pp;
}

void tapReqGroupPoll(void) {
    int             i, j;
    int             end_frameT;
    TAP_GROUPE_STR *tgs_pp = tap_groupe_str;

    for (i = 0; i < PR_ARRAYSIZE(tap_groupe_str); i++, tgs_pp++) {
        end_frameT = 0;

        if (tgs_pp->tapct_pp == NULL) {
            continue;
        }

        for (j = 0; j < 4; j++) {
            if (tgs_pp->tapct_pp[j].frame == -1) {
                continue;
            }

            if (tgs_pp->tapct_pp[j].frame == tgs_pp->timer) {
                if (tgs_pp->tapct_pp[j].actor != -1) {
                    DrawTapReqTbl(tgs_pp->tapct_pp[j].actor, i, tgs_pp->tappress_pp);
                }
                if (tgs_pp->tapct_pp[j].sound != -1) {
                    ScrTapReq(tgs_pp->sndId, i, tgs_pp->tapct_pp[j].sound);
                }
            } else {
                end_frameT = (tgs_pp->timer < tgs_pp->tapct_pp[j].frame) ? 1 : end_frameT;
            }
        }

        if (end_frameT) {
            tgs_pp->timer++;
        } else {
            WorkClear(tgs_pp, sizeof(*tgs_pp));
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/main/scrctrl", tapEventCheck);

static int otehon_all_make(EXAM_CHECK *ec_pp) {
    int i;
    int ret = 0;

    if (ec_pp->tapset_pp == NULL) {
        return 0;
    }

    for (i = 0; i < ec_pp->tapset_pp->tapdat_size; i++) {
        ret |= GetIndex2KeyCode(ec_pp->tapset_pp->tapdat_pp[i].KeyIndex);
    }

    return ret;
}

static int treateTimeChange(int time) {
    int thnum_ofs = thnum_get(24, CK_TH_NORMAL);
    int thnum_now = thnum_get((time + 96) / 4, CK_TH_NORMAL);

    if ((thnum_now % 2) != 0) {
        return -1;
    }
    if (thnum_now < thnum_ofs) {
        return -1;
    }

    thnum_now -= thnum_ofs;
    thnum_now /= 2;
    return thnum_now * 24;
}

static int thnum_get(int p96_num, CK_TH_ENUM ckth) {
    u_int thnum_data;
    int   ck_bit, ck_dat;
    int   ret_cnt;
    int   i;

    ck_bit = -1;
    ret_cnt = 0;
    thnum_data = thnum_tbl[ckth];

    for (i = 0; i <= p96_num; i++) {
        ck_dat = (thnum_data >> (23 - (i % 24))) & 1;

        if (ck_bit < 0) {
            if (ck_dat == 0) {
                ret_cnt = 1;
            }
        } else if (ck_dat != ck_bit) {
            ret_cnt++;
        }

        ck_bit = ck_dat;
    }

    return ret_cnt;
}

static int MapNormalNumGet(int time) {
    return thnum_get(time / 4, CK_TH_NORMAL);
}

INCLUDE_ASM("asm/nonmatchings/main/scrctrl", on_th_make);

/* TODO(poly): Make these static once .data is migrated */
/* static */ int exh_normal_add(EXAM_CHECK *ec_pp) {
    int i;
    int ret = 0;

    for (i = 0; i < ec_pp->ted_num; i++) {
        if (!(ec_pp->ted[i].th_num & 1)) {
            if (GetIndex2KeyCode(ec_pp->ted[i].key) & ec_pp->otehon_all) {
                ret++;
            }
        }
    }

    return ret;
}

/* static */ int exh_normal_sub(EXAM_CHECK *ec_pp) {
    int i;
    int ret = 0;

    for (i = 0; i < ec_pp->ted_num; i++) {
        if (GetIndex2KeyCode(ec_pp->ted[i].key) & ec_pp->otehon_all) {
            ret -= ec_pp->ted[i].th_num & 1;
        }
    }

    return ret;
}

INCLUDE_ASM("asm/nonmatchings/main/scrctrl", exh_nombar_sub);
#if 0
static int exh_nombar_sub(/* s2 18 */ EXAM_CHECK *ec_pp)
{
    /* s1 17 */ int i;
    /* s0 16 */ int ret = 0;
    /* a0 4 */  int bai;

    for (i = 0; i < ec_pp->ted_num; i++)
    {
        if ((GetIndex2KeyCode(ec_pp->ted[i].key) & ec_pp->otehon_all) == 0)
            ret--;
    }

    bai = 0;

    for (i = 0; i < 4U; i++)
    {
        bai += (ec_pp->otehon_all >> i) & 1;
    }

    if (bai == 1)
        ret *= 3;
    if (bai == 2)
        ret *= 2;
    if (bai == 5)
        ret /= 2;
    if (bai == 6)
        ret /= 2;

    return ret;
}
#endif

/* static */ int exh_mbar_key_out(EXAM_CHECK *ec_pp) {
    int ret;

    if (global_data.play_typeL == PLAY_TYPE_ONE) {
        return 0;
    }

    ret = -ec_pp->oth_num;
    if (ec_pp->ted_num != 0) {
        if (ec_pp->oth_num == 0) {
            ret = 0;
        } else if (ec_pp->oth[0].key == ec_pp->ted[0].key) {
            ret = 0;
        }
    }

    return ret;
}

/* static */ int exh_mbar_time_out(EXAM_CHECK *ec_pp) {
    int ret;

    ret = -ec_pp->oth_num;
    if (ec_pp->ted_num != 0) {
        if (ec_pp->oth_num == 0) {
            ret = 0;
        } else if (ec_pp->oth[0].th_num == ec_pp->ted[0].th_num) {
            ret = 0;
        }
    }

    return ret;
}

/* static */ int exh_mbar_num_out(EXAM_CHECK *ec_pp) {
    int ret = ec_pp->ted_num - ec_pp->oth_num;
    return (ret >= 0) ? -ret : ret;
}

INCLUDE_ASM("asm/nonmatchings/main/scrctrl", exh_yaku);

/* static */ int exh_yaku_original(EXAM_CHECK *ec_pp) {
    return exh_yaku(ec_pp, 0);
}

/* static */ int exh_yaku_hane(EXAM_CHECK *ec_pp) {
    return exh_yaku(ec_pp, 1);
}

INCLUDE_ASM("asm/nonmatchings/main/scrctrl", exh_allkey_out);

/* static */ int exh_allkey_out_nh(EXAM_CHECK *ec_pp) {
    int use_bit;
    int i;

    for (i = 0, use_bit = 0; i < ec_pp->ted_num; i++) {
        use_bit |= GetIndex2KeyCode(ec_pp->ted[i].key);
    }

    use_bit &= ec_pp->otehon_all;

    if (use_bit == ec_pp->otehon_all) {
        return 0;
    } else {
        return -1;
    }
}

/* static */ int exh_command(EXAM_CHECK *ec_pp) {
    return 0;
}

/* static */ int exh_renda_out(EXAM_CHECK *ec_pp) {
    int renda_ck;

    renda_ck = ec_pp->tapset_pp->taptimeEnd - ec_pp->tapset_pp->taptimeStart;
    renda_ck = ((renda_ck + 23) / 24);
    renda_ck += 3;

    if (ec_pp->ted_num >= renda_ck) {
        return -1;
    }

    return 0;
}

INCLUDE_ASM("asm/nonmatchings/main/scrctrl", manemane_check_sub);

INCLUDE_ASM("asm/nonmatchings/main/scrctrl", manemane_check);

/* static */ int exh_mane(EXAM_CHECK *ec_pp) {
    int normal_point;
    int late_point;

    normal_point = manemane_check_sub(ec_pp);

    on_th_make(ec_pp, CK_TH_LATE);

    late_point = manemane_check_sub(ec_pp);

    if (late_point < normal_point) {
        late_point = normal_point;
    }

    return late_point;
}

/* static */ int exh_all_add(EXAM_CHECK *ec_pp) {
    int i;
    int total;

    total = 0;

    for (i = 0; i < 12; i++) {
        if (i != EXH_TOTAL) {
            total += ec_pp->each_point[i];
        }
    }

    if (ec_pp->each_point[EXH_ALLKEY_OUT] != 0) {
        total = 0;
    }
    if (ec_pp->each_point[EXH_RENDA_OUT] != 0) {
        total = -100;
    }

    return total;
}

INCLUDE_ASM("asm/nonmatchings/main/scrctrl", IndvGetTapSetAdrs);

INCLUDE_ASM("asm/nonmatchings/main/scrctrl", nextExamTime);

static SCORE_INDV_STR* GetSindvPcodeLine(PLAYER_CODE pcode) {
    SCORE_INDV_STR *sindv_pp;
    int             i;

    sindv_pp = score_indv_str;

    for (i = 0; i < PR_ARRAYSIZEU(score_indv_str); i++, sindv_pp++) {
        if (sindv_pp->status & SCS_USE) {
            if (sindv_pp->plycode == pcode) {
                return sindv_pp;
            }
        }
    }

    return NULL;
}

INCLUDE_RODATA("asm/nonmatchings/main/scrctrl", D_00392D30);

INCLUDE_ASM("asm/nonmatchings/main/scrctrl", ExamScoreCheck);

static int ExamScoreCheckSame(SCORE_INDV_STR *sindv_pp) {
    SCORE_INDV_STR  sindv_tmp;
    GLOBAL_PLY      global_ply_tmp;
    int             i;
    TAPSET         *tapset_pp;
    SCR_TAP_MEMORY *scr_tap_memory_pp;
    TAPDAT         *tapdat_pp;
    int             tapdat_cnt;

    if (sindv_pp->global_ply->rank_level == RLVL_COOL ||
        sindv_pp->global_ply->rank_level == RLVL_COOL_GOOD) {
        return 150;
    }

    global_ply_tmp = *sindv_pp->global_ply;

    sindv_tmp      = *sindv_pp;
    sindv_tmp.global_ply = &global_ply_tmp;

    tapset_pp = IndvGetTapSetAdrs(sindv_pp);

    sindv_tmp.scr_tap_memory_cnt = 0;
    scr_tap_memory_pp = sindv_tmp.scr_tap_memory;

    tapdat_pp = tapset_pp->tapdat_pp;    
    tapdat_cnt = tapset_pp->tapdat_size;

    if (tapset_pp->tapscode == TAPSCODE_ANSWER) {
        tapdat_pp = vs_tapdat_tmp;
        tapdat_cnt = vs_tapdat_tmp_cnt;
    }

    for (i = 0; i < tapdat_cnt; i++, tapdat_pp++) {
        if (tapdat_pp->KeyIndex != 0) {
            if (global_data.play_typeL == PLAY_TYPE_ONE) {
                scr_tap_memory_pp->key = KiTR;
            } else {
                scr_tap_memory_pp->key = tapdat_pp->KeyIndex;
            }

            scr_tap_memory_pp->onKey = 1;
            scr_tap_memory_pp->ofs_frame = tapdat_pp->time;

            sindv_tmp.scr_tap_memory_cnt++;
            scr_tap_memory_pp++;
        }
    }

    ExamScoreCheck(&sindv_tmp);
    return global_ply_tmp.now_score;
}

static int levelChangeCheck(RANK_LEVEL lvl0, RANK_LEVEL lvl1) {
    int lvl0_tmp = RANK_LEVEL2DISP_LEVEL_HK(lvl0);
    int lvl1_tmp = RANK_LEVEL2DISP_LEVEL_HK(lvl1);

    if (lvl0_tmp != lvl1_tmp) {
        return (lvl0_tmp > lvl1_tmp);
    }

    return -1;
}

static int levelUpRank(RANK_LEVEL lvl) {
    RANK_LEVEL up_tbl[17] = {
        /* COOL      -> COOL      */ RLVL_COOL,
        /* COOL/GOOD -> COOL      */ RLVL_COOL,
        /* GOOD/COOL -> COOL      */ RLVL_COOL,
        /* GOOD      -> GOOD/COOL */ RLVL_GOOD_COOL,
        /* GOOD/BAD  -> GOOD      */ RLVL_GOOD,
        /* BAD/GOOD  -> GOOD      */ RLVL_GOOD,
        /* BAD       -> BAD/GOOD  */ RLVL_BAD_GOOD,
        /* BAD/AWFUL -> BAD       */ RLVL_BAD,
        /* AWFUL/BAD -> BAD       */ RLVL_BAD,
        /* AWFUL     -> AWFUL/BAD */ RLVL_AWFUL_BAD,
        /* AWFUL/END -> AWFUL     */ RLVL_AWFUL,
        /* END0      -> AWFUL     */ RLVL_AWFUL,
        /* END1      -> END0      */ RLVL_END0,
        /* END2      -> END1      */ RLVL_END1,
        /* HK_END0   -> END1      */ RLVL_END1,
        /* HK_END1   -> END1      */ RLVL_END1,
        /* HK_END2   -> END1      */ RLVL_END1,
    };

    return up_tbl[lvl];
}

static int levelDownRank(RANK_LEVEL lvl) {
    RANK_LEVEL down_tbl[17] = {
        /* COOL      -> COOL/GOOD */ RLVL_COOL_GOOD,
        /* COOL/GOOD -> GOOD      */ RLVL_GOOD,
        /* GOOD/COOL -> GOOD      */ RLVL_GOOD,
        /* GOOD      -> GOOD/BAD  */ RLVL_GOOD_BAD,
        /* GOOD/BAD  -> BAD       */ RLVL_BAD,
        /* BAD/GOOD  -> BAD       */ RLVL_BAD,
        /* BAD       -> BAD/AWFUL */ RLVL_BAD_AWFUL,
        /* BAD/AWFUL -> AWFUL     */ RLVL_AWFUL,
        /* AWFUL/BAD -> AWFUL     */ RLVL_AWFUL,
        /* AWFUL     -> AWFUL/END */ RLVL_AWFUL_END,
        /* AWFUL/END -> END1      */ RLVL_END1,
        /* END0      -> END1      */ RLVL_END1,
        /* END1      -> END2      */ RLVL_END2,
        /* END2      -> HK_END1   */ RLVL_HK_END1,
        /* HK_END0   -> HK_END1   */ RLVL_HK_END1,
        /* HK_END1   -> HK_END1   */ RLVL_HK_END1,
        /* HK_END2   -> HK_END1   */ RLVL_HK_END1,
    };

    return down_tbl[lvl];
}

INCLUDE_ASM("asm/nonmatchings/main/scrctrl", ScrMoveSetSub);
#if 0
void ScrMoveSetSub(SCORE_INDV_STR *sindv_pp, /* s0 16 */ int Pnum, /* s4 20 */ int sub_job, /* s1 17 */ int sub_time, /* s6 22 */ int goto_job, /* s7 23 */ int goto_time, /* fp 30 */ int start_move_line, /* s3 19 */ int start_move_time)
{
    /* s1 17 */ int ttype;
    /* s0 16 */ int tmp_cdsample;
    /* s2 18 */ int target_move_time;
    /* s0 16 */ SCORE_INDV_STR *sub_in_pp;
    /* -0xb0(sp) */ u_char chantmp[2];

    target_move_time = targetTimeGet(goto_job, goto_time, useIndevCodeGet());

    useIndevAllMove(goto_time, goto_job);
    useIndevSndKill();

    sindv_pp->wakeUpTime = sub_time;
    sindv_pp->wakeUpGoTime = goto_time;
    sindv_pp->wakeUpWaitLine = sub_job;
    sindv_pp->status |= SCS_WAIT;
    otherIndvPause(Pnum);

    sub_in_pp = score_indv_str + 4;
    sub_in_pp->status = 1;
    sub_in_pp->plycode = PCODE_MOVE;
    sub_in_pp->global_ply = NULL;

    IndivMoveChange(sub_in_pp, 0, sub_job);

    sub_in_pp->retStartLine = start_move_line;
    sub_in_pp->refStartTime = start_move_time;
    sub_in_pp->refTartegLine = goto_job;
    sub_in_pp->refTargetTime = target_move_time;

    if (GetTimeType(sub_job) != GTIME_VSYNC)
        GlobalTimeJobChange(FGF_CD);
    else
        GlobalTimeJobChange(FGF_VSYNC);

    TimeCallbackTimeSetChanTempo(sub_job, 0, GetLineTempo(sub_job));

    sub_in_pp->top_scr_ctrlpp[sub_job].lineTime = 0;
    sub_in_pp->top_scr_ctrlpp[sub_job].lineTimeFrame = 0;

    ScrLincChangTbl(sub_job);
    tapEventCheck(sub_in_pp, 0, 0, 4);

    ttype = GetTimeType(sub_job);
    if (ttype == GTIME_VSYNC)
    {
        if (GetTimeType(start_move_line) != GTIME_VSYNC)
            CdctrlWp2Stop();
    }
    else
    {
        printf("file seek\n");

        tmp_cdsample = CdctrlSndTime2WP2sample(GetLineTempo(sub_job), goto_time);
        tmp_cdsample -= (GetTimeOfset(goto_job) * 48) / 256;

        CheckIndvCdChannel(sindv_pp, chantmp);
        CdctrlWP2SetFileSeekChan(&score_str.stdat_dat_pp->sndfile[ttype], tmp_cdsample, chantmp[0], chantmp[1]);
    }
}
#endif

/* Big function! Decompiler discretion advised */
/*      https://decomp.me/scratch/jYNZx        */
INCLUDE_ASM("asm/nonmatchings/main/scrctrl", ScrExamSetCheck);

void subjobEvent(SCORE_INDV_STR *sindv_pp, int ctime_next) {
    int j;
    int cont_job;

    for (j = 0; j < PR_ARRAYSIZE(sindv_pp->sjob); j++) {
        cont_job = FALSE;

        if (sindv_pp->sjob[j] == -1 || ctime_next < sindv_pp->sjob[j]) {
            continue;
        }

        switch (j) {
        case SCRSUBJ_CDSND_ON:
            CdctrlWP2SetVolume(120);
            break;
        case SCRSUBJ_CDSND_OFF:
            CdctrlWP2SetVolume(0);
            break;
        case SCRSUBJ_DRAW_CHANGE:
            ScrLincChangTbl(sindv_pp->useLine);
            break;
        case SCRSUBJ_TAP_RESET:
            break;
        case SCRSUBJ_EFFECT:
            TapCt(0xb0, sindv_pp->sjob_data[j][0], sindv_pp->sjob_data[j][1]);
            break;
        case SCRSUBJ_REVERS: {
            int drline = sindv_pp->retStartLine;
            int time_tmp;
            int temp2 = (sindv_pp->sjob_data[j][0] / 2) < ctime_next;

            if (temp2) {
                drline = sindv_pp->refTartegLine;
            }

            temp2 = sindv_pp->refTargetTime - sindv_pp->refStartTime;
            time_tmp = temp2 * ctime_next;
            time_tmp /= sindv_pp->sjob_data[j][0];
            time_tmp = time_tmp != 0 ? time_tmp : 1;
            ScrLincChangTblRef(drline, time_tmp + sindv_pp->refStartTime);

            cont_job = TRUE;
            break;
        }
        case SCRSUBJ_SPU_ON:
        case SCRSUBJ_SPU_ON2:
        case SCRSUBJ_SPU_ON3:
        case SCRSUBJ_SPU_ON4:
            ScrTapReq(-1, sindv_pp->sjob_data[j][0], sindv_pp->sjob_data[j][1]);
            break;
        case SCRSUBJ_TITLE:
            if (sindv_pp->sjob_data[j][0] == 0) {
                if (sindv_pp->sjob_data[j][1] == 0) {
                    MendererCtrlTitle();
                    sindv_pp->sjob_data[j][1] = 1;
                }
                if (pad[0].one & SCE_PADstart) {
                    int next_time = ScrCtrlIndvNextTime(sindv_pp, 2);
                    TimeCallbackTimeSetChanTempo(sindv_pp->useLine, next_time, GetLineTempo(sindv_pp->useLine));
                }
            } else if (sindv_pp->sjob_data[j][0] == 2) {
                if (sindv_pp->sjob_data[j][1] == 0) {
                    MendererCtrlTitleDera();
                    sindv_pp->sjob_data[j][1] = 1;
                }
                if (pad[0].one & SCE_PADstart) {
                    int next_time = ScrCtrlIndvNextTime(sindv_pp, 2);
                    TimeCallbackTimeSetChanTempo(sindv_pp->useLine, next_time, GetLineTempo(sindv_pp->useLine));
                }
            } else if (pad[0].one & SCE_PADstart) {
                if (!titleStartKey) {
                    titleStartKey = TRUE;
                    ScrTapReq(-1, 0, 2);
                    DrawTapReqTbl(0xfe04, PINDEX_NONE, NULL);
                }
            }

            cont_job = TRUE;
            break;
        case SCRSUBJ_LOOP: {
            int next_time = sindv_pp->sjob_data[j][0];

            TimeCallbackTimeSetChanTempo(sindv_pp->useLine, next_time, GetLineTempo(sindv_pp->useLine));
            cont_job = TRUE;
            break;
        }
        case SCRSUBJ_FADEOUT:
            fadeoutStartKey = TRUE;
            cont_job = TRUE;
            break;
        case SCRSUBJ_ENDLOOP:
            gameEndWaitLoop = TRUE;
            break;
        case SCRSUBJ_SPUTRANS: {
            int *data = (int*)sindv_pp->sjob_data[j][0];
            if (*data != 0) {
                ScrTapDbuffSetSp(&score_str.stdat_dat_pp->scr_pp->sndrec_pp[*data], sindv_pp->sndId);
            }
            break;
        }
        case SCRSUBJ_STOP_MENDERER: {
            int next_time = ScrCtrlIndvNextTime(sindv_pp, 1) - sindv_pp->current_time;

            PrDecelerateMenderer(((next_time * 3600.0f) + (GetLineTempo(sindv_pp->useLine) * 96.0f * 0.5f)) / (GetLineTempo(sindv_pp->useLine) * 96.0f));
            printf("SCRSUBJ_STOP_MENDERER req\n");
            break;
        }
        case SCRSUBJ_BONUS_GAME:
            bonusGameCtrl(ctime_next);
            cont_job = TRUE;
            break;
        case SCRSUBJ_BONUS_GAME_END:
            bonusPointSave();
            bonusScoreDraw();
            break;
        case SCRSUBJ_LESSON: {
            int time_tmp;
            LessonRoundDisp(sindv_pp->sjob_data[j][0]);

            time_tmp = sindv_pp->sjob_data[j][1] + 1;
            sindv_pp->sjob_data[j][1] = time_tmp;

            cont_job = (time_tmp < 180);
            break;
        }
        case SCRSUBJ_VS_RESET: {
            SCORE_INDV_STR *cngSindv_pp;

            cngSindv_pp = GetSindvPcodeLine(PCODE_TEACHER);
            if (cngSindv_pp != NULL) {
                cngSindv_pp->global_ply->score = 500;
            }
            vsAnimationReset(1, 500);

            cngSindv_pp = GetSindvPcodeLine(PCODE_PARA);
            if (cngSindv_pp != NULL) {
                cngSindv_pp->global_ply->score = 500;
            }
            vsAnimationReset(0, 500);
            break;
        }
        case SCRSUBJ_CDSND_READY:
            CdctrlWP2Set(&score_str.stdat_dat_pp->sndfile[sindv_pp->sjob_data[j][0]]);
            break;
        case SCRSUBJ_CDSND_REQ:
            CdctrlWP2Play();
            CdctrlWP2SetVolume(sindv_pp->sjob_data[j][0]);
            break;
        case SCRSUBJ_SPU_OFF:
            ScrTapReqStop(sindv_pp->sjob_data[j][0]);
            break;
        case SCRSUBJ_JIMAKU_OFF:
            jimakuWakuOff = TRUE;
            break;
        }

        if (!cont_job) {
            sindv_pp->sjob[j] = -1;
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/main/scrctrl", ScrCtrlIndvJob);

static void ScrTimeRenew(SCR_MAIN *scr_main_pp) {
    int   i;
    int   samplecnt;
    float tempo;

    for (i = 0; i < scr_main_pp->scr_ctrl_num; i++) {
        if (scr_main_pp->scr_ctrl_pp[i].gtime_type == GTIME_VSYNC) {
            scr_main_pp->scr_ctrl_pp[i].lineTime =  ((TimeCallbackTimeGetChan(i) * 96.0f * GetLineTempo(i) + 1800.0f)  / 3600.0f);
            scr_main_pp->scr_ctrl_pp[i].lineTime += ((GetLineTempo(i) * 96.0f * scr_main_pp->scr_ctrl_pp[i].ofsCdtime) / 60000.0f);

            if (scr_main_pp->scr_ctrl_pp[i].lineTime < 0) {
                scr_main_pp->scr_ctrl_pp[i].lineTime = 0;
            }

            scr_main_pp->scr_ctrl_pp[i].lineTimeFrame = TimeCallbackTimeGetChan(i);
        } else {
            samplecnt = GlobalSndSampleGet() + ((scr_main_pp->scr_ctrl_pp[i].ofsCdtime * 48) / 256);

            if (global_data.play_step == PSTEP_XTR) {
                samplecnt = CdctrlWp2GetSampleTmp() - getTopSeekPos();
            }
            if (samplecnt < 0) {
                samplecnt = 0;
            }

            tempo = GetLineTempo(i);

            scr_main_pp->scr_ctrl_pp[i].lineTime = CdctrlWp2CdSample2SndTime(samplecnt, tempo);
            scr_main_pp->scr_ctrl_pp[i].lineTimeFrame = CdctrlWp2CdSample2Frame(samplecnt);
        }
    }
}

#ifndef NON_MATCHING
INCLUDE_ASM("asm/nonmatchings/main/scrctrl", ScrMbarReq);
#else
void ScrMbarReq(/* s5 21 */ int mbarTime)
{
    /* s6 22 */ int i;
    /* s2 18 */ SCORE_INDV_STR *sindv_pp;
    /* s1 17 */ TAPSET *tapset_pp;
    /* s0 16 */ int dare;
    /* s4 20 */ int tapdat_size;
    /* s3 19 */ TAPDAT *tapdat_pp;

    MbarSetCtrlTime(mbarTime);

    sindv_pp = score_indv_str;

    for (i = 0; i < 5; i++, sindv_pp++)
    {
        if (sindv_pp->status & 1)
        {
            if ((sindv_pp->status & 158) == 0)
            {
                tapset_pp = IndvGetTapSetAdrs(sindv_pp);
                if (tapset_pp != NULL)
                {
                    tapdat_size = tapset_pp->tapdat_size;
                    tapdat_pp = tapset_pp->tapdat_pp;

                    if (global_data.play_step == PSTEP_VS)
                    {
                        dare = MBAR_PARAPPA_VS;

                        if (sindv_pp->plycode == PCODE_TEACHER)
                            dare = MBAR_TEACHER_VS;
                        if (sindv_pp->plycode == PCODE_BOXY)
                        {
                            dare = MBAR_BOXY_VS;
                            if (mbarTime < sindv_pp->current_time + tapset_pp->taptimeEnd)
                                MbarReset();
                        }
                    }
                    else
                    {
                        if (global_data.play_step == PSTEP_HOOK)
                            dare = MBAR_PARAPPA_HOOK;
                        else
                            dare = MBAR_PARAPPA;
                        
                        if (sindv_pp->plycode != PCODE_PARA)
                            dare = MBAR_TEACHER;
                    }

                    if (tapset_pp->tapscode == TAPSCODE_ANSWER)
                    {
                        tapdat_pp = vs_tapdat_tmp;
                        tapdat_size = vs_tapdat_tmp_cnt;
                    }

                    if ((dare == MBAR_PARAPPA) || (dare == MBAR_PARAPPA_HOOK))
                    {
                        if (mbarTime < sindv_pp->current_time)
                            dare = MBAR_NONE;
                    }

                    MbarReq(dare, tapset_pp, sindv_pp->current_time, sindv_pp->scr_tap_memory, sindv_pp->scr_tap_memory_cnt, game_status.language_type, tapdat_size, tapdat_pp, sindv_pp->cursor_num);
                }
            }
        }
    }
}
#endif

void allTimeCallbackTimeSetChanTempo(int time) {
    int       i;
    SCR_MAIN *scr_main_pp = score_str.stdat_dat_pp->scr_pp;

    for (i = 0; i < scr_main_pp->scr_ctrl_num; i++) {
        if (scr_main_pp->scr_ctrl_pp[i].gtime_type == GTIME_VSYNC) {
            scr_main_pp->scr_ctrl_pp[i].lineTime = time;
            TimeCallbackTimeSetChanTempo(i, time, GetLineTempo(i));

            scr_main_pp->scr_ctrl_pp[i].lineTimeFrame = (time * 3600.0f + GetLineTempo(i) * 96.0f * 0.5f) / (GetLineTempo(i) * 96.0f);
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/main/scrctrl", SetIndvDrawTblLine);
#if 0
int SetIndvDrawTblLine(/* s0 16 */ SCORE_INDV_STR *sindv_pp)
{
    /* a1 5 */ TAPSET *tapset_pp;
    /* a2 6 */ int ctime;

    if ((sindv_pp->status & SCS_USE) && (sindv_pp->status & 158))
    {
        {
            if (sindv_pp->top_scr_ctrlpp[sindv_pp->useLine].gtime_type != GTIME_VSYNC)
            {
                tapset_pp = IndvGetTapSetAdrs(sindv_pp);
                ctime     = sindv_pp->top_scr_ctrlpp[sindv_pp->useLine].lineTime;

                if (tapset_pp != NULL)
                {
                    if (global_data.play_step != PSTEP_VS)
                    {
                        if (ctime < sindv_pp->current_time + tapset_pp->taptimeStart)
                            return 0;
                    }

                    if (ctime < sindv_pp->current_time + tapset_pp->taptimeEnd)
                        currentTblNumber = sindv_pp->scrdat_pp->drawofs[global_data.tapLevel];
                }
            }
        }
    }

    return 0;
}
#endif

INCLUDE_ASM("asm/nonmatchings/main/scrctrl", otehonSetCheck);
#if 0
static int otehonSetCheck(void)
{
    /* s0 16 */ SCORE_INDV_STR *sindv_pp = score_indv_str;
    /* v0 2 */ TAPSET *tapset_pp;
    /* s1 17 */ int i;

    for (i = 0; i < 5; i++)
    {
        if (sindv_pp[i].status & 1)
            return 0;

        tapset_pp = IndvGetTapSetAdrs(sindv_pp);

        if (tapset_pp != NULL)
        {
            if (sindv_pp[i].top_scr_ctrlpp[sindv_pp->useLine].lineTime >= sindv_pp[i].current_time)
                return 1;
        }
    }
}
#endif

/* TODO: Match .sdata */
extern const char D_00399478[];
extern const char D_00399480[];
extern const char D_00399488[];
extern const char D_00399490[];
extern const char D_00399498[];
extern const char D_003994A0[];
extern const char D_003994A8[];
extern const char D_003994B0[];
extern const char D_003994B8[];
extern const char D_003994C0[];
extern const char D_003994C8[];
extern const char D_003994D0[];
extern const char D_003994D8[];
extern const char D_003994E0[];
extern const char D_003994E8[];
extern const char D_003994F0[];
extern const char D_003994F8[];

void ScrCtrlMainLoop(void *x) {
    int tmp_time;
    int rtime;
    int subtline;
    int i;

    if (score_str.stdat_dat_pp->play_step == PSTEP_BONUS) {
        bonusGameInit();
    }

    while (TapCt(0x8070, 0, 0) != 0) {
        MtcWait(1);
    }

    if (GetTimeType(global_data.draw_tbl_top) != GTIME_VSYNC) {
        while (CdctrlWP2CheckBuffer()) {
            printf("cd buffer wait\n");
            MtcWait(1);
        }
    }
    MtcWait(3);

    score_str.ready_flag = TRUE;
    do {
        MtcWait(1);
    } while (!score_str.go_loop_flag);

    if (GetTimeType(global_data.draw_tbl_top) != GTIME_VSYNC) {
        CdctrlWP2Play();
        CdctrlWP2SetVolume(120);
    } else {
        TimeCallbackTimeSet(0);
        allTimeCallbackTimeSetChanTempo(0);
    }

    while (1) {
        MbarReset();
        MbarDispSceneVsDrawInit();

        outsideDrawSceneClear();
        
        GlobalTimeJob();
        ScrTimeRenew(score_str.stdat_dat_pp->scr_pp);

        if (global_data.demo_flagL == DEMOF_REPLAY) {
            if (pad[0].one & SCE_PADselect) {
                replayGuiOffFlag ^= 1;
            }
        }

        ScrLineSafeRefMode();

        ScrCtrlIndvJob();
        tapReqGroupPoll();

        for (i = 0; i < PR_ARRAYSIZEU(score_indv_str); i++) {
            SetIndvCdChannel(&score_indv_str[i]);
            SetIndvDrawTblLine(&score_indv_str[i]);
        }

        DrawCtrlTimeSet(ScrDrawTimeGetFrame(scrDrawLine));

        rtime = nextExamTime();
        if (rtime >= 0) {
            BallThrowSetFrame(((rtime * 3600.0f) + (score_str.stdat_dat_pp->tempo * 96.0f * 0.5f)) / (score_str.stdat_dat_pp->tempo * 96.0f));
        }

        SprClear();

        if (score_str.mbar_flag) {
            ScrMbarReq(ScrDrawTimeGet(scrMbarLine));
            outsideDrawSceneReq(MbarDispScene, 0xdc, 0, DNUM_VRAM2, NULL);
            if (replayGuiOffFlag == 0) {
                if (otehonSetCheck()) {
                    outsideDrawSceneReq(MbarDispGuiScene, 0xf0, 2, DNUM_DRAW, NULL);
                } else {
                    outsideDrawSceneReq(MbarDispGuiScene, 0xf0, 0, DNUM_DRAW, NULL);
                }
            } else {
                outsideDrawSceneReq(MbarDispGuiSceneMbarArea, 0xf0, 0, DNUM_DRAW, NULL);
            }
        } else if (jimakuWakuOff == 0) {
            outsideDrawSceneReq(MbarDispGuiScene, 0xf0, 0, DNUM_DRAW, NULL);
            if (game_status.subtitle == SUBTITLE_ON) {
                ExamDispSubt();
            }
        }

        tmp_time = ((ScrDrawTimeGet(scrJimakuLine) * 3600.0f) + (score_str.stdat_dat_pp->tempo * 96.0f * 0.5f)) / (score_str.stdat_dat_pp->tempo * 96.0f);
        subtline = GetSubtLine(scrJimakuLine);
        if (subtline != -1) {
            if (game_status.subtitle == SUBTITLE_ON) {
                SubtCtrlPrint(score_str.stdat_dat_pp->jimaku_str_pp, subtline, tmp_time, game_status.language_type);
            }
        }

        if (global_data.play_step == PSTEP_GAME ||
            global_data.play_step == PSTEP_VS   ||
            global_data.play_step == PSTEP_HOOK ||
            global_data.play_step == PSTEP_BONUS) {
            sceGifPacket dbgPk;
            u_char *dbg_tbl_msg[17] = {
                #if 0
                "BASE",
                "LV1",  "LV2",  "LV3",  "LV4",
                "LV5",  "LV6",  "LV7",  "LV8",
                "LV9",  "LV10", "LV11", "LV12",
                "LV13", "LV14", "LV15", "LV16",
                #else
                (u_char*)D_00399478,
                (u_char*)D_00399480, (u_char*)D_00399488, (u_char*)D_00399490, (u_char*)D_00399498,
                (u_char*)D_003994A0, (u_char*)D_003994A8, (u_char*)D_003994B0, (u_char*)D_003994B8,
                (u_char*)D_003994C0, (u_char*)D_003994C8, (u_char*)D_003994D0, (u_char*)D_003994D8,
                (u_char*)D_003994E0, (u_char*)D_003994E8, (u_char*)D_003994F0, (u_char*)D_003994F8,
                #endif
            };
            int    drtime;
            u_char timemsg[16];
            /* sdata 399500 */ extern int dbgmsg_on_off;

            if (pad[0].one & SCE_PADi) {
                dbgmsg_on_off ^= 1;
            }

            if (dbgmsg_on_off) {
                DbgMsgInit();
                DbgMsgClear();
                CmnGifOpenCmnPk(&dbgPk);
                DbgMsgClearUserPkt(&dbgPk);
                DbgMsgPrintUserPkt(dbg_tbl_msg[global_data.tapLevel], 0x6c2, 0x79c, &dbgPk);
                drtime = ScrDrawTimeGet(scrMbarLine);
                sprintf(timemsg, "%2d.%d.%2d", (drtime / 384) + 1, ((drtime / 96) % 4) + 1, (drtime % 96) + 1);
                DbgMsgPrintUserPkt(timemsg, 0x6c2, 0x792, &dbgPk);
                CmnGifCloseCmnPk(&dbgPk, 0xf);
            }
        }
        
        MtcWait(1);
    }
}

GET_TIME_TYPE GetTimeType(int scr_line) {
    return score_str.stdat_dat_pp->scr_pp->scr_ctrl_pp[scr_line].gtime_type;
}

int GetTimeOfset(int scr_line) {
    return score_str.stdat_dat_pp->scr_pp->scr_ctrl_pp[scr_line].ofsCdtime;
}

int GetSubtLine(int scr_line) {
    return score_str.stdat_dat_pp->scr_pp->scr_ctrl_pp[scr_line].subtLine;
}

int GetDrawLine(int scr_line) {
    return score_str.stdat_dat_pp->scr_pp->scr_ctrl_pp[scr_line].drawLine;
}

float GetLineTempo(int scr_line) {
    return score_str.stdat_dat_pp->scr_pp->scr_ctrl_pp[scr_line].tempo;
}

void SetLineChannel(int scr_line) {
    SCR_CTRL *scr_ctrl_pp = score_str.stdat_dat_pp->scr_pp->scr_ctrl_pp + scr_line;

    if (scr_ctrl_pp->gtime_type != GTIME_VSYNC) {
        CdctrlWP2SetChannel(scr_ctrl_pp->cdChan[0], scr_ctrl_pp->cdChan[1]);
    }
}

INCLUDE_ASM("asm/nonmatchings/main/scrctrl", SetIndvCdChannel);

INCLUDE_ASM("asm/nonmatchings/main/scrctrl", CheckIndvCdChannel);

void ScrCtrlInit(STDAT_DAT *sdat_pp, void *data_top) {
    int           i, j;
    int           add_move;
    GET_TIME_TYPE ttype;

    score_str.int_top      = data_top;
    score_str.stdat_dat_pp = sdat_pp;
    ScrTapCtrlInit(data_top);

    PrSetStage(global_data.play_stageL);
    PrRestartMenderer();

    if (global_data.play_step == PSTEP_HOOK) {
        inCmnHook2GameSave(tapLevelChangeSub());
        MbarHookUseInit();
        MbarNikoHookUse();
    } else {
        MbarHookUnUse();
        if (global_data.play_step == PSTEP_VS) {
            MbarNikoVsUse();
        } else {
            MbarNikoUnUse();
        }
    }

    SprInit();
    mccReqCtrlClr();
    tapReqGroupInit();

    if (global_data.play_step == PSTEP_VS) {
        add_move = tapLevelChangeSub();
        if (global_data.demo_flagL == DEMOF_REPLAY) {
            add_move = mccReqLvlGet();
        } else {
            mccReqLvlSet(add_move);
        }
        global_data.tapLevel = add_move;
    }

    MbarMemberClear(clearStageCheck());

    currentTblNumber = 0;
    outsideDrawSceneClear();

    if (score_str.stdat_dat_pp->scr_pp->sndrec_num != 0) {
        ScrTapDataTrans(score_str.stdat_dat_pp->scr_pp->sndrec_pp, 0, score_str.int_top);
    }

    TapCt(0xb0, 0, 0x1fff);
    TapCt(0x80, PR_CONCAT(0x3fff, 0x3fff), 0);

    for (i = 0; i < PR_ARRAYSIZE(score_str.stdat_dat_pp->sndfile); i++) {
        if (score_str.stdat_dat_pp->sndfile[i].fname != NULL) {
            while (!CdctrlSerch(&score_str.stdat_dat_pp->sndfile[i])) {
                printf("file serch error![%s]\n", score_str.stdat_dat_pp->sndfile[i].fname);
            }
        }
    }
    FlushCache(0);

    ScrCtrlIndvInit(sdat_pp);
    for (i = 0; i < PR_ARRAYSIZE(score_indv_str); i++) {
        ScrCtrlIndvNextRead(&score_indv_str[i], 1);
    }

    ttype = GetTimeType(global_data.draw_tbl_top);
    if (ttype == GTIME_VSYNC) {
        GlobalTimeJobChange(FGF_VSYNC);
    } else {
        GlobalTimeJobChange(FGF_CD);
        if (score_str.stdat_dat_pp->play_step != PSTEP_XTR) {
            CdctrlWP2Set(&score_str.stdat_dat_pp->sndfile[ttype]);
        } else {
            CdctrlXTRset(&score_str.stdat_dat_pp->sndfile[ttype], UsrMemAllocNext());
        }
    }

    SetLineChannel(global_data.draw_tbl_top);

    score_str.ready_flag = FALSE;

    scrJimakuLine = global_data.draw_tbl_top;
    scrMbarLine   = global_data.draw_tbl_top;
    scrDrawLine   = global_data.draw_tbl_top;

    score_str.go_loop_flag = FALSE;
    MtcExec(ScrCtrlMainLoop, MTC_TASK_SCORECTRL);

    score_str.mbar_flag = FALSE;

    if (GlobalMendererUseCheck()) {
        PrInitializeMenderer(0x3ded, GetIntAdrsCurrent(INTNUM_NOODLE_TEX), DrawGetFbpPos(DNUM_VRAM2));
    }

    if (score_str.stdat_dat_pp->play_step != PSTEP_SERIAL &&
        score_str.stdat_dat_pp->play_step != PSTEP_BONUS &&
        score_str.stdat_dat_pp->play_step != PSTEP_XTR) {
        for (j = 0; j < 4; j++) {
            /* Empty */
        }

        score_str.mbar_flag = TRUE;
        MbarInit(score_str.stdat_dat_pp->stage);
        ExamDispReset();
    }

    SubtCtrlInit(GetIntAdrsCurrent(INTNUM_SUBT_CODE),
        /* Serial types (Cutscene, bonus stage, Boxy Boy practice) */
        (score_str.stdat_dat_pp->play_step == PSTEP_SERIAL) |
        (score_str.stdat_dat_pp->play_step == PSTEP_BONUS) |
        (score_str.stdat_dat_pp->play_step == PSTEP_HOOK) |
        (score_str.stdat_dat_pp->play_step == PSTEP_XTR)
    );

    titleStartKey = FALSE;
    fadeoutStartKey = FALSE;
    gameEndWaitLoop = FALSE;
    replayGuiOffFlag = FALSE;
    followTapInit();

    jimakuWakuOff = FALSE;
}

void ScrCtrlQuit(void) {
    useAllClearKeySnd();
    TapCt(0x120, 0, 0);

    MtcKill(MTC_TASK_SCORECTRL);
}

int ScrCtrlInitCheck(void) {
    return score_str.ready_flag;
}

void ScrCtrlGoLoop(void) {
    score_str.go_loop_flag = TRUE;
}

int ScrEndCheckScore(void) {
    int             i;
    SCORE_INDV_STR *sindv_pp = score_indv_str;

    for (i = 0; i < PR_ARRAYSIZE(score_indv_str); i++, sindv_pp++) {
        if (!(sindv_pp->status & SCS_USE)) {
            continue;
        }
        if (sindv_pp->status & SCS_END) {
            printf("end end end[%d] line time[%d]\n", i, sindv_pp->top_scr_ctrlpp[i].lineTime);
            return 1;
        }
    }

    return 0;
}

int ScrEndCheckTitle(void) {
    return titleStartKey;
}

int ScrEndCheckFadeOut(void) {
    return fadeoutStartKey;
}

int ScrEndWaitLoop(void) {
    return gameEndWaitLoop;
}

static void bonusGameInit(void) {
    WorkClear(&bng_str, sizeof(bng_str));

    bng_str.st_time   = 0;
    bng_str.end_time  = 18048;
    bng_str.bonus_cnt = 0;
}

static int bonusGameCntPls(void) {
    bng_str.bonus_cnt++;
    return bng_str.bonus_cnt;
}

static void bonusPointSave(void) {
    ingame_common_str.BonusScore = bng_str.ok_cnt - bng_str.ng_cnt;
    if (ingame_common_str.BonusScore < 0) {
        ingame_common_str.BonusScore = 0;
    }
}

void bngTapEventCheck(SCORE_INDV_STR *sindv_pp, int num, int id) {
    TAPSET *tapset_pp;
    TAPDAT *tapdat_pp;

    if (sindv_pp->scrdat_pp == NULL) {
        return;
    }

    tapset_pp = &sindv_pp->scrdat_pp->tapstr[global_data.tapLevel].tapset_pp[sindv_pp->tapset_pos];
    tapdat_pp = &tapset_pp->tapdat_pp[num];

    if (tapdat_pp->tapct[0].actor != -1) {
        DrawTapReqTbl(tapdat_pp->tapct[0].actor, Pcode2Pindex(sindv_pp->plycode), NULL);
    }

    if (tapdat_pp->tapct[0].sound != -1) {
        ScrTapReq(sindv_pp->sndId, id, tapdat_pp->tapct[0].sound);
    }
}

static void bonusGameParaReq(BNG_ACT_P_ENUM actnum) {
    bngTapEventCheck(&score_indv_str[2], actnum, 0);
}

static void bonusGameKoamaReq(int kotamaNum, BNG_ACT_K_ENUM actnum) {
    bngTapEventCheck(&score_indv_str[1], actnum + kotamaNum, kotamaNum + 1);
}

static int bonus_minus_point_sub(int wtime) {
    if (wtime < 8) {
        return 9;
    } else if (wtime < 13) {
        return 13;
    } else if (wtime < 20) {
        return 18;
    } else {
        return 24;
    }
}

static int bonus_pls_point_sub(int wtime) {
    if (wtime < 3) {
        return 26;
    } else if (wtime < 6) {
        return 15;
    } else if (wtime < 9) {
        return 9;
    } else {
        return 5;
    }
}

INCLUDE_ASM("asm/nonmatchings/main/scrctrl", bonusGameCtrl);

static u_long hex2dec(u_long data) {
    u_long ret = 0;
    u_int  i;

    for (i = 0; i < 16; i++) {
        if (data == 0) {
            break;
        }

        ret |= (data % 10) << (i * 4);
        data /= 10;
    }

    return ret;
}

INCLUDE_ASM("asm/nonmatchings/main/scrctrl", bnNumberDisp);

static void bonusScoreDraw(void) {
    long         scr_stg;
    long         scr_bn;
    long         scr_add;

    sceGifPacket bn_gif;
    VCLR_PARA    vclr_para = {};

    DrawVramClear(&vclr_para, 0, 0, 0, DNUM_VRAM2);
    ChangeDrawArea(DrawGetDrawEnvP(DNUM_VRAM2));

    CmnGifADPacketMake(&bn_gif, NULL);

    sceGifPkAddGsAD(&bn_gif, SCE_GS_TEXFLUSH, 0);
    sceGifPkAddGsAD(&bn_gif, SCE_GS_TEST_1, SCE_GS_SET_TEST_1(0, 0, 0, 0, 0, 0, 1, 1));
    sceGifPkAddGsAD(&bn_gif, SCE_GS_TEXA, 0x8000008000);
    sceGifPkAddGsAD(&bn_gif, SCE_GS_CLAMP_1, 5);
    sceGifPkAddGsAD(&bn_gif, SCE_GS_PABE, 0);
    sceGifPkAddGsAD(&bn_gif, SCE_GS_TEXA, 0x8000008000);

    scr_bn  = ingame_common_str.BonusScore;
    scr_stg = ingame_common_str.SingleScore;
    scr_add = scr_stg + scr_bn;

    bnNumberDisp(&bn_gif, scr_stg,  0,  0, 5, 1, 0);
    bnNumberDisp(&bn_gif, scr_stg, 96, 32, 5, 0, 1);

    bnNumberDisp(&bn_gif, scr_bn,  32,  0, 5, 1, 0);
    bnNumberDisp(&bn_gif, scr_bn,  96, 56, 5, 0, 1);

    bnNumberDisp(&bn_gif, scr_add, 64,  0, 5, 1, 0);
    bnNumberDisp(&bn_gif, scr_add, 96, 80, 5, 0, 1);

    bnNumberDisp(&bn_gif, ingame_common_str.BonusStage, 96,  0, 1, 0, 0);
    bnNumberDisp(&bn_gif, ingame_common_str.BonusStage, 128, 0, 1, 0, 1);

    CmnGifADPacketMakeTrans(&bn_gif);
}

static void set_lero_gifset(sceGifPacket *gifpk_pp, LERO_TIM2_PT *let2_pp, short xp, short yp) {
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_UV,   SCE_GS_SET_UV(let2_pp->u0 << 4, let2_pp->v0 << 4));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_XYZ2, SCE_GS_SET_XYZ2((xp + 2048) << 4,
                                                           (yp + 2048) << 4, 1));

    sceGifPkAddGsAD(gifpk_pp, SCE_GS_UV,   SCE_GS_SET_UV(let2_pp->u1 << 4, let2_pp->v1 << 4));
    sceGifPkAddGsAD(gifpk_pp, SCE_GS_XYZ2, SCE_GS_SET_XYZ2((xp + 2048 + let2_pp->w) << 4, 
                                                           (yp + 2048 + let2_pp->h) << 4, 1));
}

static void LessonRoundDisp(SCRRJ_LESSON_ROUND_ENUM type) {
    sceGifPacket gifpk;
    TIM2_DAT    *tim2_dat_pp;
    int          i;

    if (type >= SCRRJ_LR_MAX) {
        return;
    }

    tim2_dat_pp = lessonTim2InfoGet();
    (*(sceGsTex0*)&tim2_dat_pp->GsTex0).CBP = (*(sceGsTex0*)&lessonCl2InfoGet(type)->GsTex0).CBP;

    CmnGifOpenCmnPk(&gifpk);
    ChangeDrawAreaSetGifTag(DrawGetDrawEnvP(DNUM_DRAW), &gifpk);

    sceGifPkAddGsAD(&gifpk, SCE_GS_TEXFLUSH, 0);
    sceGifPkAddGsAD(&gifpk, SCE_GS_TEST_1, SCE_GS_SET_TEST_1(1, 6, 0, 0, 0, 0, 1, 1));
    sceGifPkAddGsAD(&gifpk, SCE_GS_TEXA, 0x8000008000);
    sceGifPkAddGsAD(&gifpk, SCE_GS_CLAMP_1, 5);
    sceGifPkAddGsAD(&gifpk, SCE_GS_PABE, 0);
    sceGifPkAddGsAD(&gifpk, SCE_GS_TEXA, 0x8000008000);
    sceGifPkAddGsAD(&gifpk, SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1(0, 1, 0, 1, 0));
    sceGifPkAddGsAD(&gifpk, SCE_GS_TEX0_1, tim2_dat_pp->GsTex0);
    sceGifPkAddGsAD(&gifpk, SCE_GS_TEX1_1, tim2_dat_pp->GsTex1);
    sceGifPkAddGsAD(&gifpk, SCE_GS_PRIM, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 1, 0, 1, 0, 1, 0, 0));

    for (i = 0; i < 2; i++) {
        set_lero_gifset(&gifpk, &lero_tim2_pt[lero_pos_str[type][i].tim2_num], lero_pos_str[type][i].posx, lero_pos_str[type][i].posy);
    }

    CmnGifCloseCmnPk(&gifpk, 2);
}
