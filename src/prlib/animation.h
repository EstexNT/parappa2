#ifndef PRLIB_ANIMATION_H
#define PRLIB_ANIMATION_H

#include "common.h"

class SpmNode;
class SpaNodeAnimation;

class SpaFileHeader {
public:
    PR_PADDING(unk0, 0x14);
    float unk14;
    PR_PADDING(unk18, 0x38);
    SpaNodeAnimation **unk50;

public:
    void Initialize();

    bool IsNodeVisible(SpmNode *arg0, float arg1) const;

    void ChangePointer();
};

#endif /* PRLIB_ANIMATION_H */
