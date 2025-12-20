#include "menu/memc.h"

#include <stdio.h>
#include <string.h>

/* data 18ddf8 */ extern sceMcIconSys memc_iconsys; /* static */
/* data 18e1c0 */ extern u_int _memc_type[]; /* static */
/* bss 1c81540 */ extern MEMC_STAT memc_stat; /* static */

static int memc_SaveFileClust(void);
static void memc_clearMEMCINFO(MEMC_INFO *info);
static int memc_mansub_ErrChk(int result);
static int memc_mansub_Open(char *name, u_int type);
static int memc_mansub_Close(void);
static int memcsub_fileChk(sceMcTblGetDir *dir, unsigned char *name, int max);
static int memc_mansub_GetInfo(int result);
static int memc_mansub_load(int result);
static int memc_manager_save(int result);
static int memc_manager_overwrite(int result);
static int memc_manager_chk(int mode);

void memc_init(void) {
    sceMcInit();
    memset(&memc_stat, 0, sizeof(memc_stat));
    memset(memc_iconsys.Reserve3, 0, sizeof(memc_iconsys.Reserve3));
}

void memc_setDirName(char *name) {
    memcpy(memc_stat.saveDir, name, 63);
    memc_stat.saveDir[63] = '\0';
}

void memc_setSaveTitle(char *name, int nLFPos) {
    int length;

    length = strlen(name);
    if (length > 64) {
        length = 64;
    }

    memcpy(memc_iconsys.TitleName, name, 64);

    memc_iconsys.TitleName[length + 1] = '\0';
    memc_iconsys.OffsLF = nLFPos;
}

void memc_setIconSysHed(void *pIhData, int IhSize) {
    int nLF;

    if (!pIhData || IhSize <= 0) {
        return;
    }

    nLF = memc_iconsys.OffsLF;
    
    memcpy(&memc_iconsys, pIhData, IhSize);
    memc_iconsys.OffsLF = nLF;
}

void memc_setSaveIcon(int no, void *pIconData, int nIconSize) {
    MEMC_STAT *pmw = &memc_stat;

    switch (no) {
    case 0:
        pmw->iconData1 = pIconData;
        pmw->iconSize1 = nIconSize;
        
        strcpy(memc_iconsys.FnameView, "icon1.ico");
        break;
    case 1:
        if (pIconData == NULL || nIconSize == 0) {
            pmw->iconData2 = 0;
            pmw->iconSize2 = 0;

            strcpy(memc_iconsys.FnameCopy, "icon1.ico");
        } else {
            pmw->iconData2 = pIconData;
            pmw->iconSize2 = nIconSize;

            strcpy(memc_iconsys.FnameCopy, "icon2.ico");
        }
        break;
    case 2:
        if (pIconData == NULL || nIconSize == 0) {
            pmw->iconData3 = 0;
            pmw->iconSize3 = 0;

            strcpy(memc_iconsys.FnameDel, "icon1.ico");
        } else {
            pmw->iconData3 = pIconData;
            pmw->iconSize3 = nIconSize;

            strcpy(memc_iconsys.FnameDel, "icon3.ico");
        }
        break;
    default:
        break;
    }
}

char* memc_getfilename(int no) {
    char *fbody;
    extern char tmps0[64];

    switch (no) {
    case -1:
        fbody = "icon.sys";
        break;
    case -2:
        fbody = "icon1.ico";
        break;
    case -3:
        if (memc_stat.iconData2 == NULL) {
            return NULL;
        }
    case -4:
        if (memc_stat.iconData3 == NULL) {
            return NULL;
        }
    default:
        if (no >= 2) {
            return NULL;
        }

        if (no == 0) {
            fbody = memc_stat.saveDir;
        } else {
            sprintf(tmps0, "SAVE%03d", no);
            fbody = tmps0;
        }
        break;
    }

    return fbody;
}

