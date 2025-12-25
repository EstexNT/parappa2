#include "model.h"

#include <nalib/navector.h>

INCLUDE_ASM("asm/nonmatchings/prlib/model", PrSetPostureWorkArea);

INCLUDE_ASM("asm/nonmatchings/prlib/model", AllocateFromWorkArea__FUi);

INCLUDE_ASM("asm/nonmatchings/prlib/model", __13PrModelObjectP13SpmFileHeader);

INCLUDE_ASM("asm/nonmatchings/prlib/model", _$_13PrModelObject);

INCLUDE_ASM("asm/nonmatchings/prlib/model", Initialize__13PrModelObject);

INCLUDE_ASM("asm/nonmatchings/prlib/model", LinkAnimation__13PrModelObjectP13SpaFileHeader);

INCLUDE_ASM("asm/nonmatchings/prlib/model", CleanupAnimation__13PrModelObject);

INCLUDE_ASM("asm/nonmatchings/prlib/model", LinkPositionAnimation__13PrModelObjectP13SpaFileHeader);

INCLUDE_ASM("asm/nonmatchings/prlib/model", CleanupPositionAnimation__13PrModelObject);

void PrModelObject::UnionBoundaryBox(NaVECTOR<float, 4> *arg0, NaVECTOR<float, 4> *arg1) {
    asm volatile(
        "lqc2       $vf13,   0x0(%0)       \n\t"
        "lqc2       $vf14,  0x10(%0)       \n\t"
        "lqc2       $vf15,  0x20(%0)       \n\t"
        "lqc2       $vf16,  0x30(%0)       \n\t"
    : : "r"(&this->unk10));

    asm volatile(
        "lqc2       $vf17,  0x0(%0)        \n\t"
        "vmulax     ACC,    $vf13,  $vf17  \n\t"
        "vmadday    ACC,    $vf14,  $vf17  \n\t"
        "vmaddaz    ACC,    $vf15,  $vf17  \n\t"
        "vmaddw     $vf17,  $vf16,  $vf17  \n\t"
    : : "r"(&mSpmImage->unk30));

    asm volatile(
        "lqc2       $vf04,  0x0(%0)        \n\t"
        "vmini.xyz  $vf04,  $vf04,  $vf17  \n\t"
        "sqc2       $vf04,  0x0(%0)        \n\t"
    : : "r"(arg0));

    asm volatile(
        "lqc2       $vf17,  0x0(%0)        \n\t"
        "vmulax     ACC,    $vf13,  $vf17  \n\t"
        "vmadday    ACC,    $vf14,  $vf17  \n\t"
        "vmaddaz    ACC,    $vf15,  $vf17  \n\t"
        "vmaddw     $vf17,  $vf16,  $vf17  \n\t"
    : : "r"(&mSpmImage->unk40));

    asm volatile(
        "lqc2       $vf04,  0x0(%0)        \n\t"
        "vmax.xyz   $vf04,  $vf04,  $vf17  \n\t"
        "sqc2       $vf04,  0x0(%0)        \n\t"
    : : "r"(arg1));
}

INCLUDE_ASM("asm/nonmatchings/prlib/model", GetPrimitivePosition__13PrModelObjectPt8NaVECTOR2Zfi4);

INCLUDE_ASM("asm/nonmatchings/prlib/model", GetScreenPosition__13PrModelObjectPt8NaVECTOR2Zfi4);

/* nalib/navector.h */
INCLUDE_ASM("asm/nonmatchings/prlib/model", func_00140E38);
