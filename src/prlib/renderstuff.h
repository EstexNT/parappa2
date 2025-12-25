#ifndef PRLIB_RENDERSTUFF_H
#define PRLIB_RENDERSTUFF_H

#include "common.h"

#include "prpriv.h"
#include "dmaqueue.h"

#include <eetypes.h>
#include <eestruct.h>
#include <libdma.h>

class PrSceneObject;

struct PrVuNodeHeaderDmaPacket;
struct PrVuDataChunkPacketHeader;

struct PrTransmitEntry {
    float unk0;
    u_int unk4;
    const sceDmaTag *tag;
};

class PrRenderStuff {
public:
    PrRenderStuff();
    ~PrRenderStuff();

    void Initialize(sceGsZbuf zbuf);
    void Cleanup();

    u_int GetZbufBits(void) const;
    void ResetStatistics();

    void StartRender(PrSceneObject *scene);
    void WaitRender();

    void AllocateTransmitDmaArray(u_int size);
    void AppendTransmitDmaTag(const sceDmaTag *tag, u_int arg1, float arg2);

    static int CompareFunction(const void *arg0, const void *arg1);
    void SortTransmitDmaArray();

    void MergeRender();

    void InitializeEECore(PrSceneObject *scene);

    void RenderVertexEECoreBothface();
    void RenderVertexEECoreNormal();
    void RenderVertexEECoreRefmap();
    void RenderVertexEECoreContour();

    void RenderNodeEECore(PrVuNodeHeaderDmaPacket *arg0);
    void RenderChunkEECore(PrVuDataChunkPacketHeader *arg0, float arg1);

public:
    void AppendDmaTag(const sceDmaTag *tag) {
        mDmaQueue.Append((void*)tag);
    }

public:
    PrDmaQueue mDmaQueue;
    u_int mTransmitArraySize;
    int mTransmitArrayMax;
    PrTransmitEntry *mTransmitArray;
    PrSceneObject *mScene;
    sceGsZbuf mZbuf;
    int unk28;
    PrRENDERING_STATISTICS mStatistics;
};

extern PrRenderStuff prRenderStuff;

#endif /* PRLIB_RENDERSTUFF_H */
