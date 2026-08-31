#include "Roux.h"

namespace Roux {

// Placeholder. Roux solves in stages (first block, second block, CMLL, LSE), each of
// which needs its own recogniser and algorithm set; none of that is written yet.
// Main checks Method::implemented and reports the method as unavailable rather
// than calling solve, so returning nothing here is never mistaken for a solve.

void buildTables() {
}

std::vector<Move> solve(const CubeState& s) {
    (void)s;
    return {};
}
} // namespace Roux
