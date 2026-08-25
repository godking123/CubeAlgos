#include "CubeState.h"
#include "MoveTable.h"
#include <sstream>
#include <stdexcept>

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
    const MoveTable& t = MOVE_TABLES[static_cast<int>(m)];
    for (int i = 0; i < 8; i++) {
        next.cp[i] = cp[t.cp[i]];
        next.co[i] = (co[t.cp[i]] + t.co[i]) % 3;
    }
    for (int i = 0; i < 12; i++) {
        next.ep[i] = ep[t.ep[i]];
        next.eo[i] = (eo[t.ep[i]] + t.eo[i]) % 2;
    }
    return next;
}

const char* moveName(Move m) {
    static const char* names[] = {
        "U","U2","U'","R","R2","R'","F","F2","F'",
        "D","D2","D'","L","L2","L'","B","B2","B'"
    };
    return names[static_cast<int>(m)];
}

Move parseMove(const std::string& s) {
    static const char* names[] = {
        "U","U2","U'","R","R2","R'","F","F2","F'",
        "D","D2","D'","L","L2","L'","B","B2","B'"
    };
    for (int i = 0; i < 18; i++)
        if (s == names[i]) return static_cast<Move>(i);
    throw std::invalid_argument("Unknown move: " + s);
}

std::vector<Move> parseSequence(const std::string& s) {
    std::vector<Move> moves;
    std::istringstream ss(s);
    std::string token;
    while (ss >> token) moves.push_back(parseMove(token));
    return moves;
}