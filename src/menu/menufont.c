#include "menu/menufont.h"

#include "main/etc.h"

#include "menu/menu_mdl.h"
#include "menu/mntm2hed.h"

#include <string.h>

/* data 18f580 */ extern u_long SubtGsTex0_TmpMenuFont[3]; /* static */
/* data 18f598 */ extern MCODE_ASCII mcode_ascii_TmpMenuFont[]; /* static */
/* data 18fe58 */ extern MCODE_ASCII mcode_HalfSpace; /* static */
/* data 18fe68 */ extern MCODE_KANJI_ANIME PadSymbolFontA[]; /* static */
/* data 18ff70 */ extern MCODE_DAT ArowPat[];
/* data 18ffa8 */ extern MCODE_DAT TsFont[]; /* static */
/* data 190360 */ extern char Tbl_ASC2EUC[193]; /* static */
/* sdata 399890 */ extern int _PadFont_SW; /* static */
/* sdata 399894 */ extern int _PadArrowState; /* static */
/* sbss 399b7c */ extern int _AnimeFontFlg; /* static */
/* sbss 399b80 */ extern MCODE_STR *kanji_pp_TmpMenuFont; /* static */

static MCODE_CHAR mcode_dat_pp[512];
static MNFONT_INFO MnSubtFontInfo[3];

static void _PKFontPut(SPR_PKT pk, SPR_PRM *spr, SUBT_CODE *psubt, int line_num, int xp, int yp, int pflg, int hsize, float rtx, float rty);
static void _PADArrow_Put(SPR_PKT pk, SPR_PRM *spr, MCODE_DAT *pfnt, int x, int y);
static int _JPFont_GetSubtCode(u_char *str, SUBT_CODE *subt_code);
static int _EGFont_GetSubtCode(u_char *str, SUBT_CODE *subt_code, MCODE_DAT *pfnt_ascii);
static MCODE_DAT* codeKanjiCheck(u_char dat0, u_char dat1, MCODE_KANJI *kcode_pp, int kcode_max);
static MCODE_DAT* codeKanjiACheck(u_char dat0, u_char dat1, MCODE_KANJI_ANIME *kcode_pp, int kcode_max);
static void euc2sjis(unsigned char *c1, unsigned char *c2);

void MenuFont_ASC2EUC(char *des, char *src) {
    u_int len = strlen(Tbl_ASC2EUC) / 2;
    u_char c;

    for (; (c = *src) != '\0'; src++) {
        if (c >= ' ' && (c - ' ') < len) {
            c = (c - ' ') * 2;
            *des++ = Tbl_ASC2EUC[c + 0];
            *des++ = Tbl_ASC2EUC[c + 1];
        } else {
            *des++ = 0xa1; /* Full-width space */
            *des++ = 0xa1;
        }
    }

    *des = '\0';
}

void MENUSubtSetKanji(void *kanji_data_top) {
    kanji_pp_TmpMenuFont = (MCODE_STR*)kanji_data_top;
}

void MENUSubt_PadFontSw(int flg) {
    _PadFont_SW = flg;
}

void MENUSubt_PadFontArrowSet(int flg) {
    _PadArrowState = 0;

    if (flg & 1) {
        _PadArrowState = 1;
    }
    if (flg & 2) {
        _PadArrowState |= 2;
    }
    if (flg & 4) {
        _PadArrowState |= 4;
    }
    if (flg & 8) {
        _PadArrowState |= 8;
    }
}

int MENUSubtGetLine(u_char *str, int lflg) {
    SUBT_CODE subt_code[16] = {};
    int       line;

    if (lflg == LANG_JAPANESE) {
        line = _JPFont_GetSubtCode(str, subt_code);
    } else {
        line = _EGFont_GetSubtCode(str, subt_code, mcode_ascii_TmpMenuFont);
    }

    return line;
}

