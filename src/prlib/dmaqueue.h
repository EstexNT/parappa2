#ifndef DMAQUEUE_H
#define DMAQUEUE_H

#include "common.h"

#include <eetypes.h>
#include <libdma.h>

/* TODO(poly): Perhaps move to the common header? */
#define PR_DECACHE(addr) (void*)((u_int)(addr) & 0x0fffffff)

struct PrDmaList {
    int stall_qw[4];

    sceDmaTag stall_tag;
    sceDmaTag call_tag;
    sceDmaTag next_tag;
} PR_ALIGNED(128);

class PrDmaQueue {
public:
    PrDmaQueue(u_int size);
    ~PrDmaQueue();

    void Initialize();
    void Start();
    void Append(void *tag);
    void Wait();

private:
    PrDmaList *mQueue;
    u_int mSize;
    bool mStarted;
    int mPos;
};

#endif /* DMAQUEUE_H */
