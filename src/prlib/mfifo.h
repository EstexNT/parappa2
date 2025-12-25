#ifndef PRLIB_MFIFO_H
#define PRLIB_MFIFO_H

#include "common.h"

#include <eetypes.h>
#include <libdma.h>

void PrInitializeMfifo();
void PrCleanupMfifo();
void PrStartMfifo();
void PrStopMfifo();
void PrWaitMfifo();
void PrSendMfifo(const sceDmaTag *tag);
u_int PrMfifoUnsentDataSize();

#endif /* PRLIB_MFIFO_H */