void MENUSubtPut(SPR_PKT pk, SPR_PRM *spr, int x, int y, u_int abgr, int flg, u_char *str, int lflg) {
    SUBT_CODE subt_code[16] = {};
    int       line;
    u_long    tex0;

    MnSubtFontInfo[0].tex0 = SubtGsTex0_TmpMenuFont[0];
    MnSubtFontInfo[0].abgr = abgr;

    MnSubtFontInfo[1].tex0 = SubtGsTex0_TmpMenuFont[1];
    MnSubtFontInfo[1].abgr = abgr;

    tex0 = TsGetTM2Hed(1)->GsTex0;
    tex0 |= SCE_GS_SET_TEX0(0, 0, 0, 0, 0, 1 /* RGBA */, 0, 0, 0, 0, 0, 0);
    
    MnSubtFontInfo[2].tex0 = tex0;
    MnSubtFontInfo[2].abgr = SCE_GS_SET_RGBAQ(128, 128, 128, 128, 0);

    if (lflg == LANG_JAPANESE) {
        line = _JPFont_GetSubtCode(str, subt_code);
    } else {
        line = _EGFont_GetSubtCode(str, subt_code, mcode_ascii_TmpMenuFont);
    }

    if (line != 0) {
        spr->rgba0 = abgr;

        _AnimeFontFlg = ((MNSceneGetMusicFitTimer() % 40) > 20);

        spr->zx = 1.0f;
        spr->zy = 0.47f;

        _PKFontPut(pk, spr, subt_code, line, x, y, flg, 0x1a, spr->zx, spr->zy);
    }
}

void MENUFontPutL(SPR_PKT pk, SPR_PRM *spr, int x, int y, u_int abgr, int flg, u_char *str) {
    SUBT_CODE subt_code[16] = {};
    int       line;
    u_long    tex0;

    line = _EGFont_GetSubtCode(str, subt_code, mcode_ascii_TmpMenuFont);
    if (line == 0) {
        return;
    }

    MnSubtFontInfo[0].tex0 = SubtGsTex0_TmpMenuFont[0];
    MnSubtFontInfo[0].abgr = abgr;

    MnSubtFontInfo[1].tex0 = SubtGsTex0_TmpMenuFont[1];
    MnSubtFontInfo[1].abgr = abgr;

    tex0 = TsGetTM2Hed(1)->GsTex0;
    tex0 |= SCE_GS_SET_TEX0(0, 0, 0, 0, 0, 1 /* RGBA */, 0, 0, 0, 0, 0, 0);

    MnSubtFontInfo[2].tex0 = tex0;
    MnSubtFontInfo[2].abgr = SCE_GS_SET_RGBAQ(128, 128, 128, 128, 0);

    _AnimeFontFlg = ((MNSceneGetMusicFitTimer() % 40) > 20);

    spr->zx = 1.0f;
    spr->zy = 0.5f;

    _PKFontPut(pk, spr, subt_code, line, x, y, flg, 24, spr->zx, spr->zy);
}

void MENUFontPutS(SPR_PKT pk, SPR_PRM *spr, int x, int y, u_int abgr, int flg, u_char *str) {
    SUBT_CODE subt_code[16] = {};
    int       line;
    u_long    tex0;

    line = _EGFont_GetSubtCode(str, subt_code, TsFont);
    if (line == 0) {
        return;
    }

    tex0 = TsGetTM2Hed(0)->GsTex0;
    tex0 |= SCE_GS_SET_TEX0(0, 0, 0, 0, 0, 1 /* RGBA */, 0, 0, 0, 0, 0, 0);

    MnSubtFontInfo[0].tex0 = tex0;
    MnSubtFontInfo[0].abgr = abgr;

    MnSubtFontInfo[1].tex0 = SubtGsTex0_TmpMenuFont[1];
    MnSubtFontInfo[1].abgr = abgr;

    tex0 = TsGetTM2Hed(1)->GsTex0;
    tex0 |= SCE_GS_SET_TEX0(0, 0, 0, 0, 0, 1 /* RGBA */, 0, 0, 0, 0, 0, 0);

    MnSubtFontInfo[2].tex0 = tex0;
    MnSubtFontInfo[2].abgr = SCE_GS_SET_RGBAQ(128, 128, 128, 128, 0);

    _AnimeFontFlg = ((MNSceneGetMusicFitTimer() % 40) > 20);

    spr->zx = 0.6f;
    spr->zy = 0.44999999f;

    _PKFontPut(pk, spr, subt_code, line, x, y, flg, 16, spr->zx, spr->zy);
}

