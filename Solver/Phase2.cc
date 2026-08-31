#include "Phase2.h"
#include "../CubeState/CubeAlgos.h"
#include <algorithm>
#include <climits>

namespace Phase2 {

// Heuristic for A*
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
    static const int opposite[6] = {3, 4, 5, 0, 1, 2};
    // To prevent undoing a previous move

    for (int i = 0; i < 10; i++) {
        int m = PHASE2_MOVES[i];
        int face = m / 3;
        int lastFace = lastMove / 3;

        if (lastMove >= 0 && face == lastFace) continue;
        if (lastMove >= 0 && opposite[face] == lastFace && face > lastFace) continue;

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

std::vector<Move> solve(const CubeState& g1) {
    int cp        = Coords::encodeCP(g1);
    int ep        = Coords::encodeEP(g1);
    int slicePerm = Coords::encodeSlicePerm(g1);

    std::vector<Move> solution;
    int limit = h(cp, ep, slicePerm);

    while (true) {
        int result = search(cp, ep, slicePerm, 0, limit, -1, solution);
        if (result == -1) return solution;
        if (result == INT_MAX) return {};
        limit = result;
    }
}
} // namespace Phase2
