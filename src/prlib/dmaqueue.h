#ifndef DMAQUEUE_H
#define DMAQUEUE_H

#include "common.h"

#include <eetypes.h>
#include <libdma.h>

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
    PrDmaList *m_queue;
    u_int m_size;
    bool m_started;
    int m_pos;
};

#endif /* DMAQUEUE_H */
