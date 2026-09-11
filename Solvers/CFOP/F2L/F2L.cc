#include "F2L.h"
#include "../Cross/Cross.h"
#include "../PieceSearch.h"

namespace F2L {

int corner(int slot) { return 4 + slot; }
int edge(int slot)   { return 8 + slot; }

// Tracked Pieces for a Set of Slots, Cross Included
static uint16_t edgeMask(int slots) {
    uint16_t mask = Cross::EDGES;
    for (int i = 0; i < SLOTS; i++)
        if (slots & (1 << i)) mask |= 1 << edge(i);
    return mask;
}

static uint8_t cornerMask(int slots) {
    uint8_t mask = 0;
    for (int i = 0; i < SLOTS; i++)
        if (slots & (1 << i)) mask |= 1 << corner(i);
    return mask;
}

const int ALL_SLOTS = (1 << SLOTS) - 1;

bool isSolved(const CubeState& s) {
    return PieceSearch::isSolved(s, edgeMask(ALL_SLOTS), cornerMask(ALL_SLOTS));
}

std::vector<Move> solvePair(const CubeState& s, int slot, int placed) {
    int tracked = placed | (1 << slot);
    return PieceSearch::solve(s, edgeMask(tracked), cornerMask(tracked));
}

std::vector<F2LPair> solve(const CubeState& s) {
    std::vector<F2LPair> result;
    CubeState state = s;
    int placed = 0;

    while (placed != ALL_SLOTS) {
        F2LPair best;
        bool found = false;

        for (int slot = 0; slot < SLOTS; slot++) {
            if (placed & (1 << slot)) continue;
            std::vector<Move> moves = solvePair(state, slot, placed);
            if (!found || moves.size() < best.moves.size()) {
                best  = {slot, moves};
                found = true;
            }
        }

        for (Move m : best.moves) state = state.apply(m);
        placed |= 1 << best.slot;
        result.push_back(best);
    }
    return result;
}

} // namespace F2L
