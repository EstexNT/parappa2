#ifndef PRLIB_CAMERA_H
#define PRLIB_CAMERA_H

#include "prpriv.h"
#include "spadata.h"

#include <eetypes.h>

#include <nalib/navector.h>

class SpcFileHeader {
public:
    void Initialize();
    PrPERSPECTIVE_CAMERA* GetCamera(float time) const;

    void ChangePointer();

    template <typename T>
    T* CalculatePointer(T *offset) {
        if (!offset) {
            return NULL;
        }
        return reinterpret_cast<T*>(reinterpret_cast<int>(this) + reinterpret_cast<int>(offset));
    }

public:
    u_int mMagic;
    u_short mVersion;
    u_short mFlags;

    PR_PADDING(unk8, 0x10);
    char mName[32];
    PR_PADDING(unk38, 0x3c);
    int *unk74;
    PR_PADDING(unk78, 0x10);
    SpaTrack<NaVECTOR<float, 4> > *unk88;
    SpaTrack<NaVECTOR<float, 4> > *unk8C;
    SpaTrack<float> *unk90;
    SpaTrack<float> *unk94;

    u_int mDepthLevel;

    SpaTrack<float> *mFocalLenTrack;
    SpaTrack<float> *mDefocusLenTrack;
};

#endif /* PRLIB_CAMERA_H */
