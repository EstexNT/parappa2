#include "model.h"

void PrModelObject::SaveContour() {
    SpmFileHeader *spm = m_spm_image;
    if (spm->unk70 == 0 || !m_rendered_once) {
        return;
    }

    u_int node_num = spm->m_node_num;
    for (int i = 0; i < node_num; i++) {
        SpmNode *node = spm->m_nodes[i];
        if (node->m_flags & 0x40) {
            SpmComplexNode *complex = reinterpret_cast<SpmComplexNode*>(node);
            complex->SaveContour(this);
        }
    }

    m_flags |= 2;
}

INCLUDE_ASM("asm/nonmatchings/prlib/contour", SaveContour__14SpmComplexNodeP13PrModelObject);

void PrModelObject::ResetContour() {
    m_flags &= ~2;
}

INCLUDE_ASM("asm/nonmatchings/prlib/contour", RenderContour__14SpmComplexNodeP13PrModelObject);
