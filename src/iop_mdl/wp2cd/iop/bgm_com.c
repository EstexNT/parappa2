#include "../wp2cd.h"

#include <intrman.h>
#include <libcdvd.h>
#include <sifrpc.h>
#include <thread.h>

#include <stdio.h>

static void* bgmFunc(unsigned int command, void *data, int size);

int gRpcArg[16];

int sce_bgm_loop(void) {
    sceSifQueueData qd;
    sceSifServeData sd;

    CpuEnableIntr();
    EnableIntr(INUM_DMA_4);
    EnableIntr(INUM_DMA_7);
    EnableIntr(INUM_SPU);

    sceSifInitRpc(0);

    sceSifSetRpcQueue(&qd, GetThreadId());
    sceSifRegisterRpc(&sd, WP2CD_DEV, bgmFunc, gRpcArg, NULL, NULL, &qd);

    sceSifRpcLoop(&qd);
    return 0;
}

int ret = 0;

static void* bgmFunc(unsigned int command, void *data, int size) {
    switch (command) {
    case WP2_INIT:
        ret = BgmInit(*(int*)data);
        break;
    case WP2_QUIT:
        BgmQuit(*(int*)data);
        break;
    case WP2_OPEN:
        ret = BgmOpen(data);
        break;
    case WP2_CLOSE:
        BgmClose(*(int*)data);
        break;
    case WP2_PRELOAD:
        ret = BgmPreLoad();
        break;
    case WP2_START:
        ret = BgmStart();
        break;
    case WP2_STOP:
        BgmStop(*(int*)data);
        break;
    case WP2_SEEK:
        ret = BgmSeek(*(int*)data);
        break;
    case WP2_SETVOLUME:
        BgmSetVolume(*(int*)data);
        break;
    case WP2_SETVOLDIRECT:
        BgmSetVolumeDirect(*(int*)data);
        break;
    case WP2_SETMASTERVOL:
        BgmSetMasterVolume(*(int*)data);
        break;
    case WP2_SETMODE:
        BgmSetMode(*(int*)data);
        break;
    case WP2_GETMODE:
        ret = BgmGetMode();
        break;
    case WP2_SDINIT:
        BgmSdInit(*(int*)data);
        break;
    case WP2_SETCHANNEL:
        BgmSetChannel(*(int*)data);
        break;
    case WP2_CDINIT:
        BgmCdInit(*(int*)data);
        break;
    case WP2_GETTIME:
        ret = BgmGetTime();
        break;
    case WP2_GETTIMESAMPLE:
        ret = BgmGetTSample();
        break;
    case WP2_GETCDERRCODE:
        ret = BgmGetCdErrCode();
        break;
    case WP2_OPENFLOC:
        ret = BgmOpenFLoc(data);
        break;
    case WP2_SEEKFLOC:
        ret = BgmSeekFLoc(data);
        break;
    case WP2_PRELOADBACK:
        BgmPreLoadBack();
        break;
    case WP2_SETTRPOINT:
        BgmSetTrPoint(*(int*)data);
        break;
    case WP2_READBUFF:
        ret = BgmReadBuffFull();
        break;
    default:
        printf("EzBGM driver error: unknown command %d \n", *(int*)data);
        break;
    }

    return &ret;
}
