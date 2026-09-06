#ifndef KOCIEMBA_H
#define KOCIEMBA_H

#include <vector>
#include "../../CubeState/CubeState.h"

namespace Kociemba {
    void buildTables();
    std::vector<Move> solve(const CubeState& s);
} // namespace Kociemba

#endif // KOCIEMBA_H
