#ifndef ROTATION_H
#define ROTATION_H

#include "CubeState.h"

// Whole-cube rotations — only change permutation (and orientation for x/z)
// y: spin top clockwise (U stays U, F→R→B→L→F)
// x: tip forward        (R stays R, U→F→D→B→U)
// z: tilt right         (F stays F, U→L→D→R→U)

namespace Rotation {
    CubeState y (const CubeState& s);
    CubeState yp(const CubeState& s);   // y'
    CubeState y2(const CubeState& s);

    CubeState x (const CubeState& s);
    CubeState xp(const CubeState& s);   // x'
    CubeState x2(const CubeState& s);

    CubeState z (const CubeState& s);
    CubeState zp(const CubeState& s);   // z'
    CubeState z2(const CubeState& s);

    // Get the rotation needed to put a given face on bottom
    // color: 0=white 1=red 2=green 3=yellow 4=orange 5=blue
    CubeState faceToBottom(const CubeState& s, int color);

    // Name of the rotation applied by faceToBottom
    const char* faceToBottomName(int color);
}

#endif // ROTATION_H_