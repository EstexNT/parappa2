#include "main/subt.h"

#include "os/cmngifpk.h"
#include "os/syssub.h"

#include "main/etc.h"

/* data 184ba0 */ extern MCODE_ASCII mcode_ascii[]; /* static */

int SUBT_POSX = 2048;
int SUBT_POSY = 2098;
/* data 185460  */ extern u_long SubtGsTex0[3]; /* static */
static int subtSetNum = 0;

static MCODE_STR *kanji_pp;

static sceGifPacket subtPkSpr;
static SUBT_CODE subt_code[16];
static MCODE_DAT *mcode_dat_pp[256];

void SubtInit(void) {
    subtSetNum = 0;
}

void* SubtKanjiSet(void *adrs) {
    void *ret = kanji_pp;
    kanji_pp = adrs;
    return ret;
}

void SubtClear(void) {
    CmnGifOpenCmnPk(&subtPkSpr);

    sceGifPkAddGsAD(&subtPkSpr, SCE_GS_TEXFLUSH, 0);
    sceGifPkAddGsAD(&subtPkSpr, SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(128, 128, 128, 128, 0));
    sceGifPkAddGsAD(&subtPkSpr, SCE_GS_TEST_1, SCE_GS_SET_TEST_1(1, 6, 0, 0, 0, 0, 1, 1));
    sceGifPkAddGsAD(&subtPkSpr, SCE_GS_PRMODECONT, 1);
    sceGifPkAddGsAD(&subtPkSpr, SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1(0, 1, 0, 1, 0));

    subtSetNum = 0;
}

void SubtFlash(void) {
    if (subtSetNum != 0) {
        CmnGifCloseCmnPk(&subtPkSpr, 9);
    }
}

void SubtMcodeSet(int code) {
    sceGifPkAddGsAD(&subtPkSpr, SCE_GS_TEX0_1, SubtGsTex0[code]);
    sceGifPkAddGsAD(&subtPkSpr, SCE_GS_TEX1_1, SCE_GS_SET_TEX1(0, 0, 0, 1, 1, 0, 0));
    sceGifPkAddGsAD(&subtPkSpr, SCE_GS_TEXA, SCE_GS_SET_TEXA(0, 1, 128));
}

MCODE_DAT* codeKanjiCheck(u_char dat0, u_char dat1) {
    u_short      code     = dat0 | (dat1 << 8);
    MCODE_KANJI *kcode_pp = kanji_pp->mcode_kanji;
    int          i        = 0;

    for (i = 0; i < kanji_pp->mcode_max; i++, kcode_pp++) {
        if (kcode_pp->code == code) {
            return (MCODE_DAT*)(&kcode_pp->u);
        }
    }

    return NULL;
}

/* Same function, matching lines too */
/* https://github.com/JunkBox-Library/JunkBox_Lib/blob/255f34bccfd0fab817d71730f6b45cab52062ec8/Lib/tools.c#L2579 */
static void euc2sjis(unsigned char *c1, unsigned char *c2) {
    if (*c1 % 2 == 0) {
        *c2 -= 0x02;
    } else {
        *c2 -= 0x61;
        if (*c2 > 0x7e) {
            (*c2)++;
        }
    }

    if (*c1 < 0xdf) {
        (*c1)++;
        *c1 /= 2;
        *c1 += 0x30;
    } else {
        (*c1)++;
        *c1 /= 2;
        *c1 += 0x70;
    }
}

