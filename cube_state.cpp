#include "cube_state.h"

CubeState CubeState::solved() {
    CubeState s;
    for (int i = 0; i < 8;  i++) { s.cp[i] = i; s.co[i] = 0; }
    for (int i = 0; i < 12; i++) { s.ep[i] = i; s.eo[i] = 0; }
    return s;
}

bool CubeState::isSolved() const {
    for (int i = 0; i < 8;  i++) if (cp[i] != i || co[i] != 0) return false;
    for (int i = 0; i < 12; i++) if (ep[i] != i || eo[i] != 0) return false;
    return true;
}