char* memc_getfilepath(int no) {
    extern char tmps1[130];
    char       *fbody;

    fbody = memc_getfilename(no);
    if (fbody == NULL) {
        return NULL;
    } else {
        strcpy(tmps1, memc_stat.saveDir);
        strcat(tmps1, "/");
        strcat(tmps1, fbody);
        fbody = tmps1;
    }

    return fbody;
}

int memc_checkFormat(void) {
    return memc_stat.format;
}

int memc_getChangeState(void) {
    return memc_stat.isChange;
}

void memc_setChangeState(int flg) {
    memc_stat.isChange = flg;
}

static int memc_SaveFileClust(void) {
    MEMC_STAT *pmw = &memc_stat;
    int        size;

    if (pmw->seek != 0) {
        size = pmw->seek + pmw->size2;
        return (size + 1023) / 1024;
    } else {
        size = pmw->size;
        return (size + 1023) / 1024;
    }
}

static void memc_clearMEMCINFO(MEMC_INFO *info) {
    info->flag = 0;
    info->free = 0;
    info->allfile = 0;
    info->savefile = 0;

    if (info->dirfile != NULL) {
        if (info->dirfileMax < 1) {
            return;
        }

        memset(info->dirfile, 0, info->dirfileMax * 64);
    }

    info->savefile = 0;
}

sceMcTblGetDir* memc_searchDirTbl(char *name, sceMcTblGetDir *dirTbl, int num, int isClose, int cmpSize, int *status) {
    int i, j;
    int flg;

    if (name == NULL) {
        return NULL;
    }

    for (i = 0; i < num && (flg = FALSE, dirTbl[i].EntryName[0] != '\0'); i++) {
        for (j = 0; /*None*/; j++) {
            if (name[j] == '\0') {
                flg = TRUE;
                break;
            }
            if (name[j] != '?' && name[j] != dirTbl[i].EntryName[j]) {
                break;
            }
        }

        if (flg) {
            if (cmpSize != 0 && dirTbl[i].FileSizeByte != cmpSize) {
                if (status != NULL) {
                    *status = 2;
                }
                return NULL;
            }

            if (status != NULL) {
                *status = 0;
            }

            return &dirTbl[i];
        }
    }

    if (status != NULL) {
        *status = 1;
    }

    return NULL;
}

int memc_port_info(int port, MEMC_INFO *info) {
    int        re;
    MEMC_STAT *pmw = &memc_stat;
 
    pmw->port = port;
    pmw->buf = (char*)info;

    pmw->cmd = sceMcFuncNoFileInfo;
    pmw->retry = 0;

    strcpy(pmw->filename, pmw->saveDir);
    strcat(pmw->filename, "/*");

    pmw->bChkSys = 1;
    pmw->filename[63] = '\0';

    memc_clearMEMCINFO(info);

    re = sceMcGetInfo(pmw->port, pmw->slot, &pmw->type, &pmw->free, &pmw->format);
    if (re == sceMcResSucceed) {
        pmw->func = MEMC_FUNC_GETINFO;
    }
    
    return re;
}

int memc_del_file(int port, int no) {
    int        re;
    MEMC_STAT *pmw = &memc_stat;
    char      *tmpp;

    pmw->cmd = sceMcFuncNoDelete;
    pmw->retry = 0;

    pmw->port = port;
    pmw->fileNo = no;

    tmpp = memc_getfilepath(no);
    if (tmpp != NULL) {
        strcpy(pmw->filename, tmpp);
    } else {
        pmw->filename[0] = '\0';
    }

    re = sceMcDelete(pmw->port, pmw->slot, pmw->filename);
    if (re == sceMcResSucceed) {
        pmw->func = MEMC_FUNC_DELFILE;
    }

    return re;
}

