#ifndef PHASE1_H
#define PHASE1_H

#include <functional>
#include <vector>
#include "../../CubeState/CubeState.h"

namespace Phase1 {
    // Receives each solution found, returns false to stop the search
    using SolutionFn = std::function<bool(const std::vector<Move>&)>;

    // Shortest way into G1, via IDA*
    std::vector<Move> solve(const CubeState& s);

    // Every way into G1 in exactly `length` moves, one at a time
    // Returns false if the callback stopped the walk, true if it ran to the end
    bool forEachSolution(const CubeState& s, int length, const SolutionFn& onSolution);
} // namespace Phase1

#endif // PHASE1_H
