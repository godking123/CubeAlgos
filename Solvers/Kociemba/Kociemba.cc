#include "Kociemba.h"
#include "CoordTables.h"
#include "Phase1.h"
#include "Phase2.h"
#include <algorithm>

namespace Kociemba {

void buildTables() {
    buildCoordTables();
    buildPruningTables();
    buildPhase2Tables();
    buildPhase2PruningTables();
}

namespace {

const int MAX_PHASE1_LENGTH = 12;  // no state is further than 12 moves from G1
const int MAX_PHASE2_LENGTH = 18;  // nor further than 18 from solved once inside it
const int GOOD_ENOUGH       = 20;  // God's number: nothing can beat this, stop dead
const int EXTRA_LENGTHS     = 2;   // phase 1 lengths to try past the first that works

// Worse than the worst case the two phases can produce, so the first complete
// solution always improves on it.
const int NO_SOLUTION = MAX_PHASE1_LENGTH + MAX_PHASE2_LENGTH + 1;

} // namespace

// Phase 1 reduces the cube to G1, phase 2 solves it from there. Phase 2 assumes
// its input is already in G1, so it is given the state after phase 1 is applied.
//
// The shortest phase 1 solution is rarely the one that gives the shortest solve.
// G1 is a huge set, and where phase 1 lands inside it decides how much work phase 2
// is left with: a phase 1 one move longer often enters G1 at a point phase 2 can
// finish several moves sooner. So rather than take the first phase 1 solution and
// commit to it, walk the phase 1 solutions by length, run phase 2 on each, and keep
// the best combination. Each phase 2 run is capped at one move less than the best
// total so far, so it either beats the incumbent or gives up quickly.
std::vector<Move> solve(const CubeState& s) {
    std::vector<Move> best;
    int bestLength = NO_SOLUTION;

    // The first phase 1 length that completes; searching far past it costs a lot of
    // time for a move or two, since each extra length multiplies the candidates.
    int firstSuccess = -1;

    for (int p1Length = 0; p1Length <= MAX_PHASE1_LENGTH; p1Length++) {
        // Phase 1 alone is already as long as the whole incumbent solution.
        if (p1Length >= bestLength) break;
        if (firstSuccess >= 0 && p1Length > firstSuccess + EXTRA_LENGTHS) break;

        Phase1::forEachSolution(s, p1Length, [&](const std::vector<Move>& phase1) {
            CubeState g1 = s;
            for (Move m : phase1) g1 = g1.apply(m);

            // Only a solution shorter than the incumbent is of any use.
            int budget = std::min(MAX_PHASE2_LENGTH, bestLength - p1Length - 1);
            int lastMove = phase1.empty() ? -1 : static_cast<int>(phase1.back());

            std::vector<Move> phase2 = Phase2::solve(g1, budget, lastMove);
            // An empty phase 2 means either nothing to do or nothing short enough.
            if (phase2.empty() && !g1.isSolved()) return true;

            int total = p1Length + static_cast<int>(phase2.size());
            if (total < bestLength) {
                bestLength = total;
                best = phase1;
                best.insert(best.end(), phase2.begin(), phase2.end());
            }

            return bestLength > GOOD_ENOUGH;  // stop the walk once nothing can beat it
        });

        if (bestLength < NO_SOLUTION && firstSuccess < 0) firstSuccess = p1Length;
        if (bestLength <= GOOD_ENOUGH) break;
    }

    return best;
}
} // namespace Kociemba