int memc_load_file(int port, int no, char *buf, int size) {
    int        re;
    MEMC_STAT *pmw = &memc_stat;
    char      *tmpp;

    pmw->port        = port;
    pmw->buf         = buf;
    pmw->size        = size;

    pmw->retry       = 0;
    pmw->isSyncClose = 0;
    pmw->fileNo      = no;

    tmpp = memc_getfilepath(no);
    if (tmpp != NULL) {
        strcpy(pmw->filename, tmpp);
    } else {
        pmw->filename[0] = '\0';
    }

    re = sceMcGetInfo(pmw->port, pmw->slot, &pmw->type, &pmw->free, &pmw->format);
    if (re == sceMcResSucceed) {
        pmw->cmd = sceMcFuncNoFileInfo;
        pmw->func = 3;
    }

    pmw->retry = 0;
    return re;
}

int memc_loadFirst(int port, int no, char *buf, int size) {
    MEMC_STAT *pmw = &memc_stat;
    char      *tmpp;

    pmw->port        = port;
    pmw->buf         = buf;
    pmw->size        = size;

    pmw->retry       = 0;
    pmw->isSyncClose = 1;
    pmw->fileNo      = no;

    tmpp = memc_getfilepath(no);
    if (tmpp != NULL) {
        strcpy(pmw->filename, tmpp);
    } else {
        pmw->filename[0] = '\0';
    }

    if (memc_mansub_Open(pmw->filename, SCE_RDONLY) == 0) {
        pmw->func = 3;
        return 0;
    }

    return 1;
}

int memc_save_file(int port, int no, char* buf, int size, int bSysRW) {
    int        re;
    int        n;
    int        isize;

    MEMC_STAT* pmw;
    char*      tmpp;

    pmw = &memc_stat;

    pmw->port    = port;
    pmw->buf     = buf;
    pmw->size    = size;
    pmw->retry   = 0;
    pmw->stat    &= ~0x1f; /* Clear all flags */

    pmw->seek    = 0;
    pmw->size2   = 0;
    pmw->bChkSys = bSysRW;
    pmw->fileNo  = no;

    tmpp = memc_getfilepath(no);
    if (tmpp != NULL) {
        strcpy(pmw->filename, tmpp);
    } else {
        pmw->filename[0] = '\0';
    }

    n = 3;
    isize = (pmw->iconSize1 + 1023) / 1024;

    if (memc_getfilename(-3) != NULL) {
        isize += (pmw->iconSize2 + 1023) / 1024;
        n++;
    }
    if (memc_getfilename(-4) != NULL) {
        isize += (pmw->iconSize3 + 1023) / 1024;
        n++;
    }

    pmw->sysFileSize = isize + ((n + 1) / 2) + 3;

    re = sceMcGetInfo(pmw->port, pmw->slot, &pmw->type, &pmw->free, &pmw->format);
    if (re == sceMcResSucceed) {
        pmw->cmd = sceMcFuncNoFileInfo;
        pmw->func = 4;
    }

    return re;
}

int memc_seeksave_file(int port, int no, char *buf, int size, int seek, int sizef, int bSysRW) {
    MEMC_STAT *pmw = &memc_stat;
    int        re;

    re = memc_save_file(port, no, buf, size, bSysRW);

    pmw->seek = seek;
    pmw->size2 = sizef;
    return re;
}

int memc_save_overwrite(void) {
    int        re;
    int        n;
    int        isize;

    MEMC_STAT *pmw = &memc_stat;

    n = 2;
    isize = (pmw->iconSize1 + 1023) / 1024;

    if (memc_getfilename(-3) != NULL) {
        isize += (pmw->iconSize2 + 1023) / 1024;
        n++;
    }
    if (memc_getfilename(-4) != NULL) {
        isize += (pmw->iconSize3 + 1023) / 1024;
        n++;
    }

    pmw->sysFileSize = isize + ((n + 1) / 2) + 3;

    re = sceMcGetInfo(pmw->port, pmw->slot, &pmw->type, &pmw->free, &pmw->format);
    if (re == sceMcResSucceed) {
        pmw->cmd = sceMcFuncNoFileInfo;
        pmw->func = 14;
    }

    return re;
}

