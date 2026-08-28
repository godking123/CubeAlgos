#ifndef COORDS_H
#define COORDS_H

#include "CubeState.h"

namespace Coords {
    int encodeFlip(const CubeState& s);
    int encodeTwist(const CubeState& s);
    int encodeSlice(const CubeState& s);
    void decodeFlip(CubeState& s, int flip);
    void decodeTwist(CubeState& s, int twist);
    void decodeSlice(CubeState& s, int slice);
}

#endif // COORDS_H
