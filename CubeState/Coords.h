#ifndef COORDS_H
#define COORDS_H

#include "CubeState.h"

namespace Coords {
    // Phase 1
    int encodeFlip(const CubeState& s);
    int encodeTwist(const CubeState& s);
    int encodeSlice(const CubeState& s);
    void decodeFlip(CubeState& s, int flip);
    void decodeTwist(CubeState& s, int twist);
    void decodeSlice(CubeState& s, int slice);

    // Phase 2
    int encodeSlicePerm(const CubeState& s);
    int encodeCP(const CubeState& s);
    int encodeEP(const CubeState& s);
    void decodeSlicePerm(CubeState& s, int perm);
    void decodeCP(CubeState& s, int cp);
    void decodeEP(CubeState& s, int ep);
}

#endif // COORDS_H
