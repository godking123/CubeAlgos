#ifndef CUBE_STATE_H_
#define CUBE_STATE_H_

#include <cstdint>
#include <string>
#include <vector>

enum class Move : uint8_t {
  U, U2, Up,
  R, R2, Rp,
  D, D2, Dp,
  B, B2, Bp,
  F, F2, Fp,
  L, L2, Lp,
  Count
};

struct CubeState {
    uint8_t cp[8];
    uint8_t co[8];
    uint8_t ep[12];
    uint8_t eo[12];

    static CubeState solved();
    bool isSolved() const;
    CubeState apply(Move m) const;
    std::string toString() const;
};

const char* moveName(Move m);
Move parseMove(const std::string& s);

#endif  // CUBE_STATE_H_