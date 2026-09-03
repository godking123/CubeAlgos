#include "Phase2.h"
#include "../../CubeState/CubeAlgos.h"
#include "Coords.h"
#include "CoordTables.h"
#include <algorithm>
#include <climits>

namespace Phase2 {

// IDA* Heuristic — Whichever Prune Table Demands More
static int h(int cp, int ep, int slicePerm) {
    return std::max(
        pruneCPSlicePerm[cp][slicePerm],
        pruneEPSlicePerm[ep][slicePerm]
    );
}

static int search(
    int cp, int ep, int slicePerm,
    int depth, int limit,
    int lastMove,
    std::vector<Move>& solution
) {
    int f = depth + h(cp, ep, slicePerm);

    if (f > limit) return f;
    if (cp == 0 && ep == 0 && slicePerm == 0) return -1;

    int minimum = INT_MAX;
    static const int opposite[6] = {3, 4, 5, 0, 1, 2};  // Opposite Face of Each Face

    for (int i = 0; i < 10; i++) {
        int m = PHASE2_MOVES[i];
        int face = m / 3;
        int lastFace = lastMove / 3;

        // Never Turn the Same Face Twice in a Row
        if (lastMove >= 0 && face == lastFace) continue;

        // Opposite Faces Commute, So Only One Order Is Worth Searching
        // At depth 0 lastMove is phase 1's, and the other order is unreachable
        if (depth > 0 && lastMove >= 0 && opposite[face] == lastFace && face > lastFace) continue;

        int newCP = cpMove[cp][m];
        int newEP = epMove[ep][m];
        int newSlicePerm = slicePermMove[slicePerm][m];

        solution.push_back(static_cast<Move>(m));
        int result = search(newCP, newEP, newSlicePerm, depth + 1, limit, m, solution);

        if (result == -1) return -1;
        solution.pop_back();
        minimum = std::min(minimum, result);
    }

    return minimum;
}

std::vector<Move> solve(const CubeState& g1, int maxMoves, int lastMove) {
    int cp        = Coords::encodeCP(g1);
    int ep        = Coords::encodeEP(g1);
    int slicePerm = Coords::encodeSlicePerm(g1);

    std::vector<Move> solution;
    int limit = h(cp, ep, slicePerm);

    while (limit <= maxMoves) {
        int result = search(cp, ep, slicePerm, 0, limit, lastMove, solution);
        if (result == -1) return solution;
        if (result == INT_MAX) return {};
        limit = result;
    }

    return {};  // Nothing Within the Budget
}
} // namespace Phase2
