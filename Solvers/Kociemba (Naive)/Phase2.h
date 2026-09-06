#ifndef PHASE2_H
#define PHASE2_H

#include <vector>
#include "../../CubeState/CubeState.h"

namespace Phase2 {
    // Solves a state already in G1, using G1 generators only
    //
    // maxMoves caps the search: Kociemba::solve only wants a solution that beats the
    // one it holds, so anything longer is wasted work. Returns empty if none is that short
    //
    // lastMove is the phase 1 solution's last move, or -1 for none. Phase 2 never starts
    // on that face, since the pair would collapse into one move and the collapsed
    // sequence is itself a phase 1 solution of the same length
    std::vector<Move> solve(const CubeState& s, int maxMoves = 18, int lastMove = -1);
} // namespace Phase2

#endif // PHASE2_H
