#ifndef CFOP_H
#define CFOP_H

#include <vector>
#include "../../CubeState/CubeState.h"

namespace CFOP {
    void buildTables();
    std::vector<Move> solve(const CubeState& s);
} // namespace CFOP

#endif // CFOP_H
