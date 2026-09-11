#ifndef KOCIEMBA_H
#define KOCIEMBA_H

#include <vector>
#include "../../CubeState/CubeState.h"

namespace Kociemba {
    void buildTables();

    // Shortest solution the combination search finds, empty if s is already solved
    std::vector<Move> solve(const CubeState& s);

    // The same search, but nothing longer than maxMoves ever counts: the walk runs on
    // past its usual cutoff until a combination fits. Empty if s is already solved, or
    // if no phase 1 length up to 12 leads to one, which a cap of 21 never hits
    std::vector<Move> solve(const CubeState& s, int maxMoves);
} // namespace Kociemba

#endif // KOCIEMBA_H
