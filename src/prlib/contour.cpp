#include "model.h"

void PrModelObject::SaveContour() {
    SpmFileHeader *spm = mSpmImage;
    if (spm->unk70 == 0 || !mRenderedOnce) {
        return;
    }

    u_int nodeNum = spm->mNodeNum;
    for (int i = 0; i < nodeNum; i++) {
        SpmNode *node = spm->mNodes[i];
        if (node->mFlags & 0x40) {
            SpmComplexNode *complex = reinterpret_cast<SpmComplexNode*>(node);
            complex->SaveContour(this);
        }
    }

    mFlags |= 2;
}

INCLUDE_ASM("asm/nonmatchings/prlib/contour", SaveContour__14SpmComplexNodeP13PrModelObject);

void PrModelObject::ResetContour() {
    mFlags &= ~2;
}

INCLUDE_ASM("asm/nonmatchings/prlib/contour", RenderContour__14SpmComplexNodeP13PrModelObject);
