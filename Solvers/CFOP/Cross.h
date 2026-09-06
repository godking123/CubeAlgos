#ifndef CROSS_H
#define CROSS_H

#include <vector>
#include "../../CubeState/CubeState.h"

namespace Cross {
    bool isSolved(const CubeState& s);
    std::vector<Move> solve(const CubeState& s);
}

#endif // CROSS_H_