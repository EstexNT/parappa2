#include "scene.h"

#include "camera.h"
#include "model.h"

#include <float.h>

INCLUDE_ASM("asm/nonmatchings/prlib/scene", __13PrSceneObjectP13sceGsDrawEnv1PCcUi);

INCLUDE_ASM("asm/nonmatchings/prlib/scene", _$_13PrSceneObject);

void PrSceneObject::SelectCamera(SpcFileHeader *camera) {
    mCamera = camera;
    mCameraTime = 0.0f;
}

PrPERSPECTIVE_CAMERA* PrSceneObject::GetCurrentCamera() {    
    if (mCamera != NULL) {
        return mCamera->GetCamera(mCameraTime);  
    } else {
        return &mDefaultCamera;
    }
}

INCLUDE_ASM("asm/nonmatchings/prlib/scene", SetAppropriateDefaultCamera__13PrSceneObject);

float PrSceneObject::GetFocalLength() const {
    SpcFileHeader *camera = mCamera;
    if (camera == NULL || !(camera->mFlags & 0x8) ) {
        return mDefaultFocalLen;
    } else {
        float focalLength = *camera->mFocalLenTrack->GetValue(mCameraTime);
        if (focalLength <= 0.0f) {
            focalLength = FLT_EPSILON;
        }
        return focalLength;
    }
}

float PrSceneObject::GetDefocusLength() const {
    SpcFileHeader *camera = mCamera;
    if (camera == NULL || !(camera->mFlags & 0x8)) {
        return mDefaultDefocusLen;
    } else {
        return *camera->mDefocusLenTrack->GetValue(mCameraTime);
    }
}

u_int PrSceneObject::GetDepthLevel() const {
    SpcFileHeader *camera = mCamera;
    if (camera == NULL || !(camera->mFlags & 0x8)) {
        return mDefaultDepthLevel;
    } else {
        return camera->mDepthLevel;
    }
}

void PrSceneObject::PreprocessModel() {
    PrModelObject *sp = NULL;
    PrModelObject *model = mModelSet.mHead;

    PrModelObject *modelList = NULL;
    PrModelObject *screenList = NULL;
    PrModelObject *t1 = NULL;

    while (model != NULL) {
        SpmFileHeader *spm = model->mSpmImage;
        PrModelObject *next = model->mList.mNext;
        if (spm->mFlags & eSpmIsScreenModel) {
            model->mList.mNext = screenList;
            screenList = model;
        } else if (spm->mFlags & 0x200) {
            PrModelObject *a1 = sp;
            PrModelObject **a3 = &sp;
            u_int t0_1 = spm->unk78;
            while (a1 != NULL && a1->mSpmImage->unk78 < t0_1) {
                a3 = (PrModelObject**)a1;
                a1 = *a3;
            }
            model->mList.mNext = a1;
            *a3 = model;
        } else if (spm->mFlags & 0x400) {
            model->mList.mNext = t1;
            t1 = model;
        } else {
            model->mList.mNext = modelList;
            modelList = model;
        }
        model = next;
    }

    PrModelObject *head = NULL;
    PrModelObject *tail = NULL;

    mScreenModelList = screenList;
    if (modelList != NULL) {
        this->unk9C = modelList;
    } else {
        this->unk9C = screenList;
    }

    if (t1 != NULL) {
        this->unk98 = t1;
    } else {
        this->unk98 = this->unk9C;
    }

    PrModelObject *v1 = sp;
    if (v1 != NULL) {
        head = v1;
        while (sp != NULL) {
            PrModelObject *v0;
            sp = v1->mList.mNext;
            v1->mList.mPrev = tail;
            tail = v1;
            v0 = sp;
            v1 = v0;
        }
    }

    if (t1 != NULL) {
        if (head == NULL) {
            head = t1;
        } else {
            tail->mList.mNext = t1;
        }

        do {
            PrModelObject *model = t1;
            t1 = t1->mList.mNext;
            model->mList.mPrev = tail;
            tail = model;
        } while (t1 != NULL);
    }

    if (modelList != NULL) {
        if (head == NULL) {
            head = modelList;
        } else {
            tail->mList.mNext = modelList;
        }

        while (modelList != NULL) {
            PrModelObject *model = modelList;
            modelList = modelList->mList.mNext;
            model->mList.mPrev = tail;
            tail = model;
        }
    }

    if (screenList != NULL) {
        if (head == NULL) {
            head = screenList;
        } else {
            tail->mList.mNext = screenList;
        }

        do {
            PrModelObject *model = screenList;
            screenList = screenList->mList.mNext;
            model->mList.mPrev = tail;
            tail = model;
        } while (screenList != NULL);
    }

    mModelSet.mHead = head;
    mModelSet.mTail = tail;
}

/* nalib/navector.h */
INCLUDE_ASM("asm/nonmatchings/prlib/scene", func_0014B988);

INCLUDE_ASM("asm/nonmatchings/prlib/scene", func_0014B9B0);

/* prlib/objectset.h */
INCLUDE_ASM("asm/nonmatchings/prlib/scene", _$_t11PrObjectSet1Z13PrModelObject);
