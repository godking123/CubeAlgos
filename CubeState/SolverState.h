#ifndef SOLVER_STATE_H
#define SOLVER_STATE_H

#include "CubeState.h"

// Whole-Cube Rotations, Three per Axis
enum class CubeRot : uint8_t {
    x, xp, x2,
    y, yp, y2,
    z, zp, z2
};

// Colour on Each Face, in U R F D L B Order
// Identity is the solved view: 0=white 1=red 2=green 3=yellow 4=orange 5=blue
struct Orientation {
    uint8_t faces[6] = {0, 1, 2, 3, 4, 5};
};

struct SolverState {
    CubeState   cube;
    Orientation view;
};

// Orientation Helpers
Orientation rotate(const Orientation& o, CubeRot r);
Move translateMove(Move m, const Orientation& o);

// Solver State Transitions
SolverState applyMove(const SolverState& s, Move m);
SolverState applyRotation(const SolverState& s, CubeRot r);

// Cross Colour Setup
// color: 0=white 1=red 2=green 3=yellow 4=orange 5=blue
SolverState faceToBottom(const SolverState& s, int color);
const char* faceToBottomName(int color);

#endif // SOLVER_STATE_H
