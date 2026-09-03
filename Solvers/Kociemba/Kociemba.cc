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

const int MAX_PHASE1_LENGTH = 12;  // Scrambled State to G1 <= 12 Moves
const int MAX_PHASE2_LENGTH = 18;  // G1 to Solved <= 18 Moves
const int GOOD_ENOUGH       = 20;  // God's Number
const int EXTRA_LENGTHS     = 2;   // Phase 1 Length Margin

// Sentinel Worse Than Any Real Solve
const int NO_SOLUTION = MAX_PHASE1_LENGTH + MAX_PHASE2_LENGTH + 1;

} // namespace

// Phase 1 reduces the cube to G1, phase 2 solves it from there
//
// Where phase 1 lands inside G1 sets phase 2's workload, so the shortest phase 1 is
// rarely the shortest solve. Walk the phase 1 solutions by length instead, cap each
// phase 2 at one move under the best total so far, and keep the best combination
//
// Each further phase 1 length costs roughly 20x more candidates, so EXTRA_LENGTHS
// stops the walk a couple of lengths past the first that completes
std::vector<Move> solve(const CubeState& s) {
    std::vector<Move> best;
    int bestLength = NO_SOLUTION;

    int firstSuccess = -1;  // First Phase 1 Length That Completes

    for (int p1Length = 0; p1Length <= MAX_PHASE1_LENGTH; p1Length++) {
        if (p1Length >= bestLength) break;  // Phase 1 Alone Ties the Best
        if (firstSuccess >= 0 && p1Length > firstSuccess + EXTRA_LENGTHS) break;

        Phase1::forEachSolution(s, p1Length, [&](const std::vector<Move>& phase1) {
            CubeState g1 = s;
            for (Move m : phase1) g1 = g1.apply(m);

            // Phase 2 Budget for a Strictly Shorter Total
            int budget = std::min(MAX_PHASE2_LENGTH, bestLength - p1Length - 1);
            int lastMove = phase1.empty() ? -1 : static_cast<int>(phase1.back());

            std::vector<Move> phase2 = Phase2::solve(g1, budget, lastMove);
            // Already Solved, or No Solution Within Budget
            if (phase2.empty() && !g1.isSolved()) return true;

            int total = p1Length + static_cast<int>(phase2.size());
            if (total < bestLength) {
                bestLength = total;
                best = phase1;
                best.insert(best.end(), phase2.begin(), phase2.end());
            }

            return bestLength > GOOD_ENOUGH;  // Stop at God's Number
        });

        if (bestLength < NO_SOLUTION && firstSuccess < 0) firstSuccess = p1Length;
        if (bestLength <= GOOD_ENOUGH) break;
    }

    return best;
}
} // namespace Kociemba
