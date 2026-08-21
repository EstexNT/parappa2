#ifndef PRLIB_ANIMATION_H
#define PRLIB_ANIMATION_H

#include "common.h"

#include "objectset.h"

#include "spadata.h"

#define SPA_MAGIC (0x59238771)
#define SPA_VERSION (2)

class SpmNode;

class SpaFileHeader {
public:
    u_int m_magic;
    u_short m_version;
    u_short m_flags;

    PR_PADDING(unk8, 0xc);
    float unk14;
    char m_name[32];
    int *unk38;
    PrLinkedList<SpaFileHeader> m_list;
    PrObjectSet<SpaFileHeader> *m_obj_set;
    void *m_user_data;
    int unk4C;
    SpaNodeAnimation **unk50;

public:
    void Initialize();

    bool IsNodeVisible(SpmNode *arg0, float arg1) const;

    void ChangePointer();
};

#endif /* PRLIB_ANIMATION_H */
