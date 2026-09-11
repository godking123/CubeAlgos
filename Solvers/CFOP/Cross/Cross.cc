#include "Cross.h"
#include "../PieceSearch.h"

namespace Cross {

bool isSolved(const CubeState& s) {
    return PieceSearch::isSolved(s, EDGES, 0);
}

std::vector<Move> solveCross(const CubeState& s) {
    return PieceSearch::solve(s, EDGES, 0);
}

// Solve every colour in its own frame, keep the shortest
// Ties go to the lowest colour index, so white beats yellow beats the sides
CrossResult bestCross(const CubeState& scrambled) {
    CrossResult best;
    bool found = false;

    for (int c = 0; c < 6; c++) {
        Orientation hold = orientationWithBottom(c);
        std::vector<Move> moves = solveCross(scrambled.rotate(hold));
        if (!found || moves.size() < best.moves.size()) {
            best.color = c;
            best.hold  = hold;
            best.moves = moves;
            found = true;
        }
    }
    return best;
}

} // namespace Cross
