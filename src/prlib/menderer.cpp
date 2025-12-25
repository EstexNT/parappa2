#include "common.h"

#if defined(PRD_SYORI)
#include "dbug/syori.h"
#endif

#include "renderstuff.h"

#include <eeregs.h>

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", InitializeNoodleStripRendering__FUiUiUiUi);

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", GetRandom__Fv);

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", StageIndexForColor__Fv);

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", PushNoodleColor__FPUl);

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", DrawNoodleStripChunk__FRCt8NaMATRIX3Zfi4i4);

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", SetNoodleRotationMatrix__FRt8NaMATRIX3Zfi4i4f);

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", PreDrawNoodleStrip__Fv);

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", DrawNoodleStrip__Fff);

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", StartNoodleRotation__Fv);

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", UpdateNoodleRotation__Fv);

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", PrUpdateMendererSpeed__Fv);

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", PrDecelerateMenderer);

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", PrRestartMenderer);

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", DrawMenderer__Fv);
void DrawMenderer();

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", PrSetMendererRatio);

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", PrGetMendererRatio);

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", PrSetMendererDirection);

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", PrGetMendererDirection);

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", PrSetMendererColorModulation);

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", PrIsMendererColorModulation);

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", PrInitializeMenderer);

PR_EXTERN
void PrRenderMenderer() {
    DrawMenderer();
    prRenderStuff.m_statistics.render_time8 = *T3_COUNT;
#if defined(PRD_SYORI)
    SyoriUpdateStats(&prRenderStuff.mStatistics);
#endif
}

/* nalib/navector.h */
INCLUDE_ASM("asm/nonmatchings/prlib/menderer", func_0014F3B8);

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", func_0014F410);

/* prlib/menderer.cpp */
PR_EXTERN
void _GLOBAL_$I$prMendererRatio(void) {
}

/* nalib/navector.h */
INCLUDE_ASM("asm/nonmatchings/prlib/menderer", func_0014F4C8);

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", func_0014F5D0);

INCLUDE_ASM("asm/nonmatchings/prlib/menderer", func_0014F6D8);

/* prlib/menderer.cpp */
INCLUDE_RODATA("asm/nonmatchings/prlib/menderer", D_003967E0);

INCLUDE_RODATA("asm/nonmatchings/prlib/menderer", D_003967F0);
