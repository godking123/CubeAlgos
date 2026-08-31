#ifndef ROUX_H
#define ROUX_H

#include <vector>
#include "../../CubeState/CubeState.h"

namespace Roux {
    void buildTables();
    std::vector<Move> solve(const CubeState& s);
} // namespace Roux

#endif // ROUX_H