void MENUFontPutR(SPR_PKT pk, SPR_PRM *spr, int x, int y, u_int abgr, int flg, u_char *str, float dw) {
    SUBT_CODE subt_code[16] = {};
    int       line;
    u_long    tex0;

    line = _EGFont_GetSubtCode(str, subt_code, TsFont);
    if (line == 0) {
        return;
    }

    tex0 = TsGetTM2Hed(0)->GsTex0;
    tex0 |= SCE_GS_SET_TEX0(0, 0, 0, 0, 0, 1 /* RGBA */, 0, 0, 0, 0, 0, 0);

    MnSubtFontInfo[0].tex0 = tex0;
    MnSubtFontInfo[0].abgr = abgr;

    MnSubtFontInfo[1].tex0 = SubtGsTex0_TmpMenuFont[1];
    MnSubtFontInfo[1].abgr = abgr;

    tex0 = TsGetTM2Hed(1)->GsTex0;
    tex0 |= SCE_GS_SET_TEX0(0, 0, 0, 0, 0, 1 /* RGBA */, 0, 0, 0, 0, 0, 0);

    MnSubtFontInfo[2].tex0 = tex0;
    MnSubtFontInfo[2].abgr = SCE_GS_SET_RGBAQ(128, 128, 128, 128, 0);

    _AnimeFontFlg = ((MNSceneGetMusicFitTimer() % 40) > 20);

    spr->zx = 0.6f;
    spr->zy = 0.38f;

    _PKFontPut(pk, spr, subt_code, line, x, y, flg, 16, spr->zx * dw, spr->zy);
}

static void _PKFontPut(SPR_PKT pk, SPR_PRM *spr, SUBT_CODE *psubt, int line_num, int xp, int yp, int pflg, int hsize, float rtx, float rty) {
    int         i, j;
    MCODE_CHAR *ppMcode;
    int         oflg = -1;

    hsize = ((hsize * rty) + 0.5f);

    switch (pflg & 0xf00) {
    case 0x100:
        yp = (yp - ((line_num * hsize) >> 1));
        break;
    case 0x200:
        yp = (yp - (line_num * hsize) + 1);
        break;
    }

    ppMcode = mcode_dat_pp;
    for (i = 0; i < line_num; i++) {
        int x;
        int y;
        int w;

        switch (pflg & 0xf) {
        case 1:
            x = (xp - (((psubt[i].wsize * rtx) + 0.5f) * 0.5f));
            break;
        case 2:
            x = (xp - ((psubt[i].wsize * rtx) + 0.5f)) + 1.0f;
            break;
        default:
            x = xp;
            break;
        }

        y = yp + (hsize * i);

        for (j = 0; j < psubt[i].cnt; j++) {
            int        adjx, adjy;
            MCODE_DAT *pfnt = ppMcode->pmcode;
            int        mflg = ppMcode->flg;
            ppMcode++;

            if (oflg != mflg) {
                PkTEX0_Add(pk, MnSubtFontInfo[mflg].tex0);
                spr->rgba0 = MnSubtFontInfo[mflg].abgr;
                oflg = mflg;
            }

            if (mflg == 2) {
                if ((pfnt + 1)->w != 0) {
                    if (_AnimeFontFlg) {
                        pfnt = pfnt + 1;
                    }
                }
            }

            adjx = (pfnt->adjx * rtx) + 0.5f;
            adjy = (pfnt->adjy * rty) + 0.5f;

            if (pfnt->u & 0x8000) {
                _PADArrow_Put(pk, spr, pfnt, x + adjx, y + adjy);
            } else {
                spr->ux = pfnt->u;
                spr->uy = pfnt->v;
                spr->uw = pfnt->w;
                spr->uh = pfnt->h;

                spr->px = x + adjx;
                spr->py = y + adjy;
                spr->sw = pfnt->w;
                spr->sh = pfnt->h;

                PkNSprite_Add2(pk, spr, 3);
            }

            w = pfnt->w * rtx + 0.5f;
            x += w;
        }
    }
}

static void _PADArrow_Put(SPR_PKT pk, SPR_PRM *spr, MCODE_DAT *pfnt, int x, int y) {
    int        i;
    int        aflg;
    MCODE_DAT *pat;

    if (pfnt->v & 0x10) {
        aflg = _PadArrowState;
    } else {
        aflg = pfnt->v;
    }

    pat = ArowPat;

    spr->px = x;
    spr->py = y;
    
    spr->ux = pat->u;
    spr->uy = pat->v;

    spr->uw = pat->w;
    spr->uh = pat->h;

    spr->sw = pat->w;
    spr->sh = pat->h;

    PkNSprite_Add2(pk, spr, 3);
    pat++;

    for (i = 0; i < 4; i++, pat++, aflg >>= 1) {
        if (aflg & 1) {
            spr->px = x;
            spr->py = y;

            spr->ux = pat->u;
            spr->uy = pat->v;

            spr->uw = pat->w;
            spr->uh = pat->h;

            spr->sw = pat->w;
            spr->sh = pat->h;

            PkNSprite_Add2(pk, spr, 3);
        }
    }
}

