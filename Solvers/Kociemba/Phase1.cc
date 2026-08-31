#include "Phase1.h"
#include "../../CubeState/CubeAlgos.h"
#include "Coords.h"
#include "CoordTables.h"
#include <algorithm>
#include <climits>

namespace Phase1 {

// Heuristic for A*
static int h(int twist, int flip, int slice) {
    return std::max(
        pruneFlipSlice[flip][slice],
        pruneTwistSlice[twist][slice]
    );
}

static int search(
    int twist, int flip, int slice,
    int depth, int limit,
    int lastMove,
    std::vector<Move>& solution
) {
    int f = depth + h(twist, flip, slice);
 
    if (f > limit) return f;
    if (twist == 0 && flip == 0 && slice == 0) return -1;

    int minimum = INT_MAX;
    static const int opposite[6] = {3, 4, 5, 0, 1, 2};
    // To prevent undoing a previous move

    for (int m = 0; m < 18; m++) {
        int face = m / 3;
        int lastFace = lastMove / 3;

        if (lastMove >= 0 && face == lastFace) continue;
        if (lastMove >= 0 && opposite[face] == lastFace && face > lastFace) continue;

        int newTwist = twistMove[twist][m];
        int newFlip = flipMove[flip][m];
        int newSlice = sliceMove[slice][m];

        solution.push_back(static_cast<Move>(m));
        int result = search(newTwist, newFlip, newSlice, depth + 1, limit, m, solution);

        if (result == -1) return -1;
        solution.pop_back();
        minimum = std::min(minimum, result);
    }

    return minimum;
}

std::vector<Move> solve(const CubeState& scrambled) {
    int twist = Coords::encodeTwist(scrambled);
    int flip  = Coords::encodeFlip(scrambled);
    int slice = Coords::encodeSlice(scrambled);

    std::vector<Move> solution;
    int limit = h(twist, flip, slice);

    while (true) {
        int result = search(twist, flip, slice, 0, limit, -1, solution);
        if (result == -1) return solution;
        if (result == INT_MAX) return {};
        limit = result;
    }
}
} // namespace Phase1