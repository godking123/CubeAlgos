#ifndef WCA_H
#define WCA_H

#include <cstdint>
#include <vector>
#include "../CubeState/CubeState.h"

// Random state scrambles for 3x3x3, the way TNoodle generates them for competition
//
// A state is drawn uniformly from every legal cube, redrawn while it sits within one
// move of solved (WCA regulation 4b3), solved with Kociemba under the 21 move cap, and
// the solution is inverted. Applied to a solved cube, the scramble reaches that state
//
// The scramble is canonical and never longer than MAX_LENGTH. Kociemba's tables must
// be built before the first call
namespace WCA {
    const int MAX_LENGTH   = 21;  // TNoodle's Cap for 3x3x3
    const int MIN_DISTANCE = 2;   // Regulation 4b3: At Least Two Moves From Solved

    std::vector<Move> scramble();               // Seeded Once From random_device
    std::vector<Move> scramble(uint64_t seed);  // Same Scramble for the Same Seed

    // True when regulation 4b3 lets the state stand as a scramble
    bool allowed(const CubeState& s);
} // namespace WCA

#endif // WCA_H
