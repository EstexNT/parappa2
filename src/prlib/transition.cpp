#include "model.h"

void PrModelObject::SavePosture() {
    if (!(m_spm_image->m_flags & 0x40)) {
        return;
    }

    if (m_rendered_once) {
        m_active_transition = 1 - m_active_transition;
        m_flags |= 4;
    }
}

void PrModelObject::ResetPosture() {
    m_flags &= ~4;
}

INCLUDE_ASM("asm/nonmatchings/prlib/transition", BlendTransitionMatrix__7SpmNodeP13PrModelObjectRt8NaMATRIX3Zfi4i4);

INCLUDE_ASM("asm/nonmatchings/prlib/transition", BlendTransactionWeight__12SpmShapeNodeP13PrModelObjectfUi);