int memc_port_check(int port, int *type, int *free) {
    int        re;
    MEMC_STAT *pmw = &memc_stat;
  
    re = sceMcGetInfo(port, 0, type, free, &pmw->format);
    if (re == sceMcResSucceed) {
        pmw->cmd = sceMcFuncNoFileInfo;
        pmw->func = 1;
    }

    pmw->retry = 0;
    return re;
}

int memc_format(int port) {
    int        re;
    MEMC_STAT *pmw = &memc_stat;

    pmw->port = port;

    re = sceMcGetInfo(port, pmw->slot, &pmw->type, &pmw->free, &pmw->format);
    if (re == sceMcResSucceed) {
        pmw->cmd = sceMcFuncNoFileInfo;
        pmw->func = 7;
    }

    pmw->retry = 0;
    return re;
}

int memc_chg_dir(int port, char *name) {
    int        re;
    MEMC_STAT *pmw = &memc_stat;

    pmw->retry = 0;
    strcpy(pmw->filename, name);

    re = sceMcChdir(port, 0, name, NULL);
    if (re == sceMcResSucceed) {
        pmw->func = 10;
        pmw->cmd = sceMcFuncNoChDir;
    }

    return re;
}

int memc_get_dir(int port, char *name, sceMcTblGetDir *dir, int max) {
    int        re;
    MEMC_STAT *pmw = &memc_stat;

    memset(dir, 0, max * 64);

    pmw->port = port;

    strcpy(pmw->filename, name);

    pmw->size = max;
    pmw->buf = (char*)dir;

    re = sceMcGetDir(pmw->port, pmw->slot, pmw->filename, 0, max, dir);
    if (re == sceMcResSucceed) {
        pmw->func = 9;
        pmw->cmd = sceMcFuncNoGetDir;
        pmw->retry = 0;
    }

    return re;
}

int memc_get_dir_continue(sceMcTblGetDir *dir, int max) {
    int        re;
    MEMC_STAT *pmw = &memc_stat;

    memset(dir, 0, max * 64);

    pmw->size = max;
    pmw->buf = (char*)dir;

    re = sceMcGetDir(pmw->port, pmw->slot, pmw->filename, 1, max, dir);
    if (re == sceMcResSucceed) {
        pmw->func = 9;
        pmw->cmd = sceMcFuncNoGetDir;
        pmw->retry = 0;
    }

    return re;
}

static int memc_mansub_ErrChk(int result) {
    MEMC_STAT *pmw = &memc_stat;

    if (pmw->func == 9 && result >= pmw->size) {
        return 0x13;
    }

    if (result >= 0) {
        pmw->func = 0;
        return 0;
    }

    switch (result) {
    case -1:
        pmw->func = 0;
        pmw->isChange = 1;
        return 6;
    case -2:
        pmw->func = 0;
        return 3;
    /* (pmw->func==1||pmw->cmd==sceMcFuncNoFileInfo) -- sceMcResNoFormat */
    case -2000:
        pmw->func = 0;
        pmw->isChange = 1;
        return 0x30;
    case -3:
        pmw->func = 0;
        return 4;
    case sceMcResNoEntry:
        pmw->func = 0;
        switch (pmw->cmd) {
        case sceMcFuncNoMkdir:
            return 0;
        case sceMcFuncNoGetDir:
            return 0x11;
        case sceMcFuncNoOpen:
        case sceMcFuncNoDelete:
            return 5;
        }
        return 1;
    default:
        pmw->func = 0;
        return 2;
    }
}

static int memc_mansub_Open(char *name, u_int type) {
    int        re;
    MEMC_STAT *pmw = &memc_stat;

    re = sceMcOpen(pmw->port, pmw->slot, name, type);
    if (re == sceMcResSucceed) {
        pmw->cmd = sceMcFuncNoOpen;
    }

    return re;
}

static int memc_mansub_Close(void) {
    int        re;
    MEMC_STAT *pmw = &memc_stat;

    re = sceMcClose(pmw->fd);
    if (re == sceMcResSucceed) {
        pmw->cmd = sceMcFuncNoClose;
    }

    return re;
}

