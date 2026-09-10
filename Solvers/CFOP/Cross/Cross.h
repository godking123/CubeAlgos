#ifndef CROSS_H
#define CROSS_H

#include <vector>
#include "../../../CubeState/CubeState.h"
#include "../../../CubeState/SolverState.h"

struct CrossResult {
    int color;
    const char* rotation;
    std::vector<Move> moves;
};

namespace Cross {
    bool isSolved(const SolverState& s);
    std::vector<Move> solveCross(const SolverState& ss);
    CrossResult bestCross(const CubeState& scrambled);
}

#endif // CROSS_H_
