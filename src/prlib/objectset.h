#ifndef PRLIB_OBJECTSET_H
#define PRLIB_OBJECTSET_H

#include "common.h"

#include "linkedlist.h"

template <typename T>
class PrObjectSet {
public:
    PrObjectSet() {
        mHead = NULL;
        mTail = NULL;
        mNum  = 0;
    }
    ~PrObjectSet();

    void Insert(T *obj) {
        if (mTail == NULL) {
            mHead = obj;
        } else {
            mTail->mList.mNext = obj;
            obj->mList.mPrev = mTail;
        }

        mTail = obj;
        mTail->mObjSet = this;
        mNum++;
    }

public:
    T *mHead;
    T *mTail;
    int mNum;
};

#endif /* PRLIB_OBJECTSET_H */
