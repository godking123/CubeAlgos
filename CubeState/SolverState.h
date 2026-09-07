#ifndef SOLVER_STATE_H
#define SOLVER_STATE_H

#include "CubeState.h"

enum class rotation : uint32_t {
    x, xp, x2,
    y, yp, y2,
    z, zp, z2
};

struct Orientation {
    int x;
    int y;
    int z;
};

struct SolverState {
    CubeState cube;
    Orientation view;
};

#endif // SOLVER_STATE_H_