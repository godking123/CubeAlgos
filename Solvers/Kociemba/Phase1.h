#ifndef PHASE1_H
#define PHASE1_H

#include <functional>
#include <vector>
#include "../../CubeState/CubeState.h"

namespace Phase1 {
    // Called with each phase 1 solution found. Returning false stops the search.
    using SolutionFn = std::function<bool(const std::vector<Move>&)>;

    // The shortest way into G1, via IDA*.
    std::vector<Move> solve(const CubeState& s);

    // Every way into G1 that takes exactly `length` moves, reported one at a time.
    // The shortest phase 1 solution is not usually the one that leads to the
    // shortest overall solve, so Kociemba::solve walks these instead of taking the
    // first solution it is handed. Returns false if the callback stopped the walk,
    // true if it ran to the end.
    bool forEachSolution(const CubeState& s, int length, const SolutionFn& onSolution);
} // namespace Phase1

#endif // PHASE1_H_
