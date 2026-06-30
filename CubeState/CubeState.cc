#include "CubeState.h"
#include "MoveTable.h"

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

CubeState CubeState::apply(Move m) const {
    CubeState next;
    const MoveTable& table = MOVE_TABLES[static_cast<int>(m)];

    for (int i = 0; i < 8; i++) {
        // Transform CP and CO Based off of Move
        next.cp[i] = cp[table.cp[i]];
        next.co[i] = co[table.co[i]];
    }

    for (int i = 0; i < 12; i++) {
        // Transform EP and EO Based off of Move
        next.ep[i] = ep[table.ep[i]];
        next.eo[i] = eo[table.eo[i]];
    }
    return next;
}

std::string CubeState::toString() const {
    std::string out = "CP: ";
    for (int i = 0; i < 8;  i++) out += std::to_string(cp[i]) + " ";
    out += "\nCO: ";
    for (int i = 0; i < 8;  i++) out += std::to_string(co[i]) + " ";
    out += "\nEP: ";
    for (int i = 0; i < 12; i++) out += std::to_string(ep[i]) + " ";
    out += "\nEO: ";
    for (int i = 0; i < 12; i++) out += std::to_string(eo[i]) + " ";
    return out;
}