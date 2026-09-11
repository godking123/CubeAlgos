#include "WCA.h"
#include "../CubeState/Scramble.h"
#include "../Solvers/KociembaNaive/Kociemba.h"
#include <random>

namespace WCA {

// Solvable in at Most `moves` Moves
static bool solvableWithin(const CubeState& s, int moves) {
    if (s.isSolved()) return true;
    if (moves == 0) return false;
    for (int m = 0; m < 18; m++)
        if (solvableWithin(s.apply(static_cast<Move>(m)), moves - 1)) return true;
    return false;
}

bool allowed(const CubeState& s) {
    return !solvableWithin(s, MIN_DISTANCE - 1);
}

// The inverse of the solution is a sequence from solved to the state, and the search
// already keeps it canonical bar a possible same-axis pair at the phase boundary
static std::vector<Move> scrambleTo(const CubeState& s) {
    std::vector<Move> solution = Kociemba::solve(s, MAX_LENGTH);
    std::vector<Move> moves;
    for (auto it = solution.rbegin(); it != solution.rend(); ++it)
        moves.push_back(inverseMove(*it));
    return canonicalize(moves);
}

// Rejecting states within a move of solved thins the draw by 19 in 43 quintillion, so
// the result stays uniform over every state that is allowed. A state the search cannot
// fit under the cap is redrawn too, which no state has been seen to need
static std::vector<Move> generate(std::mt19937_64& rng) {
    while (true) {
        CubeState s = randomState(rng());
        if (!allowed(s)) continue;
        std::vector<Move> moves = scrambleTo(s);
        if (!moves.empty()) return moves;
    }
}

// One Engine Per Thread
std::vector<Move> scramble() {
    static thread_local std::mt19937_64 rng(std::random_device{}());
    return generate(rng);
}

std::vector<Move> scramble(uint64_t seed) {
    std::mt19937_64 rng(seed);
    return generate(rng);
}

} // namespace WCA
