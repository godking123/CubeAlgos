#ifndef CROSS_H
#define CROSS_H

#include <vector>
#include "../../../CubeState/CubeState.h"
#include "../../../CubeState/Rotation.h"

// The shortest cross over all six colours
// hold is how to turn the cube before executing moves, moves are in that frame
struct CrossResult {
    int color;
    Orientation hold;
    std::vector<Move> moves;
};

namespace Cross {
    // The cross is always the D edges, rotate the cube to pick a colour
    bool isSolved(const CubeState& s);
    std::vector<Move> solveCross(const CubeState& s);
    CrossResult bestCross(const CubeState& scrambled);
}

#endif // CROSS_H