static int memcsub_fileChk(sceMcTblGetDir *dir, unsigned char *name, int max) {
    int i, j, s;

    if (name == NULL) {
        return -1;
    }

    s = 0;

    for (i = 0; i < max; i++) {
        for (j = 0; j < 64; j++) {
            if (name[j] == '\0') {
                s++;
                break;
            }
            if (dir[i].EntryName[j] != name[j]) {
                break;
            }
        }
    }

    return s;
}

static int memc_mansub_GetInfo(int result) {
    MEMC_INFO *info;
    MEMC_STAT *pmw = &memc_stat;

    int re;

    switch (pmw->cmd) {
    case sceMcFuncNoFileInfo:
        if (pmw->type != sceMcTypePS2) {
            pmw->func = 0;
            return 2;
        } else {
            if (result < 0) {
                pmw->func = 0;

                if (result != sceMcResNoFormat) {
                    re = result;
                } else {
                    re = -2000;
                }

                return memc_mansub_ErrChk(re);
            } else {
                if (pmw->format == 0) {
                    pmw->func = 0;
                    return 3;
                } else {
                    info = (MEMC_INFO*)pmw->buf;

                    info->free = pmw->free;
                    info->flag |= _memc_type[pmw->type];

                    if (sceMcGetDir(pmw->port, pmw->slot, pmw->filename, 0, info->dirfileMax, info->dirfile) == sceMcResSucceed) {
                        pmw->cmd = sceMcFuncNoGetDir;
                        return 16;
                    }

                    return 1;
                }
            }
        }
        break;
    case sceMcFuncNoGetDir:
        if (result < 0) {
            pmw->func = 0;
            return memc_mansub_ErrChk(result);
        } else {
            info = (MEMC_INFO*)pmw->buf;

            info->allfile = result;
            info->flag |= 1;

            if (pmw->bChkSys) {
                if (memcsub_fileChk(info->dirfile, memc_getfilename(-1), result)) {
                    info->flag |= 2;
                }
                if (memcsub_fileChk(info->dirfile, memc_getfilename(-2), result)) {
                    info->flag |= 4;
                }
                if (memcsub_fileChk(info->dirfile, memc_getfilename(-3), result)) {
                    info->flag |= 8;
                }
                if (memcsub_fileChk(info->dirfile, memc_getfilename(-4), result)) {
                    info->flag |= 16;
                }
                
                info->savefile  = memcsub_fileChk(info->dirfile, pmw->saveDir, result);
                info->savefile += memcsub_fileChk(info->dirfile, "SAVE", result);
            }

            pmw->func = 0;
            return 0;
        }
        break;
    default:
        return 0;
    }
}

static int memc_mansub_load(int result) {
    MEMC_STAT *pmw = &memc_stat;

    switch (pmw->cmd) {
    case sceMcFuncNoFileInfo:
        if (result < 0) {
            return memc_mansub_ErrChk(result);
        }

        if (pmw->type != 2) {
            pmw->func = 0;
            return 2;
        }

        if (!memc_mansub_Open(pmw->filename, 1)) {
            pmw->func = 3;
            break;
        }

        return 1;
    case sceMcFuncNoOpen:
        if (result < 0) {
            return memc_mansub_ErrChk(result);
        }

        pmw->fd = result;
        if (!sceMcRead(pmw->fd, pmw->buf, pmw->size)) {
            pmw->cmd = sceMcFuncNoRead;
            return 16;
        }

        return 16;
    case sceMcFuncNoRead:
        if (result < 0) {
            return memc_mansub_ErrChk(result);
        }

        if (memc_mansub_Close()) {
            return 1;
        }

        pmw->func = 6;
        if (!pmw->isSyncClose) {
            return 16;
        }

        pmw->isSyncClose = 0;
        if (sceMcSync(0, NULL, &result) != 1) {
            return 16;
        }

        return memc_mansub_ErrChk(result);
    }

    return 16;
}