static int _JPFont_GetSubtCode(u_char *str, SUBT_CODE *subt_code) {
    int         line_num;
    MCODE_CHAR *ppMcode;
    u_char      dat0, dat1;

    if (*str == '\0') {
        return 0;
    }

    line_num = 0;
    ppMcode = mcode_dat_pp;

    while (1) {
        dat0 = *str++;

        if (dat0 == '\0') {
            line_num++;
            break;
        }

        /* '@' - new line */
        if (dat0 == '@') {
            line_num++;
            continue;
        }

        if (dat0 == ' ') {
            ppMcode->pmcode = &mcode_HalfSpace;
            subt_code[line_num].wsize += ppMcode->pmcode->w;
            subt_code[line_num].cnt++;
            ppMcode->flg = 0;
            ppMcode++;
            continue;
        }

        dat1 = *str++;
        euc2sjis(&dat0, &dat1);

        /* '@' in SJIS - new line */
        if (dat0 == 0x81 && dat1 == 0x97) {
            line_num++;
            continue;
        }

        if (_PadFont_SW) {
            ppMcode->pmcode = codeKanjiACheck(dat0, dat1, PadSymbolFontA, 12);
            if (ppMcode->pmcode != NULL) {
                subt_code[line_num].cnt++;
                subt_code[line_num].wsize += ppMcode->pmcode->w;
                ppMcode->flg = 2;
                ppMcode++;
                continue;
            }
        }

        ppMcode->pmcode = codeKanjiCheck(dat0, dat1, kanji_pp_TmpMenuFont->mcode_kanji, kanji_pp_TmpMenuFont->mcode_max);
        if (ppMcode->pmcode != NULL) {
            subt_code[line_num].cnt++;
            subt_code[line_num].wsize += ppMcode->pmcode->w;
            ppMcode->flg = 1;
            ppMcode++;
        }
    }

    return line_num;
}

static int _EGFont_GetSubtCode(u_char *str, SUBT_CODE *subt_code, MCODE_DAT *pfnt_ascii) {
    int         line_num;
    MCODE_CHAR *ppMcode;
    u_char      c;
    u_char      dat0, dat1;

    if (*str == '\0') {
        return 0;
    }

    line_num = 0;
    ppMcode = mcode_dat_pp;

    while (1) {
        c = *str++;

        if (c == '\0') {
            line_num++;
            break;
        }
        if (c == '@') {
            line_num++;
        }

        if (c < 32) {
            continue;
        }

        if (_PadFont_SW) {
            dat0 = c;
            dat1 = *str;
            euc2sjis(&dat0, &dat1);

            ppMcode->pmcode = codeKanjiACheck(dat0, dat1, PadSymbolFontA, 12);
            if (ppMcode->pmcode != NULL) {
                str++;

                subt_code[line_num].cnt++;
                subt_code[line_num].wsize += ppMcode->pmcode->w;

                ppMcode->flg = 2;
                ppMcode++;
                continue;
            }
        }

        ppMcode->pmcode = &pfnt_ascii[c - 32];
        subt_code[line_num].wsize += ppMcode->pmcode->w;
        subt_code[line_num].cnt++;

        ppMcode->flg = 0;
        ppMcode++;
    }

    return line_num;
}

static MCODE_DAT* codeKanjiCheck(u_char dat0, u_char dat1, MCODE_KANJI *kcode_pp, int kcode_max) {
    u_short code = dat0 | (dat1 << 8);
    int     i    = 0;

    for (i = 0; i < kcode_max; i++, kcode_pp++) {
        if (kcode_pp->code == code) {
            return (MCODE_DAT*)(&kcode_pp->u);
        }
    }

    return NULL;
}

static MCODE_DAT* codeKanjiACheck(u_char dat0, u_char dat1, MCODE_KANJI_ANIME *kcode_pp, int kcode_max) {
    u_short code = dat0 | (dat1 << 8);
    int     i    = 0;

    for (i = 0; i < kcode_max; i++, kcode_pp++) {
        if (kcode_pp->code == code) {
            return (MCODE_DAT*)(&kcode_pp->u);
        }
    }

    return NULL;
}

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