void SubtMsgPrint(u_char* msg_pp, int xp, int yp, int jap_flag, int mline) {
    u_char *tmp_pp;
    int     line_num;
    int     i, j, k;
    int     hsize;
    int     cnt_all;

    cnt_all = 0;    
    SubtMcodeSet(jap_flag);

    if (*msg_pp == '\0') {
        return;
    }

    WorkClear(&subt_code, sizeof(subt_code));

    line_num = 0;
    hsize    = 13;
    tmp_pp   = msg_pp;

    while (1) {
        if (hsize == 13) {
            hsize = 13;
        }

        if (*tmp_pp == '\0') {
            line_num++;
            break;
        }

        if (jap_flag) {
            u_char dat0 = tmp_pp[0];
            u_char dat1 = tmp_pp[1];

            if (dat0 == '@') {
                line_num++;
            } else if (dat0 != ' ') {
                euc2sjis(&dat0, &dat1);
                if (dat0 == 0x81 && dat1 == 0x97) {
                    line_num++;
                } else {
                    mcode_dat_pp[cnt_all] = codeKanjiCheck(dat0, dat1);
                    if (mcode_dat_pp[cnt_all] != NULL) {
                        subt_code[line_num].cnt++;
                        subt_code[line_num].wsize += mcode_dat_pp[cnt_all]->w;
                        cnt_all++;
                    }
                }

                tmp_pp++;
            }
        } else {
            if (*tmp_pp == '@') {
                line_num++;
            } else {
                mcode_dat_pp[cnt_all] = &mcode_ascii[*tmp_pp - 32];
                subt_code[line_num].cnt++;
                subt_code[line_num].wsize += mcode_dat_pp[cnt_all]->w;
                cnt_all++;
            }
        }

        tmp_pp++;
    }

    if (mline != 0 && mline < line_num) {
        line_num = mline;
    }

    for (i = 0, k = 0; i < line_num; i++) {
        int posx = xp - (subt_code[i].wsize / 2);
        int posy = yp + (hsize * i);

        for (j = 0; j < subt_code[i].cnt; j++) {
            MCODE_DAT *mcode_pp = mcode_dat_pp[k++];

            sceGifPkAddGsAD(&subtPkSpr, SCE_GS_PRIM, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 1, 0, 1, 0, 1, 0, 0));

            sceGifPkAddGsAD(&subtPkSpr, SCE_GS_UV,
                SCE_GS_SET_UV(mcode_pp->u << 4, mcode_pp->v << 4));
            sceGifPkAddGsAD(&subtPkSpr, SCE_GS_XYZ2,
                SCE_GS_SET_XYZ2((posx + mcode_pp->adjx) << 4, (posy + (mcode_pp->adjy / 2)) << 4, 1));
            sceGifPkAddGsAD(&subtPkSpr, SCE_GS_UV,
                SCE_GS_SET_UV((mcode_pp->u + mcode_pp->w) << 4, (mcode_pp->v + mcode_pp->h) << 4));
            sceGifPkAddGsAD(&subtPkSpr, SCE_GS_XYZ2,
                SCE_GS_SET_XYZ2((posx + mcode_pp->w + mcode_pp->adjx) << 4, (posy + ((mcode_pp->h + mcode_pp->adjy) / 2)) << 4, 1));

            posx += mcode_pp->w;
            subtSetNum = 1;
        }
    }
}

void SubtCtrlInit(void *adrs, int ser_f) {
    SubtInit();
    SubtKanjiSet(adrs);

    if (ser_f) {
        SUBT_POSX = 2048;
        SUBT_POSY = 2122;
    } else {
        SUBT_POSX = 2048;
        SUBT_POSY = 2104;
    }
}

void SubtCtrlPrint(JIMAKU_STR *jstr_pp, int line, int time, int lang) {
    JIMAKU_STR *jstr_tmp_pp = &jstr_pp[line];
    int         i;

    for (i = 0; i < jstr_tmp_pp->size; i++) {
        if ((time >= jstr_tmp_pp->jimaku_dat_pp[i].starTime) &&
            (time <  jstr_tmp_pp->jimaku_dat_pp[i].endTime)) {
            SubtClear();
            SubtMsgPrint(jstr_tmp_pp->jimaku_dat_pp[i].txtData[lang], SUBT_POSX, SUBT_POSY, lang == LANG_JAPANESE, 0);
            SubtFlash();
            return;
        }
    }
}

void SubtTapPrint(u_char *tap_msg_pp, int lang) {
    /* BUG: should be logical OR instead of bitwise? */
    if (tap_msg_pp == NULL | *tap_msg_pp == '\0') {
        return;
    }

    SubtClear();
    SubtMsgPrint(tap_msg_pp, SUBT_POSX, SUBT_POSY, lang == LANG_JAPANESE, 2);
    SubtFlash();
}