static int memc_manager_save(int result) {
    int re;
    MEMC_STAT *pmw = &memc_stat;
    char name[64];
    char *fname;
    int i;
    int iscls;
    int isfn;
    sceMcTblGetDir *owDir;
    int isSysOWrite;

    switch (pmw->cmd) {
    case 0xe:
        if (result == 0) {
            if (pmw->format == 0) {
                result = -2;
            }
        }

        if (result < 0) {
            return memc_mansub_ErrChk(result);
        }

        if (pmw->type != 2) {
            pmw->func = 0;
            return 2;
        }

        strcpy(name, pmw->saveDir);
        strcpy(name + strlen(pmw->saveDir), "/*");

        if (sceMcGetDir(pmw->port, pmw->slot, name, 0, 8, pmw->curDir) == 0) {
            pmw->cmd = 0xd;
        } else {
            pmw->func = 0;
            pmw->cmd = 0;
        }

        break;

    case 0xd:
        if (result == 0) {
            if (pmw->format == 0) {
                result = -2;
            }
        }

        if (result == -4) {
            result = 0;
        }

        if (result < 0) {
            return memc_mansub_ErrChk(result);
        }

        if (pmw->type != 2) {
            pmw->func = 0;
            return 2;
        }

        pmw->oldOWClust = 0;

        if (result >= 2) {
            isSysOWrite = FALSE;
            iscls = 0;
            isfn = 0;

            for (i = 0; i < result; i++) {
                if (pmw->curDir[i].EntryName[0] != '.') {
                    iscls += ((pmw->curDir[i].FileSizeByte + 0x3ff) >> 0xa);
                    isfn++;
                }
            }

            pmw->oldOWClust = (iscls + (isfn / 2)) + 2;

            if (pmw->bChkSys ||
                memc_searchDirTbl(memc_getfilename(-1), pmw->curDir, result, TRUE, sizeof(sceMcIconSys), NULL) == NULL ||
                memc_searchDirTbl(memc_getfilename(-2), pmw->curDir, result, TRUE, pmw->iconSize1,       NULL) == NULL ||
                ((fname = memc_getfilename(-3)) != NULL &&
                    memc_searchDirTbl(fname, pmw->curDir, result, TRUE, pmw->iconSize2, NULL) == NULL) ||
                ((fname = memc_getfilename(-4)) != NULL &&
                    memc_searchDirTbl(fname, pmw->curDir, result, TRUE, pmw->iconSize3, NULL) == NULL)) {
                isSysOWrite = TRUE;
            }

            pmw->sizeOW = 0;

            if ((owDir = memc_searchDirTbl(memc_getfilename(pmw->fileNo), pmw->curDir, result, FALSE, 0, NULL)) != NULL) {
                pmw->sizeOW = owDir->FileSizeByte;
                pmw->func = 0xd;
                return 0x12;
            }

            if (isSysOWrite) {
                pmw->func = 4;
                pmw->stat |= 0x3;
                memc_mansub_Open(memc_getfilepath(-1), 0x202);
                break;
            }

            re = memc_SaveFileClust();
            if (pmw->free < re) {
                pmw->func = 0;
                return 4;
            } else {
                if (memc_mansub_Open(pmw->filename, 0x202) == 1) {
                    pmw->func = 0;
                    return 4;
                }
            }
        } else {
            re = memc_SaveFileClust();

            if (pmw->free < (re + pmw->sysFileSize)) {
                pmw->func = 0;
                return 4;
            } else {

                if (sceMcMkdir(pmw->port, pmw->slot, pmw->saveDir) == 0) {
                    pmw->cmd = 0xb;
                    pmw->stat |= 0x1;
                    break;
                } else {
                    pmw->func = 0;
                    return 1;
                }
            }
        }

        break;
    case 0xb:
        if (result == 0) {
            if (pmw->format == 0) {
                result = -2;
            }
        }

        if (result < 0) {
            return memc_mansub_ErrChk(result);
        }

        if (pmw->type != 2) {
            pmw->func = 0;
            return 2;
        }

        if ((re = memc_mansub_ErrChk(result)) != 0) {
            return re;
        }

        pmw->func = 4;
        pmw->stat |= 0x2;
        memc_mansub_Open(memc_getfilepath(-1), 0x202);
        break;
    case 2:
        if (result == 0) {
            if (pmw->format == 0) {
                result = -2;
            }
        }

        if (result < 0) {
            return memc_mansub_ErrChk(result);
        }

        if (pmw->type != 2) {
            pmw->func = 0;
            return 2;
        }

        pmw->fd = result;

        if (pmw->stat & 0x10) {
            if (sceMcWrite(result, pmw->iconData3, pmw->iconSize3) == 0) {
                pmw->cmd = 6;
            }

            break;
        }

        if (pmw->stat & 0x8) {
            if (sceMcWrite(result, pmw->iconData2, pmw->iconSize2) == 0) {
                pmw->cmd = 6;
            }

            break;
        }

        if (pmw->stat & 0x4) {
            if (sceMcWrite(result, pmw->iconData1, pmw->iconSize1) == 0) {
                pmw->cmd = 6;
            }

            break;
        }

        if (pmw->stat & 0x2) {
            if (sceMcWrite(result, &memc_iconsys, sizeof(sceMcIconSys)) == 0) {
                pmw->cmd = 6;
            }

            break;
        }

        if (sceMcWrite(result, pmw->buf, pmw->size) == 0) {
            if (pmw->seek > 0) {
                pmw->cmd = 0x1000e;
            } else {
                pmw->cmd = 6;
            }
        }

        break;

    case 0x1000e:
        if (result == 0) {
            if (pmw->format == 0) {
                result = -2;
            }
        }

        if (result < 0) {
            return memc_mansub_ErrChk(result);
        }

        if (pmw->type != 2) {
            pmw->func = 0;
            return 2;
        }

        sceMcSeek(pmw->fd, pmw->seek, 0);
        pmw->cmd = 0x1000f;
        break;
    case 0x1000f:
        if (result == 0) {
            if (pmw->format == 0) {
                result = -2;
            }
        }

        if (result < 0) {
            return memc_mansub_ErrChk(result);
        }

        if (pmw->type != 2) {
            pmw->func = 0;
            return 2;
        }

        if (sceMcWrite(pmw->fd, pmw->buf + pmw->size, pmw->size2) == 0) {
            pmw->cmd = 6;
        }

        break;
    case 6:
        if (result == 0) {
            if (pmw->format == 0) {
                result = -2;
            }
        }

        if (result < 0) {
            return memc_mansub_ErrChk(result);
        }

        if (pmw->type != 2) {
            pmw->func = 0;
            return 2;
        }

        sceMcFlush(pmw->fd);
        pmw->cmd = 0xa;
        break;
    case 0xa:
        if (memc_mansub_Close() == 0) {
            if (!(pmw->stat & 0x1)) {
                pmw->func = 6;
            }
        } else {
            pmw->func = 0;
            return 1;
        }

        break;
    case 3:
        if (result == 0) {
            char *fname;

            if (!(pmw->stat & 0x4)) {
                pmw->stat |= 0x4;
                memc_mansub_Open(memc_getfilepath(-2), 0x202);
                break;
            }

            if (!(pmw->stat & 0x8)) {
                if ((fname = memc_getfilepath(-3)) != NULL) {
                    pmw->stat |= 0x8;
                    memc_mansub_Open(fname, 0x202);
                    break;
                }
            }

            if (!(pmw->stat & 0x10)) {
                if ((fname = memc_getfilepath(-4)) != NULL) {
                    pmw->stat |= 0x10;
                    memc_mansub_Open(fname, 0x202);
                    break;
                }
            }

            pmw->stat &= ~0x1f;
            memc_mansub_Open(pmw->filename, 0x202);
        } else {
            pmw->func = 0;
            return 1;
        }

        break;
    }

    return 0x10;
}

