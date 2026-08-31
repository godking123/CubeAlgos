#ifndef PHASE2_H
#define PHASE2_H

#include <vector>
#include "../../CubeState/CubeState.h"

namespace Phase2 {
    // Solves a state already in G1 using G1 generators only.
    //
    // maxMoves caps the search: Kociemba::solve already holds a complete solution
    // and only wants to hear about one that beats it, so a search that cannot
    // finish inside the remaining budget should give up rather than find a longer
    // answer nobody will use. Returns empty when there is no solution that short.
    //
    // lastMove is the final move of the phase 1 solution this state came from, or
    // -1 when there is none. Phase 2 will not start on that same face, since such
    // a pair collapses into one move and the collapsed sequence is itself a phase 1
    // solution of the same length, which this length or an earlier one already
    // covers -- G1 is closed under every phase 2 generator.
    std::vector<Move> solve(const CubeState& s, int maxMoves = 18, int lastMove = -1);
} // namespace Phase2

#endif // PHASE2_H
