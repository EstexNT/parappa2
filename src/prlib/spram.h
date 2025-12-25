#ifndef PRLIB_SPRAM_H
#define PRLIB_SPRAM_H

#include "common.h"

#include "vu1/vumem.h"

#include <nalib/navector.h>
#include <nalib/namatrix.h>

#include <libdma.h>

class PrSPRAM_DATA {
public:
    void SendDisplayHeader();

public:
    char unk0[0x50];
    NaVECTOR<float, 4> unk50;
    char unk60[0x40];
    NaMATRIX<float, 4, 4> m_camera_matrix;
    NaMATRIX<float, 4, 4> m_view_projection_matrix;
    NaMATRIX<float, 4, 4> unk120;
    NaMATRIX<float, 4, 4> unk160;
    char unk1A0[0x100];
    sceDmaTag m_end_dmatag;
    char unk2B0[0x100];
    PrDisplayHeader m_display_header;
    char unk660[0x28];
    u_int m_disturbance_param;
};

#endif /* PRLIB_SPRAM_H */