static int memc_manager_overwrite(int result) {
    MEMC_STAT *pmw = &memc_stat;
    int size, need;
    char *fname;
    int func;

    if (pmw->cmd != 0xe) {
        return 0x10;
    }

    if ((result = memc_mansub_ErrChk(result)) == 0) {
        func = 4;
        size = memc_SaveFileClust() + pmw->sysFileSize;
        need = size - pmw->oldOWClust;

        if (pmw->free < need) {
            pmw->func = 0;
            return func;
        }

        if (pmw->bChkSys ||
            memc_searchDirTbl(memc_getfilename(-1), pmw->curDir, 0, 1, sizeof(sceMcIconSys), NULL) == NULL ||
            memc_searchDirTbl(memc_getfilename(-2), pmw->curDir, 0, 1, pmw->iconSize1,       NULL) == NULL ||
            ((fname = memc_getfilename(-3)) != NULL &&
                memc_searchDirTbl(fname, pmw->curDir, 0, 1, pmw->iconSize2, NULL) == NULL) ||
            ((fname = memc_getfilename(-4)) != NULL &&
                memc_searchDirTbl(fname, pmw->curDir, 0, 1, pmw->iconSize3, NULL) == NULL)) {
            pmw->func = 4;
            pmw->stat |= 0x3;
            memc_mansub_Open(memc_getfilepath(-1), 0x202);
            return 0x10;
        }

        if (memc_mansub_Open(pmw->filename, 0x202) == 1) {
            pmw->func = 0;
            return 4;
        } else {
            pmw->func = func;
            return 16;
        }
    }

    return result;
}

