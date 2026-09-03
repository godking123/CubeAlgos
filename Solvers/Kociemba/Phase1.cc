#include "Phase1.h"
#include "../../CubeState/CubeAlgos.h"
#include "Coords.h"
#include "CoordTables.h"
#include <algorithm>
#include <climits>

namespace Phase1 {

// IDA* Heuristic — Whichever Prune Table Demands More
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
    static const int opposite[6] = {3, 4, 5, 0, 1, 2};  // Opposite Face of Each Face

    for (int m = 0; m < 18; m++) {
        int face = m / 3;
        int lastFace = lastMove / 3;

        // Never Turn the Same Face Twice in a Row
        if (lastMove >= 0 && face == lastFace) continue;
        // Opposite Faces Commute, So Only One Order Is Worth Searching
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

// Walks every sequence of exactly `remaining` moves, reporting the ones ending in G1
// Unlike search() it never stops early and never takes a shorter solution, since
// Kociemba::solve wants one length at a time to pair against a phase 2 budget
// Returns false once the callback asks to stop, unwinding the whole walk
static bool searchExact(
    int twist, int flip, int slice,
    int remaining, int lastMove,
    std::vector<Move>& solution,
    const SolutionFn& onSolution
) {
    if (remaining == 0) {
        if (twist == 0 && flip == 0 && slice == 0) return onSolution(solution);
        return true;
    }
    // Too Far From G1 to Arrive in Time
    if (h(twist, flip, slice) > remaining) return true;

    static const int opposite[6] = {3, 4, 5, 0, 1, 2};  // Opposite Face of Each Face

    for (int m = 0; m < 18; m++) {
        int face = m / 3;
        int lastFace = lastMove / 3;

        // Never Turn the Same Face Twice in a Row
        if (lastMove >= 0 && face == lastFace) continue;
        // Opposite Faces Commute, So Only One Order Is Worth Searching
        if (lastMove >= 0 && opposite[face] == lastFace && face > lastFace) continue;

        solution.push_back(static_cast<Move>(m));
        bool keepGoing = searchExact(
            twistMove[twist][m], flipMove[flip][m], sliceMove[slice][m],
            remaining - 1, m, solution, onSolution
        );
        solution.pop_back();

        if (!keepGoing) return false;
    }

    return true;
}

bool forEachSolution(const CubeState& scrambled, int length, const SolutionFn& onSolution) {
    int twist = Coords::encodeTwist(scrambled);
    int flip  = Coords::encodeFlip(scrambled);
    int slice = Coords::encodeSlice(scrambled);

    std::vector<Move> solution;
    return searchExact(twist, flip, slice, length, -1, solution, onSolution);
}
} // namespace Phase1