void SubtMenuCtrlInit(void *adrs) {
    SubtInit();
    SubtKanjiSet(adrs);
}

void SubtMenuCtrlPrint(u_char *msg_pp, int xp, int yp, int lang) {
    xp += GS_X_COORD(0) >> 4;
    yp += GS_Y_COORD(0) >> 4;

    SubtClear();
    SubtMsgPrint(msg_pp, xp, yp, lang == LANG_JAPANESE, 0);
    SubtFlash();
}

int SubtMsgDataKaijyouCnt(u_char *msg_pp, int jap_flag) {
    u_char *tmp_pp;
    u_char  dat0, dat1;
    int     ret = 1;

    if (*msg_pp == '\0') {
        return NULL;
    }

    tmp_pp = msg_pp;

    while (*tmp_pp != '\0') {
        dat0 = tmp_pp[0];
        dat1 = tmp_pp[1];

        tmp_pp++;

        /* New line */
        if (dat0 == '@') {
            ret++;
        } else if (jap_flag) {
            /* Japanese subt. new line */ 
            euc2sjis(&dat0, &dat1);
            tmp_pp++;

            /* '@' in SJIS - new line */
            if (dat0 == 0x81 && dat1 == 0x97) {
                ret++;
            }
        }
    }

    return ret;
}

u_char* SubtMsgDataPos(u_char *msg_pp, int jap_flag, int pos) {
    u_char *tmp_pp;
    u_char  dat0, dat1;
    int     ret = 0;

    if (*msg_pp == '\0') {
        return NULL;
    }

    tmp_pp = msg_pp;

    while (1) {
        if (ret == pos) {
            return tmp_pp;
        }
        if (*tmp_pp == '\0') {
            break;
        }

        dat0 = tmp_pp[0];
        dat1 = tmp_pp[1];
        tmp_pp++;

        /* New line */
        if (dat0 == '@') {
            ret++;
        } else if (jap_flag) {
            euc2sjis(&dat0, &dat1);

            /* '@' in SJIS - new line */
            if (dat0 == 0x81 && dat1 == 0x97) {
                ret++;
            }

            tmp_pp++;
        }
    }

    return NULL;
}

void SubtTapPrintWake(u_char *tap_msg_pp, int lang, int lng, int nowp) {
    int cntmax;
    int selpos;

    if (nowp < 0) {
        return;
    }

    if (nowp >= lng) {
        nowp = lng - 1;
    }

    if (tap_msg_pp == NULL) {
        return;
    }

    cntmax = SubtMsgDataKaijyouCnt(tap_msg_pp, lang == LANG_JAPANESE);
    if (cntmax >= 3) {
        cntmax = ((cntmax + 1) / 2 * nowp) / lng * 2;
        tap_msg_pp = SubtMsgDataPos(tap_msg_pp, lang == LANG_JAPANESE, cntmax);
    }

    SubtTapPrint(tap_msg_pp, lang);
}

void SubtCtrlPrintBoxyWipe(JIMAKU_STR *jstr_pp, int line, int time, int lang, void *code_pp) {
    int         i;
    int         lang_f;

    void       *kanjiset_tmp_pp;
    JIMAKU_STR *jstr_tmp_pp = &jstr_pp[line];

    for (i = 0; i < jstr_tmp_pp->size; i++) {
        if ((time >= jstr_tmp_pp->jimaku_dat_pp[i].starTime) &&
            (time <  jstr_tmp_pp->jimaku_dat_pp[i].endTime)) {
            SubtInit();

            kanjiset_tmp_pp = SubtKanjiSet(code_pp);

            lang_f = (lang == LANG_JAPANESE);
            if (lang_f) {
                lang_f = 2;
            }

            SubtClear();
            SubtMsgPrint(jstr_tmp_pp->jimaku_dat_pp[i].txtData[lang], 2048, 2122, lang_f, 0);

            SubtFlash();
            SubtKanjiSet(kanjiset_tmp_pp);
            return;
        }
    }
}
