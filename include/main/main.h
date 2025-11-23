#ifndef MAIN_H
#define MAIN_H

#include "common.h"

#include <eetypes.h>

#include "dbug/dbug.h"

#include "main/cdctrl.h"
#include "main/sprite.h"

typedef struct { // 0x14
    /* 0x00 */ u_char *msg_pp;
    /* 0x04 */ int *set_pp;
    /* 0x08 */ int min;
    /* 0x0c */ int max;
    /* 0x10 */ u_char **selmsg_pp;
} DBG_MODE_STR;

enum {
    dmyPmTitle,
    dmyPmREPLAY,
    dmyPmSINGLE,
    dmyPmVS_MAN,
    dmyPmVS_COM,
};

typedef struct { // 0x8
    /* 0x0 */ int num;
    /* 0x4 */ int *data_pp;
} RT2TRANS_STR;

typedef enum {
    CBE_NORMAL,
    CBE_SINGLE,
    CBE_VS_MAN,
    CBE_VS_COM,
    CBE_HOOK,
    CBE_MAX,
} CANCEL_TYPE_ENUM;

int selPlayDisp(int sel_stage, int sel_disp, int firstf);
int selPlayDispTitleDisp(int sel_stage, int sel_disp, int ovl_load);

void xtrView(FILE_STR *file_str_pp);

void logoDispOne(SPR_PRIM *sprm_pp, TIM2_DAT *tmd_pp);
void startUpDisp(void);

int selPlayDispType(int sel_stage, int sel_disp, CANCEL_TYPE_ENUM canseltype);
int selPlayDispSetPlay(int sel_stage);
int selPlayDispSetPlayOne(int sel_stage);

int gamePlayDisp();
void titleDisp(int firstf);

int urawazaKeyCheck(void);
void ura_check(void);

void mainStart(void* xx);

extern DBG_SELECT_STR dbg_select_str;

#endif /* MAIN_H */