static int memc_manager_chk(int mode) {
    int        re;
    int        result;
    MEMC_STAT *pmw;

    pmw = &memc_stat;

    re = sceMcSync(mode, 0, &result);
    if (re == sceMcExecFinish) {
        switch (pmw->func) {
        case 3:
            return memc_mansub_load(result);
        case 4:
            return memc_manager_save(result);
        case 1:
            if (result == sceMcResNoFormat) {
                result = -2000;
            }
            return memc_mansub_ErrChk(result);
        case 7:
            switch (pmw->cmd) {
            case sceMcFuncNoFileInfo:
                if (result >= 0) {
                    if (pmw->type != 2) {
                        pmw->func = 0;
                        return 2;
                    }

                    re = sceMcFormat(pmw->port, pmw->slot);
                    if (re == 0) {
                        pmw->cmd = sceMcFuncNoFormat;
                        break;
                    }

                    pmw->func = 0;
                    pmw->cmd = 0;
                    return 1;
                }

                return memc_mansub_ErrChk(result);
            case sceMcFuncNoFormat:
                if (result >= 0) {
                    pmw->func = 0;
                    return 0;
                }

                pmw->retry++;
                if (pmw->retry >= 30) {
                    pmw->func = 0;
                    return 1;
                }

                sceMcFormat(pmw->port, pmw->slot);
                break;
            }
            break;
        case 2:
        case 6:
        case 8:
        case 9:
        case 10:
        case MEMC_FUNC_DELFILE:
            return memc_mansub_ErrChk(result);
        case MEMC_FUNC_GETINFO:
            return memc_mansub_GetInfo(result);
        case 14:
            return memc_manager_overwrite(result);
        case 0:
        case 5:
        default:
            pmw->func = 0;
            break;
        case 13:
            return 0x10;
        }
    }

    if (re == sceMcExecIdle) {
        pmw->func = 0;
        return 0;
    }

    return 0x10;
}

int memc_manager(int mode) {
    int re;

    if (mode == 1) {
        re = memc_manager_chk(1);
    } else {
        do {
            re = memc_manager_chk(mode);
        } while (re == 0x10);
    }

    return re;
}
