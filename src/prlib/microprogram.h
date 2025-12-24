#ifndef PRLIB_MICROPROGRAM_H
#define PRLIB_MICROPROGRAM_H

#include "common.h"

#include <eetypes.h>

enum PrMICRO_PROGRAM_MODULE {
    PR_MICRO_PROGRAM_NORMAL,
    PR_MICRO_PROGRAM_BOTHFACE,
    PR_MICRO_PROGRAM_CONTOUR,
    PR_MICRO_PROGRAM_REFMAP,
    PR_MICRO_PROGRAM_SCREEN,
    PR_MICRO_PROGRAM_ANTILINE,
    PR_MICRO_PROGRAM_MAX,
};

void PrLoadMicroPrograms();
u_int PrGetMicroProgramAddress(PrMICRO_PROGRAM_MODULE module);

u_int PrGetInputVertexParameterNum(PrMICRO_PROGRAM_MODULE module);
u_int PrGetInputVertexParameterSet(PrMICRO_PROGRAM_MODULE module);

u_int PrGetOutputVertexParameterNum(PrMICRO_PROGRAM_MODULE module);
u_int PrGetOutputVertexParameterSet(PrMICRO_PROGRAM_MODULE module);

u_int PrGetMendererCreateTextureAddress();
u_int PrGetMendererDrawMeshAddress();

#endif /* PRLIB_MICROPROGRAM_H */
