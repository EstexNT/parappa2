#ifndef PRLIB_VUMEM_H
#define PRLIB_VUMEM_H

//
// ================ VU1 memory layout ================
// NOTE: Incomplete, not definitive, might have mistakes.
//
// Menderer texture creation (vump_menderer_create_texture):
// - [ 0x0000 ... 0x04B0 ] Menderer texture packet
//
// Render microprograms (normal/bothface/contour/refmap/screen/antiline):
// - [ 0x0000 ... 0x0280 ] Display header
// - [ 0x0280 ... 0x0400 ] Packet header
// - [ 0x0400 ... 0x0C00 ] Unused*
// - [ 0x0C00 ... 0x1920 ] Packet chunk (1)
// - [ 0x1920 ... 0x2600 ] Packet chunk (1) (GS area)
// - [ 0x2600 ... 0x3320 ] Packet chunk (2)
// - [ 0x3320 ... 0x4000 ] Packet chunk (2) (GS area)
//
// *Has leftover data, so must be used somewhere.
//
// The VU1 renderer is quad-buffered, this way the VIF, VU and GIF
// are all kept busy, so both data uploads and rendering take place
// simultaneously in parallel.
//
// This data flow design isn't perfect though, as this eats up VU memory,
// meaning each chunk's size is limited to ~3.28 KB (3360 bytes / 210 QW),
// including the chunk header itself (GS packet header, vertex number, etc).
//

/* Divides address by 8, for use with MSCAL */
#define VU_ADDR(x, base) (((int)x - (int)base) >> 3)

#endif /* PRLIB_VUMEM_H */
