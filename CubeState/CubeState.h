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

// Whole-Cube Rotations, Three per Axis
// x follows R, y follows U, z follows F
enum class CubeRot : uint8_t {
    x, xp, x2,
    y, yp, y2,
    z, zp, z2
};

struct Orientation;

struct CubeState {
    uint8_t cp[8];   // Corner Permutation — Which Corner Sits in Each Slot
    uint8_t co[8];   // Corner Orientation — Twist, 0 to 2
    uint8_t ep[12];  // Edge Permutation
    uint8_t eo[12];  // Edge Orientation — Flip, 0 or 1

    static CubeState solved();
    bool isSolved() const;
    bool operator==(const CubeState& o) const;
    CubeState apply(Move m) const;

    // The same cube described from a new frame: pieces move to the slots they now
    // occupy and are renamed by their new homes, so a solved cube stays solved and
    // the D edges after rotate(x) are the cross of the colour now on the bottom
    CubeState rotate(CubeRot r) const;
    CubeState rotate(const Orientation& o) const;
};

Move inverseMove(Move m);

const char* moveName(Move m);
Move parseMove(const std::string& s);
std::vector<Move> parseSequence(const std::string& s);

#endif  // CUBE_STATE_H_
