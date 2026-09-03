#ifndef CUBE_STATE_H_
#define CUBE_STATE_H_

#include <cstdint>
#include <string>
#include <vector>

// Three Turns per Face, in U R F D L B Order
// The layout is load-bearing: face = move / 3, and opposite faces sit 3 apart
enum class Move : uint8_t {
    U, U2, Up,
    R, R2, Rp,
    F, F2, Fp,
    D, D2, Dp,
    L, L2, Lp,
    B, B2, Bp,
    Count
};

struct CubeState {
    uint8_t cp[8];   // Corner Permutation — Which Corner Sits in Each Slot
    uint8_t co[8];   // Corner Orientation — Twist, 0 to 2
    uint8_t ep[12];  // Edge Permutation
    uint8_t eo[12];  // Edge Orientation — Flip, 0 or 1

    static CubeState solved();
    bool isSolved() const;
    CubeState apply(Move m) const;
};

const char* moveName(Move m);
Move parseMove(const std::string& s);
std::vector<Move> parseSequence(const std::string& s);

#endif  // CUBE_STATE_H_
