#include "Kociemba.h"
#include "CoordTables.h"
#include "Phase1.h"
#include "Phase2.h"

namespace Kociemba {

void buildTables() {
    buildCoordTables();
    buildPruningTables();
    buildPhase2Tables();
    buildPhase2PruningTables();
}

// Phase 1 reduces the cube to G1, phase 2 solves it there. Phase 2 assumes its
// input is already in G1, so it is given the state after phase 1 is applied.
std::vector<Move> solve(const CubeState& s) {
    std::vector<Move> solution = Phase1::solve(s);

    CubeState g1 = s;
    for (Move m : solution)
        g1 = g1.apply(m);

    for (Move m : Phase2::solve(g1))
        solution.push_back(m);

    return solution;
}
} // namespace Kociemba
