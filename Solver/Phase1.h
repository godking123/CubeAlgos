#ifndef PHASE1_H
#define PHASE1_H

#include <vector>
#include "../CubeState/CubeState.h"

namespace Phase1 {
    std::vector<Move> solve(const CubeState& s);
} // namespace Phase1

#endif // PHASE1_H_