#include "renderstuff.h"

#if defined(PRD_SYORI)
#include "dbug/syori.h"
#endif

#include "gifreg.h"
#include "mfifo.h"
#include "microprogram.h"
#include "scene.h"

#include <eeregs.h>
#include <libgraph.h>

#include <stdlib.h>

PrRenderStuff::PrRenderStuff() : mDmaQueue(1200) {
    mTransmitArraySize = 0;
    mTransmitArrayMax = 0;
    mTransmitArray = NULL;
    AllocateTransmitDmaArray(600);

    mScene = NULL;
}

PrRenderStuff::~PrRenderStuff() {
    Cleanup();
}

void PrRenderStuff::Initialize(sceGsZbuf zbuf) {
    mZbuf = zbuf;
    mScene = NULL;
    this->unk28 = 0;

    PrLoadMicroPrograms();

    PrInitializeDmaStripGifRegister(zbuf);
    PrInitializeMfifo();

    /*
     * Disable VIF DMAtag mismatch errors to
     * avoid HW bug where these are triggered
     * with valid packets, causing stalls.
     *
     * Though these are already guaranteed
     * to be set from the sceDevVif0Reset()
     * and sceGsResetPath() calls on
     * InitSystem() (os/system.c).
     */
    *VIF0_ERR |= (1<<1);
    *VIF1_ERR |= (1<<1);
}

void PrRenderStuff::Cleanup() {
    PrCleanupMfifo();

    delete mTransmitArray;
    mTransmitArraySize = 0;
    mTransmitArrayMax = 0;
    mTransmitArray = NULL;

    mScene = NULL;
}

u_int PrRenderStuff::GetZbufBits(void) const {
    switch (mZbuf.PSM) {
    case 0:  /* PSMZ32 */
        return 32;
    case 1:  /* PSMZ24 */
        return 24;
    case 2:  /* PSMZ16 */
        return 16;
    case 10: /* PSMZ16S */
        return 16;
    default:
        break;
    }

    return 0;
}

void PrRenderStuff::ResetStatistics() {
    mStatistics.node_num = 0;

    mStatistics.opaque_context1_node_num = 0;
    mStatistics.transmit_context1_node_num = 0;
    mStatistics.opaque_context2_node_num = 0;
    mStatistics.transmit_context2_node_num = 0;
    
    mStatistics.render_time0 = 0;
    mStatistics.render_time1 = 0;
    mStatistics.render_time2 = 0;
    mStatistics.render_time3 = 0;
    mStatistics.render_time4 = 0;
    mStatistics.render_time5 = 0;
    mStatistics.render_time6 = 0;
    mStatistics.render_time7 = 0;
    mStatistics.render_time8 = 0;

    mStatistics.dynamic_append_transmit_node = false;
}

void PrRenderStuff::StartRender(PrSceneObject* scene) {
    mDmaQueue.Start();
    mScene = scene;
}

void PrRenderStuff::WaitRender() {
    bool noSync = false;

    mDmaQueue.Wait();
    PrStopMfifo();

    mStatistics.render_time6 = *T3_COUNT;

    if (mScene->GetFocalLength() != 0.0f) {
        sceGsSyncPath(0, 0);
        mScene->ApplyDepthOfField();
        sceGsSyncPath(0, 0);
        noSync = true;
    }

    if (mScene->mScreenModelList != NULL) {
        mScene->PrepareScreenModelRender();
        mDmaQueue.Wait();
        noSync = false;
    }

    if (!noSync) {
        sceGsSyncPath(0, 0);
    }

    mStatistics.render_time7 = *T3_COUNT;

    mScene = NULL;

#if defined(PRD_SYORI)
    /*
     * By pushing statistics here we assume
     * that any call to PrRender() will also
     * immediately call PrWaitRender()
     * (which is indeed the case here).
     */
    SyoriPushStats(&mStatistics);
#endif
}

void PrRenderStuff::AllocateTransmitDmaArray(u_int size) {
    if (mTransmitArrayMax >= size) {
        return;
    }

    int elems = 600;
    while (elems < size) {
        elems *= 2;
    }

    delete mTransmitArray;
    mTransmitArray = new PrTransmitEntry[elems];
    mTransmitArrayMax = elems;
}

void PrRenderStuff::AppendTransmitDmaTag(const sceDmaTag* tag, u_int arg1, float arg2) {
    extern bool warned_tmp_renderstuff;

    if (mTransmitArraySize >= mTransmitArrayMax) {
        if (!warned_tmp_renderstuff) {
            warned_tmp_renderstuff = true;
            return;
        }
    } else {
        mTransmitArray[mTransmitArraySize].unk0 = arg2;
        mTransmitArray[mTransmitArraySize].unk4 = arg1;
        mTransmitArray[mTransmitArraySize].tag = tag;
        mTransmitArraySize++;
    }
}

int PrRenderStuff::CompareFunction(const void* arg0, const void* arg1) {
    PrTransmitEntry* a0 = (PrTransmitEntry*)arg0;
    PrTransmitEntry* a1 = (PrTransmitEntry*)arg1;

    if (a0->unk4 != a1->unk4) {
        return (a0->unk4 >= a1->unk4) ? 1 : -1;
    }

    if (a0->unk0 == a1->unk0) {
        return 0;
    } else if (a1->unk0 < a0->unk0) {
        return 1;
    }

    return -1;
}

void PrRenderStuff::SortTransmitDmaArray() {
    if (mTransmitArraySize > 1) {
        qsort(mTransmitArray, mTransmitArraySize, sizeof(PrTransmitEntry), CompareFunction);
    }
}

void PrRenderStuff::MergeRender() {
    bool first = false;

    for (int i = 0; i < mTransmitArraySize; i++) {
        if (!first && mTransmitArray[i].unk4 == -1) {
            PrDmaStripForSetGifRegister* strip = PrGetDmaStripGifRegister(eGifRegisterMode_Unk1);
            AppendDmaTag(&strip->mTag);
            first = true;
        }

        AppendDmaTag(mTransmitArray[i].tag);
    }

    if (prCurrentStage == 19) {
        PrDmaStripForSetGifRegister* strip = PrGetDmaStripGifRegister(eGifRegisterMode_Unk1);
        AppendDmaTag(&strip->mTag);
    }
}

INCLUDE_ASM("asm/nonmatchings/prlib/renderstuff", _GLOBAL_$D$prRenderStuff);

INCLUDE_ASM("asm/nonmatchings/prlib/renderstuff", _GLOBAL_$I$prRenderStuff);
