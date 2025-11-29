#include "../wp2cd.h"

#include <intrman.h>
#include <loadcore.h>
#include <sif.h>
#include <sifrpc.h>
#include <thread.h>

#include <stdio.h>

ModuleInfo Module = { "WaveP2_driver", 0x100 };

int start(void) {
    struct ThreadParam param;
    int th;

    CpuEnableIntr();

    if (!sceSifCheckInit()) {
        sceSifInit();
    }

    sceSifInitRpc(0);

    printf("WaveP2 driver version 1.2.0\n");

    param.attr         = TH_C;
    param.entry        = sce_bgm_loop;
    param.initPriority = 0x57;
    param.stackSize    = 0x800;
    param.option       = 0;

    th = CreateThread(&param);
    if (th > 0) {
        StartThread(th, NULL);
        printf(" WaveP2 loader thread \n");
        return RESIDENT_END;
    } else {
        return NO_RESIDENT_END;
    }
}
